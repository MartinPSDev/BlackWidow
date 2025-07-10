/**
 * SecurityAnalyzer.h
 * Definición de la clase principal que integra todos los analizadores de seguridad
 */

#ifndef BLACKWIDOW_SECURITY_ANALYZER_MAIN_H
#define BLACKWIDOW_SECURITY_ANALYZER_MAIN_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "XxeAnalyzer.h"
#include "XssAnalyzer.h"
#include "SqlInjectionAnalyzer.h"
#include "CsrfAnalyzer.h"

namespace BlackWidow {
namespace Security {
namespace Analyzer {

/**
 * Enumeración de tipos de vulnerabilidades
 */
enum class VulnerabilityType {
    XSS,
    XXE,
    SQL_INJECTION,
    CSRF,
    ALL
};

/**
 * Estructura para almacenar un informe completo de análisis de seguridad
 */
struct SecurityAnalysisReport {
    std::string targetUrl;                // URL objetivo analizada
    std::string scanDate;                 // Fecha y hora del análisis
    bool vulnerabilitiesFound;            // Indica si se encontraron vulnerabilidades
    
    std::vector<XssAnalysisResult> xssResults;             // Resultados de análisis XSS
    std::vector<XxeAnalysisResult> xxeResults;             // Resultados de análisis XXE
    std::vector<SqlInjectionAnalysisResult> sqlResults;    // Resultados de análisis SQLi
    std::vector<CsrfAnalysisResult> csrfResults;           // Resultados de análisis CSRF
    
    // Estadísticas generales
    int totalVulnerabilities;             // Número total de vulnerabilidades
    int criticalVulnerabilities;          // Vulnerabilidades críticas (impacto 5)
    int highVulnerabilities;              // Vulnerabilidades altas (impacto 4)
    int mediumVulnerabilities;            // Vulnerabilidades medias (impacto 3)
    int lowVulnerabilities;               // Vulnerabilidades bajas (impacto 1-2)
    
    SecurityAnalysisReport() : vulnerabilitiesFound(false), totalVulnerabilities(0),
                              criticalVulnerabilities(0), highVulnerabilities(0),
                              mediumVulnerabilities(0), lowVulnerabilities(0) {}
};

/**
 * Clase principal que integra todos los analizadores de seguridad
 */
class SecurityAnalyzer {
public:
    /**
     * Constructor
     */
    SecurityAnalyzer();
    
    /**
     * Realiza un análisis completo de seguridad en un objetivo
     * @param url URL del objetivo a analizar
     * @param vulnerabilityTypes Tipos de vulnerabilidades a analizar (por defecto, todas)
     * @param headers Cabeceras HTTP a incluir en las peticiones
     * @return Informe completo del análisis
     */
    SecurityAnalysisReport analyzeTarget(const std::string& url, 
                                        const std::vector<VulnerabilityType>& vulnerabilityTypes = {VulnerabilityType::ALL},
                                        const std::map<std::string, std::string>& headers = {});
    
    /**
     * Analiza un formulario en busca de vulnerabilidades XSS y CSRF
     * @param url URL del formulario a analizar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Informe del análisis
     */
    SecurityAnalysisReport analyzeForm(const std::string& url,
                                      const std::map<std::string, std::string>& headers = {});
    
    /**
     * Analiza un endpoint XML en busca de vulnerabilidades XXE
     * @param url URL del endpoint XML a analizar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Informe del análisis
     */
    SecurityAnalysisReport analyzeXmlEndpoint(const std::string& url,
                                            const std::map<std::string, std::string>& headers = {});
    
    /**
     * Analiza un endpoint de API en busca de vulnerabilidades de inyección
     * @param url URL del endpoint de API a analizar
     * @param params Parámetros a probar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Informe del análisis
     */
    SecurityAnalysisReport analyzeApiEndpoint(const std::string& url,
                                            const std::map<std::string, std::string>& params,
                                            const std::map<std::string, std::string>& headers = {});
    
    /**
     * Genera un informe en formato HTML del análisis de seguridad
     * @param report Informe de análisis a convertir a HTML
     * @return Cadena HTML con el informe
     */
    std::string generateHtmlReport(const SecurityAnalysisReport& report);
    
    /**
     * Genera un informe en formato JSON del análisis de seguridad
     * @param report Informe de análisis a convertir a JSON
     * @return Cadena JSON con el informe
     */
    std::string generateJsonReport(const SecurityAnalysisReport& report);
    
    /**
     * Realiza análisis avanzado de SQL injection con técnicas específicas
     * @param url URL del objetivo a analizar
     * @param params Parámetros base para las pruebas
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Informe del análisis avanzado de SQL injection
     */
    SecurityAnalysisReport performAdvancedSqlAnalysis(
        const std::string& url,
        const std::map<std::string, std::string>& params,
        const std::map<std::string, std::string>& headers = {});
    
    /**
     * Realiza análisis avanzado de XSS con técnicas de evasión
     * @param url URL del objetivo a analizar
     * @param params Parámetros base para las pruebas
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Informe del análisis avanzado de XSS
     */
    SecurityAnalysisReport performAdvancedXssAnalysis(
        const std::string& url,
        const std::map<std::string, std::string>& params,
        const std::map<std::string, std::string>& headers = {});

private:
    std::unique_ptr<XssAnalyzer> xssAnalyzer;              // Analizador de XSS
    std::unique_ptr<XxeAnalyzer> xxeAnalyzer;              // Analizador de XXE
    std::unique_ptr<SqlInjectionAnalyzer> sqlAnalyzer;     // Analizador de SQLi
    std::unique_ptr<CsrfAnalyzer> csrfAnalyzer;            // Analizador de CSRF
    
    /**
     * Actualiza las estadísticas del informe basado en los resultados
     * @param report Informe a actualizar
     */
    void updateReportStatistics(SecurityAnalysisReport& report);
    
    /**
     * Escapa caracteres especiales para JSON
     * @param input Cadena a escapar
     * @return Cadena escapada para JSON
     */
    std::string escapeJsonString(const std::string& input);
    
    /**
     * Obtiene la fecha y hora actual en formato string
     * @return Fecha y hora actual
     */
    std::string getCurrentDateTime();
};

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_ANALYZER_MAIN_H