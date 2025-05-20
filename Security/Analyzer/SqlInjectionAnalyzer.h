/**
 * SqlInjectionAnalyzer.h
 * Definición de la clase para analizar vulnerabilidades de inyección SQL
 */

#ifndef BLACKWIDOW_SECURITY_ANALYZER_SQLINJECTION_H
#define BLACKWIDOW_SECURITY_ANALYZER_SQLINJECTION_H

#include <string>
#include <vector>
#include <map>
#include "../Payloads/SqlInjectionPayloads.h"

namespace BlackWidow {
namespace Security {
namespace Analyzer {

/**
 * Estructura para almacenar los resultados del análisis de inyección SQL
 */
struct SqlInjectionAnalysisResult {
    bool vulnerable;                  // Indica si el objetivo es vulnerable a inyección SQL
    std::string vulnerabilityType;    // Tipo de vulnerabilidad (Error-based, Blind, Union-based, etc.)
    std::string databaseType;         // Tipo de base de datos detectada
    std::string evidence;             // Evidencia de la vulnerabilidad
    std::string payload;              // Payload que detectó la vulnerabilidad
    std::map<std::string, std::string> extractedData; // Datos extraídos mediante la inyección
    int impactLevel;                  // Nivel de impacto (1-5)
    
    SqlInjectionAnalysisResult() : vulnerable(false), impactLevel(0) {}
};

/**
 * Clase para analizar vulnerabilidades de inyección SQL en aplicaciones web
 */
class SqlInjectionAnalyzer {
public:
    /**
     * Constructor
     */
    SqlInjectionAnalyzer();
    
    /**
     * Analiza una respuesta HTTP para detectar vulnerabilidades de inyección SQL
     * @param response Respuesta HTTP a analizar
     * @param payload Payload SQL utilizado
     * @param originalInput Entrada original que se envió al servidor
     * @return Resultado del análisis
     */
    SqlInjectionAnalysisResult analyzeResponse(const std::string& response, 
                                             const std::string& payload,
                                             const std::string& originalInput);
    
    /**
     * Detecta vulnerabilidades de inyección SQL en un endpoint
     * @param url URL del endpoint a analizar
     * @param params Parámetros a probar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado del análisis
     */
    SqlInjectionAnalysisResult detectVulnerability(const std::string& url, 
                                                 const std::map<std::string, std::string>& params,
                                                 const std::map<std::string, std::string>& headers = {});
    
    /**
     * Detecta el tipo de base de datos utilizada
     * @param url URL del endpoint vulnerable
     * @param vulnerableParam Parámetro vulnerable
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Tipo de base de datos detectada
     */
    Payloads::DatabaseType detectDatabaseType(const std::string& url,
                                             const std::string& vulnerableParam,
                                             const std::map<std::string, std::string>& headers = {});
    
    /**
     * Extrae información de la base de datos mediante inyección SQL
     * @param url URL del endpoint vulnerable
     * @param vulnerableParam Parámetro vulnerable
     * @param dbType Tipo de base de datos
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado con la información extraída
     */
    SqlInjectionAnalysisResult extractDatabaseInfo(const std::string& url,
                                                 const std::string& vulnerableParam,
                                                 Payloads::DatabaseType dbType,
                                                 const std::map<std::string, std::string>& headers = {});
    
    /**
     * Realiza pruebas de inyección SQL ciega (blind)
     * @param url URL del endpoint a analizar
     * @param params Parámetros a probar
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Resultado del análisis
     */
    SqlInjectionAnalysisResult detectBlindInjection(const std::string& url, 
                                                  const std::map<std::string, std::string>& params,
                                                  const std::map<std::string, std::string>& headers = {});

private:
    /**
     * Envía una petición HTTP con un payload de inyección SQL
     * @param url URL del endpoint
     * @param params Parámetros con el payload SQL
     * @param headers Cabeceras HTTP a incluir en la petición
     * @return Respuesta HTTP
     */
    std::string sendSqlInjectionRequest(const std::string& url, 
                                      const std::map<std::string, std::string>& params,
                                      const std::map<std::string, std::string>& headers);
    
    /**
     * Detecta errores de base de datos en la respuesta
     * @param response Respuesta HTTP a analizar
     * @return true si se detectan errores de base de datos
     */
    bool detectDatabaseErrors(const std::string& response);
    
    /**
     * Detecta cambios en la respuesta que indican una inyección exitosa
     * @param originalResponse Respuesta original
     * @param injectedResponse Respuesta con inyección
     * @return true si se detectan cambios significativos
     */
    bool detectResponseChanges(const std::string& originalResponse, const std::string& injectedResponse);
    
    /**
     * Extrae datos de la respuesta que podrían haber sido filtrados por la inyección
     * @param response Respuesta HTTP a analizar
     * @return Mapa con los datos extraídos
     */
    std::map<std::string, std::string> extractLeakedData(const std::string& response);
    
    /**
     * Determina el nivel de impacto de una vulnerabilidad de inyección SQL
     * @param vulnerabilityType Tipo de vulnerabilidad
     * @param databaseType Tipo de base de datos
     * @return Nivel de impacto (1-5)
     */
    int determineImpactLevel(const std::string& vulnerabilityType, const std::string& databaseType);
};

} // namespace Analyzer
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_ANALYZER_SQLINJECTION_H