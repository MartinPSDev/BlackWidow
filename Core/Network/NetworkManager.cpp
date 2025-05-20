/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Implementación del gestor de conexiones de red
 */

#include "NetworkManager.h"
#include "HttpClient.h"
#include "SocketManager.h"
#include "TrafficAnalyzer.h"
#include <iostream>

namespace Core::Network {

NetworkManager::NetworkManager()
    : m_trafficInterceptionEnabled(false)
    , m_vulnerabilityScanningEnabled(false)
{
    m_proxyConfig.enabled = false;
    m_proxyConfig.port = 0;
    std::cout << "Inicializando gestor de red" << std::endl;
}

NetworkManager::~NetworkManager() {
    std::cout << "Finalizando gestor de red" << std::endl;
}

bool NetworkManager::initialize() {
    std::cout << "Inicializando componentes de red" << std::endl;
    
    if (!initializeHttpClient()) {
        std::cout << "Error al inicializar el cliente HTTP" << std::endl;
        return false;
    }
    
    if (!initializeSocketManager()) {
        std::cout << "Error al inicializar el gestor de sockets" << std::endl;
        return false;
    }
    
    if (!initializeTrafficAnalyzer()) {
        std::cout << "Error al inicializar el analizador de tráfico" << std::endl;
        return false;
    }
    
    std::cout << "Componentes de red inicializados correctamente" << std::endl;
    return true;
}

bool NetworkManager::sendHttpRequest(
    const std::string& url, 
    const std::string& method, 
    const std::vector<std::pair<std::string, std::string>>& headers, 
    const std::vector<uint8_t>& body,
    std::function<void(int, const std::vector<std::pair<std::string, std::string>>&, const std::vector<uint8_t>&)> callback) {
    
    if (!m_httpClient) {
        std::cout << "Cliente HTTP no inicializado" << std::endl;
        return false;
    }
    
    // Aplicar configuración de proxy si está habilitado
    if (m_proxyConfig.enabled) {
        m_httpClient->setProxy(m_proxyConfig.host, m_proxyConfig.port, 
                             m_proxyConfig.username, m_proxyConfig.password);
    }
    
    // Enviar la solicitud HTTP
    return m_httpClient->sendRequest(url, method, headers, body, callback);
}

bool NetworkManager::enableTrafficInterception(bool enable) {
    m_trafficInterceptionEnabled = enable;
    
    if (!m_trafficAnalyzer) {
        std::cout << "Analizador de tráfico no inicializado" << std::endl;
        return false;
    }
    
    return m_trafficAnalyzer->enableInterception(enable);
}

bool NetworkManager::setProxy(const std::string& host, int port, const std::string& username, const std::string& password) {
    m_proxyConfig.host = host;
    m_proxyConfig.port = port;
    m_proxyConfig.username = username;
    m_proxyConfig.password = password;
    m_proxyConfig.enabled = true;
    
    std::cout << "Proxy configurado: " << host << ":" << port << std::endl;
    return true;
}

bool NetworkManager::enableVulnerabilityScanning(bool enable) {
    m_vulnerabilityScanningEnabled = enable;
    
    if (!m_trafficAnalyzer) {
        std::cout << "Analizador de tráfico no inicializado" << std::endl;
        return false;
    }
    
    return m_trafficAnalyzer->enableVulnerabilityScanning(enable);
}

bool NetworkManager::initializeHttpClient() {
    try {
        m_httpClient = std::make_unique<HttpClient>();
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error al inicializar el cliente HTTP: " << e.what() << std::endl;
        return false;
    }
}

bool NetworkManager::initializeSocketManager() {
    try {
        m_socketManager = std::make_unique<SocketManager>();
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error al inicializar el gestor de sockets: " << e.what() << std::endl;
        return false;
    }
}

bool NetworkManager::initializeTrafficAnalyzer() {
    try {
        m_trafficAnalyzer = std::make_unique<TrafficAnalyzer>();
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error al inicializar el analizador de tráfico: " << e.what() << std::endl;
        return false;
    }
}

} // namespace Core::Network