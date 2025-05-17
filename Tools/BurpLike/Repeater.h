/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Herramienta Repeater - Similar a Burp Suite Repeater
 * Permite enviar solicitudes HTTP personalizadas y analizar las respuestas
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace Tools::BurpLike {

/**
 * Estructura que representa una solicitud HTTP
 */
struct HttpRequest {
    std::string method;      // GET, POST, PUT, etc.
    std::string url;         // URL completa
    std::string headers;     // Cabeceras HTTP
    std::string body;        // Cuerpo de la solicitud
    
    // Constructor por defecto
    HttpRequest() : method("GET") {}
    
    // Constructor con parámetros
    HttpRequest(const std::string& m, const std::string& u, 
                const std::string& h = "", const std::string& b = "") :
        method(m), url(u), headers(h), body(b) {}
};

/**
 * Estructura que representa una respuesta HTTP
 */
struct HttpResponse {
    int statusCode;          // Código de estado HTTP
    std::string statusText;  // Texto del estado HTTP
    std::string headers;     // Cabeceras de respuesta
    std::string body;        // Cuerpo de la respuesta
    int timeMs;              // Tiempo de respuesta en milisegundos
};

/**
 * Clase que implementa la funcionalidad de Repeater
 * Permite enviar solicitudes HTTP personalizadas y analizar las respuestas
 */
class Repeater {
public:
    /**
     * Constructor
     */
    Repeater();
    
    /**
     * Destructor
     */
    ~Repeater();
    
    /**
     * Establece la solicitud actual
     * @param request Solicitud HTTP a establecer
     */
    void setRequest(const HttpRequest& request);
    
    /**
     * Obtiene la solicitud actual
     * @return Solicitud HTTP actual
     */
    HttpRequest getRequest() const;
    
    /**
     * Envía la solicitud actual y obtiene la respuesta
     * @return Respuesta HTTP recibida
     */
    HttpResponse sendRequest();
    
    /**
     * Obtiene el historial de solicitudes enviadas
     * @return Vector con el historial de solicitudes
     */
    std::vector<std::pair<HttpRequest, HttpResponse>> getHistory() const;
    
    /**
     * Establece una función de callback para cuando se recibe una respuesta
     * @param callback Función a llamar cuando se recibe una respuesta
     */
    void setResponseCallback(std::function<void(const HttpResponse&)> callback);
    
    /**
     * Limpia el historial de solicitudes
     */
    void clearHistory();

private:
    // Solicitud actual
    HttpRequest m_currentRequest;
    
    // Historial de solicitudes y respuestas
    std::vector<std::pair<HttpRequest, HttpResponse>> m_history;
    
    // Callback para cuando se recibe una respuesta
    std::function<void(const HttpResponse&)> m_responseCallback;
};

} // namespace Tools::BurpLike