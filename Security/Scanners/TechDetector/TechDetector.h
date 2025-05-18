/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Detector de tecnologías similar a Wappalyzer
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace Security::Scanners::TechDetector {

/**
 * Estructura para almacenar información sobre una tecnología detectada
 */
struct TechnologyInfo {
    std::string name;        // Nombre de la tecnología
    std::string version;     // Versión detectada (si está disponible)
    std::string category;    // Categoría (CMS, Framework, Servidor Web, etc.)
    std::string website;     // Sitio web oficial de la tecnología
    std::string icon;        // Ruta al icono de la tecnología
    std::map<std::string, std::string> details; // Detalles adicionales
};

/**
 * Estructura para almacenar resultados del análisis de tecnologías
 */
struct TechDetectionResult {
    std::string url;                      // URL analizada
    std::vector<TechnologyInfo> technologies; // Tecnologías detectadas
    std::string timestamp;                // Momento del análisis
};

/**
 * Clase que implementa un detector de tecnologías similar a Wappalyzer
 * Permite identificar tecnologías utilizadas en sitios web (CMS, frameworks, librerías, etc.)
 */
class TechDetector {
public:
    /**
     * Constructor
     */
    TechDetector();
    
    /**
     * Destructor
     */
    ~TechDetector();

    /**
     * Inicializa el detector
     * @return true si la inicialización fue exitosa
     */
    bool initialize();

    /**
     * Analiza una URL para detectar tecnologías
     * @param url URL a analizar
     * @return Resultado con las tecnologías detectadas
     */
    TechDetectionResult analyzeUrl(const std::string& url);

    /**
     * Analiza una respuesta HTTP para detectar tecnologías
     * @param url URL de la respuesta
     * @param headers Cabeceras HTTP
     * @param body Cuerpo de la respuesta
     * @return Resultado con las tecnologías detectadas
     */
    TechDetectionResult analyzeResponse(const std::string& url, 
                                       const std::map<std::string, std::string>& headers, 
                                       const std::string& body);

    /**
     * Registra una función de callback para notificar progreso
     * @param callback Función a llamar con el progreso (0-100)
     */
    void setProgressCallback(std::function<void(int)> callback);

    /**
     * Genera un informe del análisis
     * @param result Resultado del análisis
     * @return Informe en formato HTML
     */
    std::string generateReport(const TechDetectionResult& result);

    /**
     * Obtiene la lista de tecnologías soportadas
     * @return Mapa de categorías y tecnologías
     */
    std::map<std::string, std::vector<std::string>> getSupportedTechnologies() const;

private:
    // Estado del detector
    bool m_initialized;
    std::function<void(int)> m_progressCallback;

    // Base de datos de tecnologías
    std::map<std::string, std::map<std::string, std::vector<std::string>>> m_techPatterns;
    // Estructura: {categoría: {tecnología: [patrones]}}

    // Métodos privados para la detección
    void loadTechnologyDatabase();
    std::vector<TechnologyInfo> detectTechnologiesInHeaders(const std::map<std::string, std::string>& headers);
    std::vector<TechnologyInfo> detectTechnologiesInHtml(const std::string& html);
    std::vector<TechnologyInfo> detectTechnologiesInScripts(const std::string& html);
    std::vector<TechnologyInfo> detectTechnologiesInCookies(const std::map<std::string, std::string>& headers);
    std::string detectVersion(const std::string& content, const std::string& technology);
};

} // namespace Security::Scanners::TechDetector