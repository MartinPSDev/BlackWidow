/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Escáner de inyecciones SQL similar a SQLMap
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace Security::Scanners::SQLInjection {

/**
 * Estructura para almacenar información sobre una base de datos detectada
 */
struct DatabaseInfo {
    std::string type;        // MySQL, PostgreSQL, SQLite, Oracle, etc.
    std::string version;     // Versión detectada
    std::string banner;      // Banner completo si está disponible
    std::vector<std::string> tables;  // Tablas detectadas
    std::map<std::string, std::vector<std::string>> columns; // Columnas por tabla
};

/**
 * Estructura para almacenar resultados de una inyección SQL
 */
struct SQLInjectionResult {
    bool vulnerable;          // Si el punto es vulnerable
    std::string payload;     // Payload que funcionó
    std::string parameter;   // Parámetro vulnerable
    std::string url;         // URL donde se encontró la vulnerabilidad
    std::string response;    // Respuesta que confirma la vulnerabilidad
    std::string injectionType; // Tipo de inyección (Union, Error, Blind, etc.)
    DatabaseInfo database;   // Información de la base de datos si se pudo extraer
};

/**
 * Clase que implementa un escáner de inyecciones SQL similar a SQLMap
 * Permite detectar y explotar vulnerabilidades de inyección SQL en aplicaciones web
 */
class SQLInjectionScanner {
public:
    /**
     * Constructor
     */
    SQLInjectionScanner();
    
    /**
     * Destructor
     */
    ~SQLInjectionScanner();

    /**
     * Inicializa el escáner
     * @return true si la inicialización fue exitosa
     */
    bool initialize();

    /**
     * Establece la URL objetivo para el escaneo
     * @param url URL a escanear
     */
    void setTargetUrl(const std::string& url);

    /**
     * Establece los parámetros a probar
     * @param parameters Lista de parámetros
     */
    void setParameters(const std::vector<std::string>& parameters);

    /**
     * Establece si se debe realizar un escaneo automático al detectar formularios
     * @param enable true para activar el escaneo automático
     */
    void enableAutoScan(bool enable);

    /**
     * Establece el nivel de profundidad del escaneo (1-5)
     * @param level Nivel de profundidad
     */
    void setScanLevel(int level);

    /**
     * Establece el riesgo máximo de los payloads a utilizar (1-3)
     * @param risk Nivel de riesgo
     */
    void setRiskLevel(int risk);

    /**
     * Inicia el escaneo de inyecciones SQL
     * @return Resultados del escaneo
     */
    std::vector<SQLInjectionResult> scan();

    /**
     * Escanea una URL específica
     * @param url URL a escanear
     * @return Resultados del escaneo
     */
    std::vector<SQLInjectionResult> scanUrl(const std::string& url);

    /**
     * Escanea una petición HTTP interceptada
     * @param request Petición HTTP
     * @param response Respuesta HTTP
     * @return Resultados del escaneo
     */
    std::vector<SQLInjectionResult> scanRequest(const std::string& request, const std::string& response);

    /**
     * Intenta extraer información de la base de datos
     * @param result Resultado de inyección confirmado
     * @return Información detallada de la base de datos
     */
    DatabaseInfo extractDatabaseInfo(const SQLInjectionResult& result);

    /**
     * Registra una función de callback para notificar progreso
     * @param callback Función a llamar con el progreso (0-100)
     */
    void setProgressCallback(std::function<void(int)> callback);

    /**
     * Genera un informe del escaneo
     * @param results Resultados del escaneo
     * @return Informe en formato HTML
     */
    std::string generateReport(const std::vector<SQLInjectionResult>& results);

private:
    // Estado del escáner
    bool m_initialized;
    bool m_autoScanEnabled;
    int m_scanLevel;
    int m_riskLevel;
    std::string m_targetUrl;
    std::vector<std::string> m_parameters;
    std::function<void(int)> m_progressCallback;

    // Colección de payloads por tipo de inyección y base de datos
    std::map<std::string, std::vector<std::string>> m_payloads;

    // Métodos privados para la detección y explotación
    bool detectDatabaseType(const std::string& url, const std::string& parameter, std::string& dbType);
    bool testInjectionPoint(const std::string& url, const std::string& parameter, const std::string& payload, std::string& response);
    std::vector<std::string> extractTables(const std::string& url, const std::string& parameter, const std::string& dbType);
    std::vector<std::string> extractColumns(const std::string& url, const std::string& parameter, const std::string& dbType, const std::string& table);
    bool isResponseVulnerable(const std::string& response, const std::string& injectionType);
    void loadPayloads();
};

} // namespace Security::Scanners::SQLInjection