/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Cliente HTTP/HTTPS para realizar solicitudes web
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace Core::Network {

/**
 * Clase que implementa un cliente HTTP/HTTPS
 * Gestiona las solicitudes y respuestas HTTP/HTTPS
 */
class HttpClient {
public:
    /**
     * Constructor
     */
    HttpClient();
    
    /**
     * Destructor
     */
    ~HttpClient();

    /**
     * Envía una solicitud HTTP/HTTPS
     * @param url URL de la solicitud
     * @param method Método HTTP (GET, POST, etc.)
     * @param headers Cabeceras de la solicitud
     * @param body Cuerpo de la solicitud
     * @param callback Función de callback para la respuesta
     * @return true si la solicitud fue enviada correctamente, false en caso contrario
     */
    bool sendRequest(const std::string& url, 
                    const std::string& method, 
                    const std::vector<std::pair<std::string, std::string>>& headers, 
                    const std::vector<uint8_t>& body,
                    std::function<void(int, const std::vector<std::pair<std::string, std::string>>&, const std::vector<uint8_t>&)> callback);

    /**
     * Configura un proxy para las solicitudes
     * @param host Host del proxy
     * @param port Puerto del proxy
     * @param username Usuario para autenticación (opcional)
     * @param password Contraseña para autenticación (opcional)
     * @return true si el proxy fue configurado correctamente, false en caso contrario
     */
    bool setProxy(const std::string& host, int port, const std::string& username = "", const std::string& password = "");

    /**
     * Establece un timeout para las solicitudes
     * @param timeout_ms Timeout en milisegundos
     */
    void setTimeout(int timeout_ms);

    /**
     * Habilita o deshabilita la verificación de certificados SSL
     * @param verify true para verificar certificados, false para ignorar errores de certificados
     */
    void setVerifySsl(bool verify);

private:
    // Implementación privada del cliente HTTP
    class HttpClientImpl;
    std::unique_ptr<HttpClientImpl> m_impl;

    // Métodos privados para la gestión de solicitudes
    bool parseUrl(const std::string& url, std::string& protocol, std::string& host, std::string& path, int& port);
    std::vector<uint8_t> buildRequestData(const std::string& method, const std::string& path, const std::string& host, 
                                         const std::vector<std::pair<std::string, std::string>>& headers, 
                                         const std::vector<uint8_t>& body);
};

} // namespace Core::Network