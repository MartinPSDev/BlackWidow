/**
 * SecurityAnalyzer.cpp
 * Implementación de la clase principal que integra todos los analizadores de seguridad
 */

#include "SecurityAnalyzer.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace BlackWidow {
namespace Security {
namespace Analyzer {

/**
 * Constructor
 */
SecurityAnalyzer::SecurityAnalyzer() {
    // Inicializar los analizadores individuales
    xssAnalyzer = std::make_unique<XssAnalyzer>();
    xxeAnalyzer = std::make_unique<XxeAnalyzer>();
    sqlAnalyzer = std::make_unique<SqlInjectionAnalyzer>();
    csrfAnalyzer = std::make_unique<CsrfAnalyzer>();
}

/**
 * Realiza un análisis completo de seguridad en un objetivo
 */
SecurityAnalysisReport SecurityAnalyzer::analyzeTarget(
    const std::string& url, 
    const std::vector<VulnerabilityType>& vulnerabilityTypes,
    const std::map<std::string, std::string>& headers) {
    
    SecurityAnalysisReport report;
    report.targetUrl = url;
    report.scanDate = getCurrentDateTime();
    
    // Verificar si se debe analizar todas las vulnerabilidades
    bool analyzeAll = false;
    for (const auto& type : vulnerabilityTypes) {
        if (type == VulnerabilityType::ALL) {
            analyzeAll = true;
            break;
        }
    }
    
    // Analizar XSS si está especificado o si se analizan todas
    if (analyzeAll || std::find(vulnerabilityTypes.begin(), vulnerabilityTypes.end(), 
                               VulnerabilityType::XSS) != vulnerabilityTypes.end()) {
        // Crear parámetros de prueba para XSS
        std::map<std::string, std::string> xssParams = {
            {"q", "test"},
            {"search", "test"},
            {"id", "1"}
        };
        
        XssAnalysisResult xssResult = xssAnalyzer->detectVulnerability(url, xssParams, headers);
        if (xssResult.vulnerable) {
            report.xssResults.push_back(xssResult);
        }
    }
    
    // Analizar XXE si está especificado o si se analizan todas
    if (analyzeAll || std::find(vulnerabilityTypes.begin(), vulnerabilityTypes.end(), 
                               VulnerabilityType::XXE) != vulnerabilityTypes.end()) {
        XxeAnalysisResult xxeResult = xxeAnalyzer->detectVulnerability(url, headers);
        if (xxeResult.vulnerable) {
            report.xxeResults.push_back(xxeResult);
        }
    }
    
    // Analizar SQL Injection si está especificado o si se analizan todas
    if (analyzeAll || std::find(vulnerabilityTypes.begin(), vulnerabilityTypes.end(), 
                               VulnerabilityType::SQL_INJECTION) != vulnerabilityTypes.end()) {
        // Crear parámetros de prueba para SQLi
        std::map<std::string, std::string> sqlParams = {
            {"id", "1"},
            {"user", "admin"},
            {"product", "123"}
        };
        
        SqlInjectionAnalysisResult sqlResult = sqlAnalyzer->detectVulnerability(url, sqlParams, headers);
        if (sqlResult.vulnerable) {
            report.sqlResults.push_back(sqlResult);
        }
    }
    
    // Analizar CSRF si está especificado o si se analizan todas
    if (analyzeAll || std::find(vulnerabilityTypes.begin(), vulnerabilityTypes.end(), 
                               VulnerabilityType::CSRF) != vulnerabilityTypes.end()) {
        CsrfAnalysisResult csrfResult = csrfAnalyzer->detectVulnerability(url, headers);
        if (csrfResult.vulnerable) {
            report.csrfResults.push_back(csrfResult);
        }
    }
    
    // Actualizar estadísticas del informe
    updateReportStatistics(report);
    
    return report;
}

/**
 * Analiza un formulario en busca de vulnerabilidades XSS y CSRF
 */
SecurityAnalysisReport SecurityAnalyzer::analyzeForm(
    const std::string& url,
    const std::map<std::string, std::string>& headers) {
    
    SecurityAnalysisReport report;
    report.targetUrl = url;
    report.scanDate = getCurrentDateTime();
    
    // Analizar CSRF en el formulario
    CsrfAnalysisResult csrfResult = csrfAnalyzer->detectVulnerability(url, headers);
    if (csrfResult.vulnerable) {
        report.csrfResults.push_back(csrfResult);
    }
    
    // Extraer campos del formulario para probar XSS
    // Nota: En una implementación real, se extraerían los campos del formulario
    // Para esta implementación, usamos campos genéricos
    std::map<std::string, std::string> formParams = {
        {"name", "test"},
        {"email", "test@example.com"},
        {"comment", "test comment"}
    };
    
    // Analizar XSS en los campos del formulario
    XssAnalysisResult xssResult = xssAnalyzer->detectVulnerability(url, formParams, headers);
    if (xssResult.vulnerable) {
        report.xssResults.push_back(xssResult);
    }
    
    // Actualizar estadísticas del informe
    updateReportStatistics(report);
    
    return report;
}

/**
 * Analiza un endpoint XML en busca de vulnerabilidades XXE
 */
SecurityAnalysisReport SecurityAnalyzer::analyzeXmlEndpoint(
    const std::string& url,
    const std::map<std::string, std::string>& headers) {
    
    SecurityAnalysisReport report;
    report.targetUrl = url;
    report.scanDate = getCurrentDateTime();
    
    // Analizar XXE en el endpoint XML
    XxeAnalysisResult xxeResult = xxeAnalyzer->detectVulnerability(url, headers);
    if (xxeResult.vulnerable) {
        report.xxeResults.push_back(xxeResult);
        
        // Si es vulnerable, intentar explotar para leer archivos
        XxeAnalysisResult fileReadResult = xxeAnalyzer->exploitFileRead(url, "/etc/passwd", headers);
        if (fileReadResult.vulnerable) {
            report.xxeResults.push_back(fileReadResult);
        }
        
        // Intentar SSRF a través de XXE
        XxeAnalysisResult ssrfResult = xxeAnalyzer->exploitSsrf(url, "http://localhost:8080", headers);
        if (ssrfResult.vulnerable) {
            report.xxeResults.push_back(ssrfResult);
        }
    }
    
    // Actualizar estadísticas del informe
    updateReportStatistics(report);
    
    return report;
}

/**
 * Analiza un endpoint de API en busca de vulnerabilidades de inyección
 */
SecurityAnalysisReport SecurityAnalyzer::analyzeApiEndpoint(
    const std::string& url,
    const std::map<std::string, std::string>& params,
    const std::map<std::string, std::string>& headers) {
    
    SecurityAnalysisReport report;
    report.targetUrl = url;
    report.scanDate = getCurrentDateTime();
    
    // Analizar SQL Injection en el endpoint de API
    SqlInjectionAnalysisResult sqlResult = sqlAnalyzer->detectVulnerability(url, params, headers);
    if (sqlResult.vulnerable) {
        report.sqlResults.push_back(sqlResult);
        
        // Si es vulnerable, intentar detectar el tipo de base de datos
        // Asumimos que el primer parámetro es vulnerable para simplificar
        std::string vulnerableParam = params.begin()->first;
        Payloads::DatabaseType dbType = sqlAnalyzer->detectDatabaseType(url, vulnerableParam, headers);
        
        // Extraer información de la base de datos
        SqlInjectionAnalysisResult dbInfoResult = sqlAnalyzer->extractDatabaseInfo(
            url, vulnerableParam, dbType, headers);
        if (dbInfoResult.vulnerable) {
            report.sqlResults.push_back(dbInfoResult);
        }
        
        // Probar inyección SQL ciega
        SqlInjectionAnalysisResult blindResult = sqlAnalyzer->detectBlindInjection(url, params, headers);
        if (blindResult.vulnerable) {
            report.sqlResults.push_back(blindResult);
        }
    }
    
    // Analizar XSS en el endpoint de API
    XssAnalysisResult xssResult = xssAnalyzer->detectVulnerability(url, params, headers);
    if (xssResult.vulnerable) {
        report.xssResults.push_back(xssResult);
    }
    
    // Actualizar estadísticas del informe
    updateReportStatistics(report);
    
    return report;
}

/**
 * Genera un informe en formato HTML del análisis de seguridad
 */
std::string SecurityAnalyzer::generateHtmlReport(const SecurityAnalysisReport& report) {
    std::stringstream html;
    
    // Encabezado HTML
    html << "<!DOCTYPE html>\n"
         << "<html lang=\"es\">\n"
         << "<head>\n"
         << "    <meta charset=\"UTF-8\">\n"
         << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
         << "    <title>Informe de Seguridad - BlackWidow</title>\n"
         << "    <style>\n"
         << "        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }\n"
         << "        h1, h2, h3 { color: #333; }\n"
         << "        .container { max-width: 1200px; margin: 0 auto; }\n"
         << "        .header { background-color: #f5f5f5; padding: 20px; border-radius: 5px; margin-bottom: 20px; }\n"
         << "        .summary { display: flex; justify-content: space-between; margin-bottom: 20px; }\n"
         << "        .summary-box { background-color: #f9f9f9; padding: 15px; border-radius: 5px; width: 23%; text-align: center; }\n"
         << "        .critical { border-left: 5px solid #d9534f; }\n"
         << "        .high { border-left: 5px solid #f0ad4e; }\n"
         << "        .medium { border-left: 5px solid #5bc0de; }\n"
         << "        .low { border-left: 5px solid #5cb85c; }\n"
         << "        .vulnerability { background-color: #fff; border: 1px solid #ddd; padding: 15px; margin-bottom: 10px; border-radius: 5px; }\n"
         << "        .evidence { background-color: #f5f5f5; padding: 10px; border-radius: 3px; font-family: monospace; overflow-x: auto; }\n"
         << "    </style>\n"
         << "</head>\n"
         << "<body>\n"
         << "    <div class=\"container\">\n"
         << "        <div class=\"header\">\n"
         << "            <h1>Informe de Análisis de Seguridad</h1>\n"
         << "            <p><strong>URL Objetivo:</strong> " << report.targetUrl << "</p>\n"
         << "            <p><strong>Fecha de Análisis:</strong> " << report.scanDate << "</p>\n"
         << "        </div>\n";
    
    // Resumen de vulnerabilidades
    html << "        <div class=\"summary\">\n"
         << "            <div class=\"summary-box critical\">\n"
         << "                <h3>Críticas</h3>\n"
         << "                <h2>" << report.criticalVulnerabilities << "</h2>\n"
         << "            </div>\n"
         << "            <div class=\"summary-box high\">\n"
         << "                <h3>Altas</h3>\n"
         << "                <h2>" << report.highVulnerabilities << "</h2>\n"
         << "            </div>\n"
         << "            <div class=\"summary-box medium\">\n"
         << "                <h3>Medias</h3>\n"
         << "                <h2>" << report.mediumVulnerabilities << "</h2>\n"
         << "            </div>\n"
         << "            <div class=\"summary-box low\">\n"
         << "                <h3>Bajas</h3>\n"
         << "                <h2>" << report.lowVulnerabilities << "</h2>\n"
         << "            </div>\n"
         << "        </div>\n";
    
    // Sección de vulnerabilidades XSS
    if (!report.xssResults.empty()) {
        html << "        <h2>Vulnerabilidades XSS</h2>\n";
        for (const auto& result : report.xssResults) {
            html << "        <div class=\"vulnerability\">\n"
                 << "            <h3>" << result.vulnerabilityType << " (Impacto: " << result.impactLevel << "/5)</h3>\n"
                 << "            <p><strong>Contexto:</strong> " << result.context << "</p>\n"
                 << "            <p><strong>Payload:</strong> " << result.payload << "</p>\n"
                 << "            <div class=\"evidence\">\n"
                 << "                <p><strong>Evidencia:</strong></p>\n"
                 << "                <pre>" << result.evidence << "</pre>\n"
                 << "            </div>\n"
                 << "        </div>\n";
        }
    }
    
    // Sección de vulnerabilidades XXE
    if (!report.xxeResults.empty()) {
        html << "        <h2>Vulnerabilidades XXE</h2>\n";
        for (const auto& result : report.xxeResults) {
            html << "        <div class=\"vulnerability\">\n"
                 << "            <h3>" << result.vulnerabilityType << "</h3>\n"
                 << "            <p><strong>Payload:</strong> " << result.payload << "</p>\n"
                 << "            <div class=\"evidence\">\n"
                 << "                <p><strong>Evidencia:</strong></p>\n"
                 << "                <pre>" << result.evidence << "</pre>\n"
                 << "            </div>\n";
            
            // Mostrar datos extraídos si hay
            if (!result.extractedData.empty()) {
                html << "            <div class=\"evidence\">\n"
                     << "                <p><strong>Datos Extraídos:</strong></p>\n";
                for (const auto& data : result.extractedData) {
                    html << "                <p>" << data.first << ": " << data.second << "</p>\n";
                }
                html << "            </div>\n";
            }
            
            html << "        </div>\n";
        }
    }
    
    // Sección de vulnerabilidades SQL Injection
    if (!report.sqlResults.empty()) {
        html << "        <h2>Vulnerabilidades de Inyección SQL</h2>\n";
        for (const auto& result : report.sqlResults) {
            html << "        <div class=\"vulnerability\">\n"
                 << "            <h3>" << result.vulnerabilityType << " (Impacto: " << result.impactLevel << "/5)</h3>\n"
                 << "            <p><strong>Base de Datos:</strong> " << result.databaseType << "</p>\n"
                 << "            <p><strong>Payload:</strong> " << result.payload << "</p>\n"
                 << "            <div class=\"evidence\">\n"
                 << "                <p><strong>Evidencia:</strong></p>\n"
                 << "                <pre>" << result.evidence << "</pre>\n"
                 << "            </div>\n";
            
            // Mostrar datos extraídos si hay
            if (!result.extractedData.empty()) {
                html << "            <div class=\"evidence\">\n"
                     << "                <p><strong>Datos Extraídos:</strong></p>\n";
                for (const auto& data : result.extractedData) {
                    html << "                <p>" << data.first << ": " << data.second << "</p>\n";
                }
                html << "            </div>\n";
            }
            
            html << "        </div>\n";
        }
    }
    
    // Sección de vulnerabilidades CSRF
    if (!report.csrfResults.empty()) {
        html << "        <h2>Vulnerabilidades CSRF</h2>\n";
        for (const auto& result : report.csrfResults) {
            html << "        <div class=\"vulnerability\">\n"
                 << "            <h3>" << result.vulnerabilityType << " (Impacto: " << result.impactLevel << "/5)</h3>\n"
                 << "            <p><strong>Payload:</strong> " << result.payload << "</p>\n"
                 << "            <div class=\"evidence\">\n"
                 << "                <p><strong>Evidencia:</strong></p>\n"
                 << "                <pre>" << result.evidence << "</pre>\n"
                 << "            </div>\n";
            
            // Mostrar funcionalidades afectadas si hay
            if (!result.affectedFunctions.empty()) {
                html << "            <div class=\"evidence\">\n"
                     << "                <p><strong>Funcionalidades Afectadas:</strong></p>\n";
                for (const auto& func : result.affectedFunctions) {
                    html << "                <p>" << func.first << ": " << func.second << "</p>\n";
                }
                html << "            </div>\n";
            }
            
            html << "        </div>\n";
        }
    }
    
    // Pie de página
    html << "        <div class=\"footer\">\n"
         << "            <p>Generado por BlackWidow Security Analyzer</p>\n"
         << "        </div>\n"
         << "    </div>\n"
         << "</body>\n"
         << "</html>";
    
    return html.str();
}

/**
 * Genera un informe en formato JSON del análisis de seguridad
 */
std::string SecurityAnalyzer::generateJsonReport(const SecurityAnalysisReport& report) {
    nlohmann::json jsonReport;
    
    // Información general
    jsonReport["target_url"] = report.targetUrl;
    jsonReport["scan_date"] = report.scanDate;
    jsonReport["vulnerabilities_found"] = report.vulnerabilitiesFound;
    
    // Estadísticas
    jsonReport["statistics"] = {
        {"total_vulnerabilities", report.totalVulnerabilities},
        {"critical_vulnerabilities", report.criticalVulnerabilities},
        {"high_vulnerabilities", report.highVulnerabilities},
        {"medium_vulnerabilities", report.mediumVulnerabilities},
        {"low_vulnerabilities", report.lowVulnerabilities}
    };
    
    // Vulnerabilidades XSS
    nlohmann::json xssVulnerabilities = nlohmann::json::array();
    for (const auto& result : report.xssResults) {
        nlohmann::json xssResult = {
            {"vulnerable", result.vulnerable},
            {"vulnerability_type", result.vulnerabilityType},
            {"context", result.context},
            {"evidence", result.evidence},
            {"payload", result.payload},
            {"impact_level", result.impactLevel}
        };
        xssVulnerabilities.push_back(xssResult);
    }
    jsonReport["xss_vulnerabilities"] = xssVulnerabilities;
    
    // Vulnerabilidades XXE
    nlohmann::json xxeVulnerabilities = nlohmann::json::array();
    for (const auto& result : report.xxeResults) {
        nlohmann::json xxeResult = {
            {"vulnerable", result.vulnerable},
            {"vulnerability_type", result.vulnerabilityType},
            {"evidence", result.evidence},
            {"payload", result.payload},
            {"extracted_data", result.extractedData}
        };
        xxeVulnerabilities.push_back(xxeResult);
    }
    jsonReport["xxe_vulnerabilities"] = xxeVulnerabilities;
    
    // Vulnerabilidades SQL Injection
    nlohmann::json sqlVulnerabilities = nlohmann::json::array();
    for (const auto& result : report.sqlResults) {
        nlohmann::json sqlResult = {
            {"vulnerable", result.vulnerable},
            {"vulnerability_type", result.vulnerabilityType},
            {"database_type", result.databaseType},
            {"evidence", result.evidence},
            {"payload", result.payload},
            {"extracted_data", result.extractedData},
            {"impact_level", result.impactLevel}
        };
        sqlVulnerabilities.push_back(sqlResult);
    }
    jsonReport["sql_injection_vulnerabilities"] = sqlVulnerabilities;
    
    // Vulnerabilidades CSRF
    nlohmann::json csrfVulnerabilities = nlohmann::json::array();
    for (const auto& result : report.csrfResults) {
        nlohmann::json csrfResult = {
            {"vulnerable", result.vulnerable},
            {"vulnerability_type", result.vulnerabilityType},
            {"evidence", result.evidence},
            {"payload", result.payload},
            {"impact_level", result.impactLevel},
            {"affected_functions", result.affectedFunctions}
        };
        csrfVulnerabilities.push_back(csrfResult);
    }
    jsonReport["csrf_vulnerabilities"] = csrfVulnerabilities;
    
    return jsonReport.dump(4); // Formatear con indentación de 4 espacios
}

/**
 * Actualiza las estadísticas del informe basado en los resultados
 */
void SecurityAnalyzer::updateReportStatistics(SecurityAnalysisReport& report) {
    // Reiniciar contadores
    report.totalVulnerabilities = 0;
    report.criticalVulnerabilities = 0;
    report.highVulnerabilities = 0;
    report.mediumVulnerabilities = 0;
    report.lowVulnerabilities = 0;
    
    // Contar vulnerabilidades XSS
    for (const auto& result : report.xssResults) {
        report.totalVulnerabilities++;
        
        // Clasificar por nivel de impacto
        switch (result.impactLevel) {
            case 5:
                report.criticalVulnerabilities++;
                break;
            case 4:
                report.highVulnerabilities++;
                break;
            case 3:
                report.mediumVulnerabilities++;
                break;
            default: // 1-2
                report.lowVulnerabilities++;
                break;
        }
    }
    
    // Contar vulnerabilidades SQL Injection
    for (const auto& result : report.sqlResults) {
        report.totalVulnerabilities++;
        
        // Clasificar por nivel de impacto
        switch (result.impactLevel) {
            case 5:
                report.criticalVulnerabilities++;
                break;
            case 4:
                report.highVulnerabilities++;
                break;
            case 3:
                report.mediumVulnerabilities++;
                break;
            default: // 1-2
                report.lowVulnerabilities++;
                break;
        }
    }
    
    // Contar vulnerabilidades CSRF
    for (const auto& result : report.csrfResults) {
        report.totalVulnerabilities++;
        
        // Clasificar por nivel de impacto
        switch (result.impactLevel) {
            case 5:
                report.criticalVulnerabilities++;
                break;
            case 4:
                report.highVulnerabilities++;
                break;
            case 3:
                report.mediumVulnerabilities++;
                break;
            default: // 1-2
                report.lowVulnerabilities++;
                break;
        }
    }
    
    // Contar vulnerabilidades XXE (asumimos que todas son críticas)
    for (const auto& result : report.xxeResults) {
        report.totalVulnerabilities++;
        report.criticalVulnerabilities++; // XXE generalmente es crítico
    }
    
    // Actualizar el indicador de vulnerabilidades encontradas
    report.vulnerabilitiesFound = (report.totalVulnerabilities > 0);
}

/**
 * Obtiene la fecha y hora actual en formato string
 */
std::string SecurityAnalyzer::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow