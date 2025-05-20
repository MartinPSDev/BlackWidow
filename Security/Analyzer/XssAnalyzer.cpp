/**
 * XssAnalyzer.cpp
 * Implementación de la clase para analizar vulnerabilidades XSS (Cross-Site Scripting)
 */

#include "XssAnalyzer.h"
#include <regex>
#include <iostream>
#include <sstream>

namespace BlackWidow {
namespace Security {
namespace Analyzer {

XssAnalyzer::XssAnalyzer() {
    // Constructor por defecto
}

XssAnalysisResult XssAnalyzer::analyzeResponse(const std::string& response, 
                                             const std::string& payload,
                                             const std::string& originalInput) {
    XssAnalysisResult result;
    
    // Verificar si el payload se refleja en la respuesta
    if (isPayloadReflected(response, payload)) {
        // Verificar si el payload se ejecuta (no solo se refleja)
        if (isPayloadExecuted(response, payload)) {
            result.vulnerable = true;
            result.evidence = response;
            result.payload = payload;
            
            // Determinar el tipo de vulnerabilidad
            // Por defecto asumimos XSS reflejado
            result.vulnerabilityType = "Reflected XSS";
            
            // Analizar el contexto de ejecución
            result.context = analyzeExecutionContext(response, payload);
            
            // Determinar el nivel de impacto
            result.impactLevel = determineImpactLevel(result.vulnerabilityType, result.context);
        }
    }
    
    return result;
}

XssAnalysisResult XssAnalyzer::detectVulnerability(const std::string& url, 
                                                 const std::map<std::string, std::string>& params,
                                                 const std::map<std::string, std::string>& headers) {
    XssAnalysisResult result;
    
    // Obtener payloads básicos para pruebas
    std::vector<std::string> basicPayloads = Payloads::XssPayloads::getBasicPayloads();
    
    // Probar cada parámetro con cada payload
    for (const auto& param : params) {
        std::map<std::string, std::string> testParams = params;
        std::string originalValue = param.second;
        
        for (const auto& payload : basicPayloads) {
            // Modificar el parámetro con el payload
            testParams[param.first] = payload;
            
            // Enviar la petición
            std::string response = sendXssRequest(url, testParams, headers);
            
            // Analizar la respuesta
            XssAnalysisResult tempResult = analyzeResponse(response, payload, originalValue);
            
            if (tempResult.vulnerable) {
                return tempResult; // Retornar al primer indicio de vulnerabilidad
            }
        }
        
        // Si no se encontró vulnerabilidad con payloads básicos, probar con avanzados
        std::vector<std::string> advancedPayloads = Payloads::XssPayloads::getAdvancedPayloads();
        
        for (const auto& payload : advancedPayloads) {
            // Modificar el parámetro con el payload
            testParams[param.first] = payload;
            
            // Enviar la petición
            std::string response = sendXssRequest(url, testParams, headers);
            
            // Analizar la respuesta
            XssAnalysisResult tempResult = analyzeResponse(response, payload, originalValue);
            
            if (tempResult.vulnerable) {
                return tempResult;
            }
        }
        
        // Probar con payloads políglotas
        std::vector<std::string> polyglotsPayloads = Payloads::XssPayloads::getPolyglotPayloads();
        
        for (const auto& payload : polyglotsPayloads) {
            // Modificar el parámetro con el payload
            testParams[param.first] = payload;
            
            // Enviar la petición
            std::string response = sendXssRequest(url, testParams, headers);
            
            // Analizar la respuesta
            XssAnalysisResult tempResult = analyzeResponse(response, payload, originalValue);
            
            if (tempResult.vulnerable) {
                return tempResult;
            }
        }
    }
    
    return result; // No se encontró vulnerabilidad
}

XssAnalysisResult XssAnalyzer::detectDomBasedXss(const std::string& url,
                                               const std::map<std::string, std::string>& headers) {
    XssAnalysisResult result;
    
    // En una implementación real, aquí se cargaría la página y se ejecutaría JavaScript
    // para detectar vulnerabilidades DOM XSS. Para esta implementación, simulamos el proceso.
    
    // Simular carga de la página
    std::map<std::string, std::string> emptyParams;
    std::string pageContent = sendXssRequest(url, emptyParams, headers);
    
    // Buscar patrones vulnerables en el DOM
    std::vector<std::string> vulnerablePatterns = {
        "document.write(",
        "innerHTML",
        "outerHTML",
        "eval(",
        "setTimeout(",
        "setInterval(",
        "location.hash",
        "location.search",
        "document.URL",
        "document.documentURI"
    };
    
    for (const auto& pattern : vulnerablePatterns) {
        if (pageContent.find(pattern) != std::string::npos) {
            // Potencial vulnerabilidad DOM XSS encontrada
            // Intentar explotar con un payload específico
            
            // Generar URL con payload en fragmento o query string
            std::string testUrl = url;
            if (url.find("?") == std::string::npos) {
                testUrl += "?xss=<img src=x onerror=alert('XSS')>";
            } else {
                testUrl += "&xss=<img src=x onerror=alert('XSS')>";
            }
            
            // Simular carga de la página con el payload
            std::string responseWithPayload = sendXssRequest(testUrl, emptyParams, headers);
            
            // Verificar si el payload se ejecuta
            if (isPayloadExecuted(responseWithPayload, "<img src=x onerror=alert('XSS')>")) {
                result.vulnerable = true;
                result.vulnerabilityType = "DOM-based XSS";
                result.context = "JavaScript";
                result.evidence = responseWithPayload;
                result.payload = "<img src=x onerror=alert('XSS')>";
                result.impactLevel = determineImpactLevel("DOM-based XSS", "JavaScript");
                return result;
            }
        }
    }
    
    return result; // No se encontró vulnerabilidad
}

std::string XssAnalyzer::analyzeExecutionContext(const std::string& html, const std::string& payload) {
    // Determinar el contexto de ejecución del payload
    
    // Verificar si el payload está dentro de una etiqueta script
    std::regex scriptRegex("<script[^>]*>([\s\S]*?)" + std::regex::escape(payload) + "([\s\S]*?)</script>", 
                          std::regex::icase);
    if (std::regex_search(html, scriptRegex)) {
        return "JAVASCRIPT";
    }
    
    // Verificar si el payload está dentro de un atributo de evento
    std::regex eventAttrRegex("\s(on\w+)\s*=\s*(['\"])[^'\"]*" + std::regex::escape(payload), 
                             std::regex::icase);
    if (std::regex_search(html, eventAttrRegex)) {
        return "HTML_ATTRIBUTE";
    }
    
    // Verificar si el payload está dentro de una URL
    std::regex urlRegex("(href|src|action)\s*=\s*(['\"])[^'\"]*" + std::regex::escape(payload), 
                       std::regex::icase);
    if (std::regex_search(html, urlRegex)) {
        return "URL";
    }
    
    // Por defecto, asumimos que está en el contexto HTML
    return "HTML_TAG";
}

std::vector<std::string> XssAnalyzer::generateContextSpecificPayloads(const std::string& context) {
    // Convertir el contexto a XssContext para usar la función de XssPayloads
    Payloads::XssContext xssContext;
    
    if (context == "HTML_TAG") {
        xssContext = Payloads::XssContext::HTML_TAG;
    } else if (context == "HTML_ATTRIBUTE") {
        xssContext = Payloads::XssContext::HTML_ATTRIBUTE;
    } else if (context == "JAVASCRIPT") {
        xssContext = Payloads::XssContext::JAVASCRIPT;
    } else if (context == "URL") {
        xssContext = Payloads::XssContext::URL;
    } else {
        // Contexto desconocido, usar payloads básicos
        return Payloads::XssPayloads::getBasicPayloads();
    }
    
    return Payloads::XssPayloads::getContextSpecificPayloads(xssContext);
}

std::string XssAnalyzer::sendXssRequest(const std::string& url, 
                                      const std::map<std::string, std::string>& params,
                                      const std::map<std::string, std::string>& headers) {
    // Nota: En una implementación real, aquí se enviaría una petición HTTP
    // con los parámetros al URL especificado. Para esta implementación,
    // simplemente devolvemos un placeholder.
    
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Type: text/html\r\n\r\n";
    responseStream << "<!DOCTYPE html>\n<html>\n<head>\n<title>Test Page</title>\n</head>\n<body>\n";
    
    // Simular una respuesta basada en los parámetros
    for (const auto& param : params) {
        // Simular reflejo de parámetros en la respuesta
        responseStream << "<div>Parameter " << param.first << ": " << param.second << "</div>\n";
        
        // Simular vulnerabilidad XSS para ciertos payloads
        if (param.second.find("<script>") != std::string::npos ||
            param.second.find("<img src=x onerror=") != std::string::npos ||
            param.second.find("<svg onload=") != std::string::npos) {
            // Simular que el payload se ejecuta (no se escapa correctamente)
            responseStream << param.second << "\n";
        }
    }
    
    responseStream << "</body>\n</html>";
    
    return responseStream.str();
}

bool XssAnalyzer::isPayloadReflected(const std::string& response, const std::string& payload) {
    // Verificar si el payload aparece en la respuesta
    return response.find(payload) != std::string::npos;
}

bool XssAnalyzer::isPayloadExecuted(const std::string& response, const std::string& payload) {
    // En una implementación real, aquí se verificaría si el payload se ejecuta
    // mediante análisis del DOM o ejecución de JavaScript. Para esta implementación,
    // simplemente verificamos si el payload aparece sin escapar en la respuesta.
    
    // Verificar si el payload aparece en un contexto donde se ejecutaría
    if (payload.find("<script>") != std::string::npos && 
        response.find("<script>") != std::string::npos) {
        return true;
    }
    
    if (payload.find("onerror=") != std::string::npos && 
        response.find("onerror=") != std::string::npos) {
        return true;
    }
    
    if (payload.find("onload=") != std::string::npos && 
        response.find("onload=") != std::string::npos) {
        return true;
    }
    
    // Verificar otros patrones de ejecución
    std::vector<std::string> executionPatterns = {
        "javascript:",
        "eval(",
        "setTimeout(",
        "setInterval("
    };
    
    for (const auto& pattern : executionPatterns) {
        if (payload.find(pattern) != std::string::npos && 
            response.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

int XssAnalyzer::determineImpactLevel(const std::string& vulnerabilityType, const std::string& context) {
    // Determinar el nivel de impacto basado en el tipo de vulnerabilidad y el contexto
    
    // Stored XSS tiene mayor impacto que Reflected XSS
    int baseLevel = 0;
    if (vulnerabilityType == "Stored XSS") {
        baseLevel = 4; // Alto impacto
    } else if (vulnerabilityType == "Reflected XSS") {
        baseLevel = 3; // Impacto medio
    } else if (vulnerabilityType == "DOM-based XSS") {
        baseLevel = 3; // Impacto medio
    }
    
    // Ajustar basado en el contexto
    if (context == "JAVASCRIPT") {
        baseLevel += 1; // Mayor impacto en contexto JavaScript
    } else if (context == "HTML_ATTRIBUTE") {
        // Mantener el nivel base
    } else if (context == "URL") {
        baseLevel -= 1; // Menor impacto en contexto URL
    }
    
    // Asegurar que el nivel esté entre 1 y 5
    if (baseLevel < 1) baseLevel = 1;
    if (baseLevel > 5) baseLevel = 5;
    
    return baseLevel;
}

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow