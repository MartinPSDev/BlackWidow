/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Analizador de tráfico de red para detección de vulnerabilidades
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace Core::Network {

/**
 * Clase que analiza el tráfico de red en busca de vulnerabilidades
 * Permite interceptar, modificar y analizar solicitudes y respuestas HTTP/HTTPS
 */
class TrafficAnalyzer {
public:
    /**
     * Constructor
     */
    TrafficAnalyzer();
    
    /**
     * Destructor
     */
    ~TrafficAnalyzer();

    /**
     * Habilita o deshabilita la interceptación de tráfico
     * @param enable true para habilitar, false para deshabilitar
     * @return true si la operación fue exitosa, false en caso contrario
     */
    bool enableInterception(bool enable);

    /**
     * Habilita o deshabilita el análisis de vulnerabilidades
     * @param enable true para habilitar, false para deshabilitar
     * @return true si la operación fue exitosa, false en caso contrario
     */
    bool enableVulnerabilityScanning(bool enable);

    /**
     * Registra un callback para interceptar solicitudes HTTP/HTTPS
     * @param callback Función que recibe y puede modificar una solicitud
     * @return ID del interceptor registrado
     */
    int registerRequestInterceptor(std::function<void(std::string&, std::string&, std::vector<std::pair<std::string, std::string>>&, std::vector<uint8_t>&)> callback);

    /**
     * Registra un callback para interceptar respuestas HTTP/HTTPS
     * @param callback Función que recibe y puede modificar una respuesta
     * @return ID del interceptor registrado
     */
    int registerResponseInterceptor(std::function<void(int&, std::vector<std::pair<std::string, std::string>>&, std::vector<uint8_t>&)> callback);

    /**
     * Elimina un interceptor registrado
     * @param interceptor_id ID del interceptor a eliminar
     * @return true si el interceptor fue eliminado correctamente, false en caso contrario
     */
    bool removeInterceptor(int interceptor_id);

    /**
     * Analiza una solicitud en busca de vulnerabilidades
     * @param url URL de la solicitud
     * @param method Método HTTP
     * @param headers Cabeceras de la solicitud
     * @param body Cuerpo de la solicitud
     * @return Vector de vulnerabilidades encontradas (vacío si no hay vulnerabilidades)
     */
    std::vector<std::string> analyzeRequest(const std::string& url, const std::string& method, 
                                          const std::vector<std::pair<std::string, std::string>>& headers, 
                                          const std::vector<uint8_t>& body);

    /**
     * Analiza una respuesta en busca de vulnerabilidades
     * @param status_code Código de estado HTTP
     * @param headers Cabeceras de la respuesta
     * @param body Cuerpo de la respuesta
     * @return Vector de vulnerabilidades encontradas (vacío si no hay vulnerabilidades)
     */
    std::vector<std::string> analyzeResponse(int status_code, 
                                           const std::vector<std::pair<std::string, std::string>>& headers, 
                                           const std::vector<uint8_t>& body);

private:
    // Estado de la interceptación
    bool m_interceptionEnabled;

    // Estado del análisis de vulnerabilidades
    bool m_vulnerabilityScanningEnabled;

    // Interceptores de solicitudes registrados
    std::unordered_map<int, std::function<void(std::string&, std::string&, std::vector<std::pair<std::string, std::string>>&, std::vector<uint8_t>&)>> m_requestInterceptors;

    // Interceptores de respuestas registrados
    std::unordered_map<int, std::function<void(int&, std::vector<std::pair<std::string, std::string>>&, std::vector<uint8_t>&)>> m_responseInterceptors;

    // Contador para generar IDs de interceptores
    int m_nextInterceptorId;

    // Métodos privados para el análisis de tráfico
    bool detectXssVulnerabilities(const std::string& url, const std::vector<uint8_t>& body, std::vector<std::string>& vulnerabilities);
    bool detectSqlInjectionVulnerabilities(const std::string& url, const std::vector<uint8_t>& body, std::vector<std::string>& vulnerabilities);
    bool detectCsrfVulnerabilities(const std::vector<std::pair<std::string, std::string>>& headers, std::vector<std::string>& vulnerabilities);
    bool detectInsecureHeaders(const std::vector<std::pair<std::string, std::string>>& headers, std::vector<std::string>& vulnerabilities);
};

} // namespace Core::Network