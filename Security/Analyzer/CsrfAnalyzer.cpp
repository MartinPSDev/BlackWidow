/**
 * CsrfAnalyzer.cpp
 * Implementación de la clase para analizar vulnerabilidades CSRF (Cross-Site Request Forgery)
 */

#include "CsrfAnalyzer.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <random>

namespace BlackWidow {
namespace Security {
namespace Analyzer {

CsrfAnalyzer::CsrfAnalyzer() {
    // Constructor por defecto
}

CsrfAnalysisResult CsrfAnalyzer::analyzeResponse(const std::string& response) {
    CsrfAnalysisResult result;
    
    // Extraer formularios de la respuesta
    std::vector<std::string> forms = extractForms(response);
    
    // Analizar cada formulario
    for (const auto& form : forms) {
        if (!hasCSRFProtection(form)) {
            // Formulario vulnerable encontrado
            result.vulnerable = true;
            result.evidence = form;
            
            // Extraer información del formulario
            std::smatch actionMatch;
            std::regex actionRegex("action=\"([^\"]+)\"", std::regex::icase);
            std::string formAction = "";
            if (std::regex_search(form, actionMatch, actionRegex) && actionMatch.size() > 1) {
                formAction = actionMatch[1].str();
            }
            
            std::smatch methodMatch;
            std::regex methodRegex("method=\"([^\"]+)\"", std::regex::icase);
            std::string formMethod = "GET"; // Por defecto
            if (std::regex_search(form, methodMatch, methodRegex) && methodMatch.size() > 1) {
                formMethod = methodMatch[1].str();
            }
            
            // Extraer campos del formulario
            std::map<std::string, std::string> formFields = extractFormFields(form);
            
            // Determinar tipo de vulnerabilidad
            if (formMethod == "POST" || formMethod == "post") {
                result.vulnerabilityType = "POST CSRF";
            } else {
                result.vulnerabilityType = "GET CSRF";
            }
            
            // Determinar nivel de impacto
            result.impactLevel = determineImpactLevel(formAction, formMethod, formFields);
            
            // Generar payload de ejemplo
            Payloads::CsrfMethod csrfMethod = (formMethod == "POST" || formMethod == "post") ? 
                                            Payloads::CsrfMethod::POST : 
                                            Payloads::CsrfMethod::GET;
            result.payload = Payloads::CsrfPayloads::generateAutoSubmitPayload(csrfMethod, formAction, formFields);
            
            // Registrar funcionalidades afectadas
            if (!formAction.empty()) {
                result.affectedFunctions["form_action"] = formAction;
            }
            
            if (!formFields.empty()) {
                std::ostringstream fieldsStr;
                for (const auto& field : formFields) {
                    fieldsStr << field.first << ", ";
                }
                result.affectedFunctions["form_fields"] = fieldsStr.str();
            }
            
            // Encontramos una vulnerabilidad, no necesitamos seguir buscando
            break;
        }
    }
    
    return result;
}

CsrfAnalysisResult CsrfAnalyzer::detectVulnerability(const std::string& url, 
                                                   const std::map<std::string, std::string>& headers) {
    // Obtener la página con el formulario
    std::string response = sendFormRequest(url, headers);
    
    // Analizar la respuesta
    CsrfAnalysisResult result = analyzeResponse(response);
    
    return result;
}

std::string CsrfAnalyzer::generateExploitPayload(const std::string& targetUrl, 
                                               Payloads::CsrfMethod method,
                                               const std::map<std::string, std::string>& params,
                                               bool autoSubmit) {
    // Utilizar la clase CsrfPayloads para generar el payload
    if (autoSubmit) {
        return Payloads::CsrfPayloads::generateAutoSubmitPayload(method, targetUrl, params);
    } else {
        return Payloads::CsrfPayloads::generateCsrfPayload(method, targetUrl, params);
    }
}

bool CsrfAnalyzer::isTokenSecure(const std::string& token) {
    // Verificar si el token es lo suficientemente seguro
    
    // Verificar longitud mínima
    if (token.length() < 10) {
        return false;
    }
    
    // Verificar entropía (caracteres variados)
    bool hasLowercase = false;
    bool hasUppercase = false;
    bool hasDigit = false;
    bool hasSpecial = false;
    
    for (char c : token) {
        if (islower(c)) hasLowercase = true;
        else if (isupper(c)) hasUppercase = true;
        else if (isdigit(c)) hasDigit = true;
        else hasSpecial = true;
    }
    
    // Al menos debe tener 2 tipos de caracteres
    int charTypes = hasLowercase + hasUppercase + hasDigit + hasSpecial;
    if (charTypes < 2) {
        return false;
    }
    
    // Verificar que no sea un patrón predecible
    if (token.find("123") != std::string::npos || 
        token.find("abc") != std::string::npos || 
        token.find("xyz") != std::string::npos) {
        return false;
    }
    
    return true;
}

CsrfAnalysisResult CsrfAnalyzer::analyzeCookieSecurity(const std::map<std::string, std::string>& cookies) {
    CsrfAnalysisResult result;
    
    // Verificar si hay cookies de sesión sin flags de seguridad
    bool hasSecureCookie = false;
    bool hasInsecureCookie = false;
    std::string insecureCookieEvidence;
    
    for (const auto& cookie : cookies) {
        // Verificar si es una cookie de sesión
        if (cookie.first.find("session") != std::string::npos || 
            cookie.first.find("auth") != std::string::npos || 
            cookie.first.find("token") != std::string::npos || 
            cookie.first.find("id") != std::string::npos) {
            
            // Verificar flags de seguridad
            if (cookie.second.find("HttpOnly") == std::string::npos || 
                cookie.second.find("SameSite=Strict") == std::string::npos || 
                cookie.second.find("SameSite=Lax") == std::string::npos) {
                
                hasInsecureCookie = true;
                insecureCookieEvidence += "Cookie insegura: " + cookie.first + 
                                         " (falta HttpOnly o SameSite)\n";
            } else {
                hasSecureCookie = true;
            }
        }
    }
    
    // Determinar resultado
    if (hasInsecureCookie) {
        result.vulnerable = true;
        result.vulnerabilityType = "Insecure Cookies (CSRF Risk)";
        result.evidence = insecureCookieEvidence;
        result.impactLevel = 3; // Impacto medio
    }
    
    return result;
}

std::string CsrfAnalyzer::sendFormRequest(const std::string& url, 
                                        const std::map<std::string, std::string>& headers) {
    // Nota: En una implementación real, aquí se enviaría una petición HTTP
    // al URL especificado. Para esta implementación, simplemente devolvemos un placeholder.
    
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Type: text/html\r\n\r\n";
    responseStream << "<!DOCTYPE html>\n<html>\n<head>\n<title>Test Form</title>\n</head>\n<body>\n";
    
    // Simular diferentes tipos de formularios
    
    // 1. Formulario protegido con token CSRF
    responseStream << "<h2>Formulario Protegido</h2>\n";
    responseStream << "<form action=\"/protected/update-profile\" method=\"POST\">\n";
    responseStream << "  <input type=\"hidden\" name=\"csrf_token\" value=\"a1b2c3d4e5f6g7h8i9j0\">\n";
    responseStream << "  <input type=\"text\" name=\"username\" value=\"usuario1\">\n";
    responseStream << "  <input type=\"email\" name=\"email\" value=\"usuario@example.com\">\n";
    responseStream << "  <input type=\"submit\" value=\"Actualizar\">\n";
    responseStream << "</form>\n\n";
    
    // 2. Formulario vulnerable (sin token CSRF)
    responseStream << "<h2>Formulario Vulnerable</h2>\n";
    responseStream << "<form action=\"/vulnerable/change-password\" method=\"POST\">\n";
    responseStream << "  <input type=\"password\" name=\"new_password\" placeholder=\"Nueva contraseña\">\n";
    responseStream << "  <input type=\"password\" name=\"confirm_password\" placeholder=\"Confirmar contraseña\">\n";
    responseStream << "  <input type=\"submit\" value=\"Cambiar Contraseña\">\n";
    responseStream << "</form>\n\n";
    
    // 3. Formulario GET vulnerable
    responseStream << "<h2>Formulario GET Vulnerable</h2>\n";
    responseStream << "<form action=\"/vulnerable/transfer\" method=\"GET\">\n";
    responseStream << "  <input type=\"text\" name=\"to_account\" value=\"12345\">\n";
    responseStream << "  <input type=\"number\" name=\"amount\" value=\"100\">\n";
    responseStream << "  <input type=\"submit\" value=\"Transferir\">\n";
    responseStream << "</form>\n";
    
    responseStream << "</body>\n</html>";
    
    return responseStream.str();
}

std::vector<std::string> CsrfAnalyzer::extractForms(const std::string& response) {
    std::vector<std::string> forms;
    
    // Extraer todos los formularios usando regex
    std::regex formRegex("<form[^>]*>([\s\S]*?)</form>", std::regex::icase);
    
    std::string::const_iterator searchStart(response.begin());
    std::smatch matches;
    
    while (std::regex_search(searchStart, response.end(), matches, formRegex)) {
        if (matches.size() > 0) {
            forms.push_back(matches[0].str());
        }
        searchStart = matches.suffix().first;
    }
    
    return forms;
}

bool CsrfAnalyzer::hasCSRFProtection(const std::string& formHtml) {
    // Verificar si el formulario tiene algún tipo de protección CSRF
    
    // 1. Buscar token CSRF en campos ocultos
    std::regex csrfTokenRegex("<input[^>]*name=\"(csrf|_token|xsrf|authenticity_token)[^\"]*\"[^>]*>", 
                             std::regex::icase);
    if (std::regex_search(formHtml, csrfTokenRegex)) {
        return true;
    }
    
    // 2. Buscar token CSRF en la URL
    std::regex csrfUrlRegex("action=\"[^\"]*\?(csrf|_token|xsrf|authenticity_token)=[^\"]*\"", 
                          std::regex::icase);
    if (std::regex_search(formHtml, csrfUrlRegex)) {
        return true;
    }
    
    // 3. Buscar atributos de seguridad en el formulario
    std::regex securityAttrRegex("<form[^>]*(data-csrf|data-token)[^>]*>", std::regex::icase);
    if (std::regex_search(formHtml, securityAttrRegex)) {
        return true;
    }
    
    return false;
}

std::map<std::string, std::string> CsrfAnalyzer::extractFormFields(const std::string& formHtml) {
    std::map<std::string, std::string> fields;
    
    // Extraer todos los campos de entrada
    std::regex inputRegex("<input[^>]*name=\"([^\"]+)\"[^>]*value=\"([^\"]*)\"", std::regex::icase);
    
    std::string::const_iterator searchStart(formHtml.begin());
    std::smatch matches;
    
    while (std::regex_search(searchStart, formHtml.end(), matches, inputRegex)) {
        if (matches.size() > 2) {
            fields[matches[1].str()] = matches[2].str();
        }
        searchStart = matches.suffix().first;
    }
    
    // Extraer campos select
    std::regex selectRegex("<select[^>]*name=\"([^\"]+)\"[^>]*>([\s\S]*?)</select>", std::regex::icase);
    searchStart = formHtml.begin();
    
    while (std::regex_search(searchStart, formHtml.end(), matches, selectRegex)) {
        if (matches.size() > 2) {
            // Buscar la opción seleccionada
            std::string selectContent = matches[2].str();
            std::regex selectedOptionRegex("<option[^>]*selected[^>]*value=\"([^\"]+)\"", std::regex::icase);
            std::smatch optionMatches;
            
            if (std::regex_search(selectContent, optionMatches, selectedOptionRegex) && optionMatches.size() > 1) {
                fields[matches[1].str()] = optionMatches[1].str();
            } else {
                // Si no hay opción seleccionada, usar la primera opción
                std::regex firstOptionRegex("<option[^>]*value=\"([^\"]+)\"", std::regex::icase);
                if (std::regex_search(selectContent, optionMatches, firstOptionRegex) && optionMatches.size() > 1) {
                    fields[matches[1].str()] = optionMatches[1].str();
                } else {
                    fields[matches[1].str()] = "";
                }
            }
        }
        searchStart = matches.suffix().first;
    }
    
    // Extraer campos textarea
    std::regex textareaRegex("<textarea[^>]*name=\"([^\"]+)\"[^>]*>([\s\S]*?)</textarea>", std::regex::icase);
    searchStart = formHtml.begin();
    
    while (std::regex_search(searchStart, formHtml.end(), matches, textareaRegex)) {
        if (matches.size() > 2) {
            fields[matches[1].str()] = matches[2].str();
        }
        searchStart = matches.suffix().first;
    }
    
    return fields;
}

int CsrfAnalyzer::determineImpactLevel(const std::string& formAction, 
                                     const std::string& formMethod,
                                     const std::map<std::string, std::string>& formFields) {
    // Determinar el nivel de impacto basado en la acción del formulario y sus campos
    
    int baseLevel = 3; // Impacto medio por defecto
    
    // Verificar si la acción parece crítica
    std::vector<std::string> criticalActions = {
        "password", "contraseña", "delete", "eliminar", "borrar",
        "transfer", "transferir", "pago", "payment", "admin",
        "config", "settings", "account", "cuenta", "user", "usuario"
    };
    
    for (const auto& action : criticalActions) {
        if (formAction.find(action) != std::string::npos) {
            baseLevel += 1;
            break;
        }
    }
    
    // Verificar si los campos parecen críticos
    std::vector<std::string> criticalFields = {
        "password", "contraseña", "token", "key", "clave", "secret",
        "admin", "root", "amount", "monto", "cantidad", "account", "cuenta"
    };
    
    for (const auto& field : formFields) {
        for (const auto& criticalField : criticalFields) {
            if (field.first.find(criticalField) != std::string::npos) {
                baseLevel += 1;
                break;
            }
        }
    }
    
    // Ajustar por método HTTP
    if (formMethod == "POST" || formMethod == "post") {
        baseLevel += 1; // POST es más crítico que GET
    }
    
    // Asegurar que el nivel esté entre 1 y 5
    if (baseLevel < 1) baseLevel = 1;
    if (baseLevel > 5) baseLevel = 5;
    
    return baseLevel;
}

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow