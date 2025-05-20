/**
 * CsrfAnalyzer.h
 * Definición de la clase para analizar vulnerabilidades CSRF (Cross-Site Request Forgery)
 */

#ifndef BLACKWIDOW_SECURITY_ANALYZER_CSRF_H
#define BLACKWIDOW_SECURITY_ANALYZER_CSRF_H

#include <string>
#include <vector>
#include <map>
#include "../Payloads/CsrfPayloads.h"

namespace BlackWidow {
namespace Security {
namespace Analyzer {

/**
 * Estructura para almacenar los resultados del análisis CSRF
 */
struct CsrfAnalysisResult {
    bool vulnerable;                  // Indica si el objetivo es vulnerable a CSRF
    std::string vulnerabilityType;    // Tipo de vulnerabilidad CSRF
    std::string evidence;             // Evidencia de la vulnerabilidad
    std::string payload;              // Payload que detectó la vulnerabilidad
    int impactLevel;                  // Nivel de impacto (1-5)
    std::map<std::string, std::string> affectedFunctions; // Funcionalidades afectadas
    
    CsrfAnalysisResult() : vulnerable(false), impactLevel(0) {}
};

/**
 * Clase para analizar vulnerabilidades CSRF en aplicaciones web
 */
class CsrfAnalyzer {
public:
    /**
     * Constructor
     */
    CsrfAnalyzer();
    
    /**
     * Analiza una respuesta HTTP para detectar vulnerabilidades CSRF
     * @param response Respuesta HTTP a analizar
     * @return Resultado del análisis
     */
    CsrfAnalysisResult analyzeResponse(const std::string& response);
    
    /**
     * Detecta vulnerabilidades CSRF en un formulario
     * @param url URL del formulario a analizar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado del análisis
     */
    CsrfAnalysisResult detectVulnerability(const std::string& url, 
                                         const std::map<std::string, std::string>& headers = {});
    
    /**
     * Genera un payload CSRF para explotar una vulnerabilidad
     * @param targetUrl URL objetivo del ataque
     * @param method Método HTTP a utilizar (GET, POST, etc.)
     * @param params Parámetros a incluir en la petición
     * @param autoSubmit Si es true, el payload se enviará automáticamente
     * @return Payload HTML para explotar la vulnerabilidad
     */
    std::string generateExploitPayload(const std::string& targetUrl, 
                                     Payloads::CsrfMethod method,
                                     const std::map<std::string, std::string>& params,
                                     bool autoSubmit = true);
    
    /**
     * Verifica si un token CSRF es seguro
     * @param token Token CSRF a verificar
     * @return true si el token es seguro
     */
    bool isTokenSecure(const std::string& token);
    
    /**
     * Analiza la seguridad de las cookies utilizadas
     * @param cookies Mapa de cookies a analizar
     * @return Resultado del análisis
     */
    CsrfAnalysisResult analyzeCookieSecurity(const std::map<std::string, std::string>& cookies);

private:
    /**
     * Envía una petición HTTP para analizar un formulario
     * @param url URL del formulario
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Respuesta HTTP
     */
    std::string sendFormRequest(const std::string& url, 
                              const std::map<std::string, std::string>& headers);
    
    /**
     * Extrae los formularios de una respuesta HTML
     * @param response Respuesta HTML
     * @return Vector con los formularios encontrados
     */
    std::vector<std::string> extractForms(const std::string& response);
    
    /**
     * Verifica si un formulario tiene protección CSRF
     * @param formHtml HTML del formulario
     * @return true si el formulario está protegido
     */
    bool hasCSRFProtection(const std::string& formHtml);
    
    /**
     * Extrae los campos de un formulario
     * @param formHtml HTML del formulario
     * @return Mapa con los campos del formulario
     */
    std::map<std::string, std::string> extractFormFields(const std::string& formHtml);
    
    /**
     * Determina el nivel de impacto de una vulnerabilidad CSRF
     * @param formAction URL de acción del formulario
     * @param formMethod Método HTTP del formulario
     * @param formFields Campos del formulario
     * @return Nivel de impacto (1-5)
     */
    int determineImpactLevel(const std::string& formAction, 
                           const std::string& formMethod,
                           const std::map<std::string, std::string>& formFields);
};

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_ANALYZER_CSRF_H