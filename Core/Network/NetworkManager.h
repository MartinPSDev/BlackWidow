/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Gestor de conexiones de red y solicitudes HTTP/HTTPS
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace Core::Network {

// Forward declarations
class HttpClient;
class SocketManager;
class TrafficAnalyzer;

/**
 * Clase que gestiona todas las conexiones de red del navegador
 * Proporciona una interfaz unificada para realizar solicitudes HTTP/HTTPS,
 * gestionar sockets y analizar tráfico de red
 */
class NetworkManager {
public:
    /**
     * Constructor
     */
    NetworkManager();
    
    /**
     * Destructor
     */
    ~NetworkManager();

    /**
     * Inicializa los componentes de red
     * @return true si la inicialización fue exitosa, false en caso contrario
     */
    bool initialize();

    /**
     * Realiza una solicitud HTTP/HTTPS
     * @param url URL de la solicitud
     * @param method Método HTTP (GET, POST, etc.)
     * @param headers Cabeceras de la solicitud
     * @param body Cuerpo de la solicitud
     * @param callback Función de callback para la respuesta
     * @return true si la solicitud fue enviada correctamente, false en caso contrario
     */
    bool sendHttpRequest(const std::string& url, 
                        const std::string& method, 
                        const std::vector<std::pair<std::string, std::string>>& headers, 
                        const std::vector<uint8_t>& body,
                        std::function<void(int, const std::vector<std::pair<std::string, std::string>>&, const std::vector<uint8_t>&)> callback);

    /**
     * Intercepta y modifica tráfico de red
     * @param enable true para activar la interceptación, false para desactivarla
     * @return true si la operación fue exitosa, false en caso contrario
     */
    bool enableTrafficInterception(bool enable);

    /**
     * Establece un proxy para todas las conexiones
     * @param host Host del proxy
     * @param port Puerto del proxy
     * @param username Usuario para autenticación (opcional)
     * @param password Contraseña para autenticación (opcional)
     * @return true si el proxy fue configurado correctamente, false en caso contrario
     */
    bool setProxy(const std::string& host, int port, const std::string& username = "", const std::string& password = "");

    /**
     * Analiza vulnerabilidades en el tráfico de red
     * @param enable true para activar el análisis, false para desactivarlo
     * @return true si la operación fue exitosa, false en caso contrario
     */
    bool enableVulnerabilityScanning(bool enable);

private:
    // Componentes de red
    std::unique_ptr<HttpClient> m_httpClient;
    std::unique_ptr<SocketManager> m_socketManager;
    std::unique_ptr<TrafficAnalyzer> m_trafficAnalyzer;

    // Estado de la interceptación de tráfico
    bool m_trafficInterceptionEnabled;

    // Estado del análisis de vulnerabilidades
    bool m_vulnerabilityScanningEnabled;

    // Configuración del proxy
    struct {
        std::string host;
        int port;
        std::string username;
        std::string password;
        bool enabled;
    } m_proxyConfig;

    // Métodos privados para la gestión de conexiones
    bool initializeHttpClient();
    bool initializeSocketManager();
    bool initializeTrafficAnalyzer();
};

} // namespace Core::Network