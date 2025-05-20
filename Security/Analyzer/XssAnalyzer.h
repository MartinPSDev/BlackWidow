/**
 * XssAnalyzer.h
 * Definición de la clase para analizar vulnerabilidades XSS (Cross-Site Scripting)
 */

#ifndef BLACKWIDOW_SECURITY_ANALYZER_XSS_H
#define BLACKWIDOW_SECURITY_ANALYZER_XSS_H

#include <string>
#include <vector>
#include <map>
#include "../Payloads/XssPayloads.h"

namespace BlackWidow {
namespace Security {
namespace Analyzer {

/**
 * Estructura para almacenar los resultados del análisis XSS
 */
struct XssAnalysisResult {
    bool vulnerable;                  // Indica si el objetivo es vulnerable a XSS
    std::string vulnerabilityType;    // Tipo de vulnerabilidad XSS (Reflected, Stored, DOM)
    std::string context;              // Contexto donde se ejecuta el XSS (HTML, JS, URL, etc.)
    std::string evidence;             // Evidencia de la vulnerabilidad
    std::string payload;              // Payload que detectó la vulnerabilidad
    int impactLevel;                  // Nivel de impacto (1-5)
    
    XssAnalysisResult() : vulnerable(false), impactLevel(0) {}
};

/**
 * Clase para analizar vulnerabilidades XSS en aplicaciones web
 */
class XssAnalyzer {
public:
    /**
     * Constructor
     */
    XssAnalyzer();
    
    /**
     * Analiza una respuesta HTTP para detectar vulnerabilidades XSS
     * @param response Respuesta HTTP a analizar
     * @param payload Payload XSS utilizado
     * @param originalInput Entrada original que se envió al servidor
     * @return Resultado del análisis
     */
    XssAnalysisResult analyzeResponse(const std::string& response, 
                                     const std::string& payload,
                                     const std::string& originalInput);
    
    /**
     * Detecta vulnerabilidades XSS en un endpoint
     * @param url URL del endpoint a analizar
     * @param params Parámetros a probar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado del análisis
     */
    XssAnalysisResult detectVulnerability(const std::string& url, 
                                         const std::map<std::string, std::string>& params,
                                         const std::map<std::string, std::string>& headers = {});
    
    /**
     * Detecta vulnerabilidades XSS en el DOM
     * @param url URL de la página a analizar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado del análisis
     */
    XssAnalysisResult detectDomBasedXss(const std::string& url,
                                       const std::map<std::string, std::string>& headers = {});
    
    /**
     * Analiza el contexto de ejecución de un payload XSS
     * @param html Contenido HTML donde se ejecuta el payload
     * @param payload Payload XSS
     * @return Contexto de ejecución (HTML_TAG, HTML_ATTRIBUTE, JAVASCRIPT, URL, etc.)
     */
    std::string analyzeExecutionContext(const std::string& html, const std::string& payload);
    
    /**
     * Genera payloads específicos para un contexto de ejecución
     * @param context Contexto de ejecución
     * @return Vector con payloads específicos para el contexto
     */
    std::vector<std::string> generateContextSpecificPayloads(const std::string& context);

private:
    /**
     * Envía una petición HTTP con un payload XSS
     * @param url URL del endpoint
     * @param params Parámetros con el payload XSS
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Respuesta HTTP
     */
    std::string sendXssRequest(const std::string& url, 
                              const std::map<std::string, std::string>& params,
                              const std::map<std::string, std::string>& headers);
    
    /**
     * Verifica si un payload XSS se refleja en la respuesta
     * @param response Respuesta HTTP a analizar
     * @param payload Payload XSS utilizado
     * @return true si el payload se refleja en la respuesta
     */
    bool isPayloadReflected(const std::string& response, const std::string& payload);
    
    /**
     * Verifica si un payload XSS se ejecuta en la respuesta
     * @param response Respuesta HTTP a analizar
     * @param payload Payload XSS utilizado
     * @return true si el payload se ejecuta en la respuesta
     */
    bool isPayloadExecuted(const std::string& response, const std::string& payload);
    
    /**
     * Determina el nivel de impacto de una vulnerabilidad XSS
     * @param vulnerabilityType Tipo de vulnerabilidad XSS
     * @param context Contexto de ejecución
     * @return Nivel de impacto (1-5)
     */
    int determineImpactLevel(const std::string& vulnerabilityType, const std::string& context);
};

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_ANALYZER_XSS_H