/**
 * XxeAnalyzer.h
 * Definición de la clase para analizar vulnerabilidades XXE (XML External Entity)
 */

#ifndef BLACKWIDOW_SECURITY_ANALYZER_XXE_H
#define BLACKWIDOW_SECURITY_ANALYZER_XXE_H

#include <string>
#include <vector>
#include <map>
#include "../Payloads/XxePayloads.h"

namespace BlackWidow {
namespace Security {
namespace Analyzer {

/**
 * Estructura para almacenar los resultados del análisis XXE
 */
struct XxeAnalysisResult {
    bool vulnerable;                  // Indica si el objetivo es vulnerable a XXE
    std::string vulnerabilityType;    // Tipo de vulnerabilidad XXE encontrada
    std::string evidence;             // Evidencia de la vulnerabilidad
    std::string payload;              // Payload que detectó la vulnerabilidad
    std::map<std::string, std::string> extractedData; // Datos extraídos mediante XXE
    
    XxeAnalysisResult() : vulnerable(false) {}
};

/**
 * Clase para analizar vulnerabilidades XXE en aplicaciones web
 */
class XxeAnalyzer {
public:
    /**
     * Constructor
     */
    XxeAnalyzer();
    
    /**
     * Analiza una respuesta HTTP para detectar vulnerabilidades XXE
     * @param response Respuesta HTTP a analizar
     * @param payload Payload XXE utilizado
     * @return Resultado del análisis
     */
    XxeAnalysisResult analyzeResponse(const std::string& response, const std::string& payload);
    
    /**
     * Detecta vulnerabilidades XXE en un endpoint que procesa XML
     * @param url URL del endpoint a analizar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado del análisis
     */
    XxeAnalysisResult detectVulnerability(const std::string& url, const std::map<std::string, std::string>& headers = {});
    
    /**
     * Intenta explotar una vulnerabilidad XXE para leer archivos del sistema
     * @param url URL del endpoint vulnerable
     * @param filePath Ruta del archivo a leer
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado de la explotación
     */
    XxeAnalysisResult exploitFileRead(const std::string& url, const std::string& filePath, 
                                     const std::map<std::string, std::string>& headers = {});
    
    /**
     * Intenta explotar una vulnerabilidad XXE para realizar SSRF
     * @param url URL del endpoint vulnerable
     * @param internalUrl URL interna a la que se intentará acceder
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado de la explotación
     */
    XxeAnalysisResult exploitSsrf(const std::string& url, const std::string& internalUrl,
                                 const std::map<std::string, std::string>& headers = {});
    
    /**
     * Configura el servidor de exfiltración para ataques OOB
     * @param serverUrl URL del servidor de exfiltración
     */
    void setExfiltrationServer(const std::string& serverUrl);
    
    /**
     * Intenta explotar una vulnerabilidad XXE para exfiltrar datos fuera de banda
     * @param url URL del endpoint vulnerable
     * @param filePath Ruta del archivo a exfiltrar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado de la explotación
     */
    XxeAnalysisResult exploitOobExfiltration(const std::string& url, const std::string& filePath,
                                            const std::map<std::string, std::string>& headers = {});

private:
    Payloads::XxePayloads payloadGenerator;  // Generador de payloads XXE
    std::string exfiltrationServer;          // URL del servidor de exfiltración
    
    /**
     * Envía una petición HTTP con un payload XXE
     * @param url URL del endpoint
     * @param payload Payload XXE a enviar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Respuesta HTTP
     */
    std::string sendXxeRequest(const std::string& url, const std::string& payload,
                              const std::map<std::string, std::string>& headers);
    
    /**
     * Analiza la respuesta para detectar si contiene información filtrada por XXE
     * @param response Respuesta HTTP a analizar
     * @return true si se detecta información filtrada
     */
    bool detectDataLeakage(const std::string& response);
    
    /**
     * Extrae datos de la respuesta que podrían haber sido filtrados por XXE
     * @param response Respuesta HTTP a analizar
     * @return Mapa con los datos extraídos
     */
    std::map<std::string, std::string> extractLeakedData(const std::string& response);
};

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_ANALYZER_XXE_H