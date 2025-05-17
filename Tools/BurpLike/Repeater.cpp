/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Implementación de la herramienta Repeater
 */

#include "Repeater.h"
#include "../../Utils/Logging/Logger.h"

namespace Tools::BurpLike {

Repeater::Repeater() {
    Utils::Logging::Logger::info("Inicializando herramienta Repeater");
}

Repeater::~Repeater() {
    Utils::Logging::Logger::info("Finalizando herramienta Repeater");
}

void Repeater::setRequest(const HttpRequest& request) {
    m_currentRequest = request;
    Utils::Logging::Logger::info("Repeater: Solicitud establecida para " + request.url);
}

HttpRequest Repeater::getRequest() const {
    return m_currentRequest;
}

HttpResponse Repeater::sendRequest() {
    Utils::Logging::Logger::info("Repeater: Enviando solicitud a " + m_currentRequest.url);
    
    // Aquí iría el código para enviar la solicitud HTTP
    // Utilizaría la biblioteca de red del navegador para realizar la petición
    
    // Por ahora, creamos una respuesta simulada
    HttpResponse response;
    response.statusCode = 200;
    response.statusText = "OK";
    response.headers = "Content-Type: text/html\r\nServer: BlackWidow/1.0";
    response.body = "<html><body><h1>Respuesta de prueba</h1></body></html>";
    response.timeMs = 150; // Tiempo simulado
    
    // Guardar en el historial
    m_history.push_back(std::make_pair(m_currentRequest, response));
    
    // Llamar al callback si está definido
    if (m_responseCallback) {
        m_responseCallback(response);
    }
    
    return response;
}

std::vector<std::pair<HttpRequest, HttpResponse>> Repeater::getHistory() const {
    return m_history;
}

void Repeater::setResponseCallback(std::function<void(const HttpResponse&)> callback) {
    m_responseCallback = callback;
}

void Repeater::clearHistory() {
    Utils::Logging::Logger::info("Repeater: Limpiando historial de solicitudes");
    m_history.clear();
}

} // namespace Tools::BurpLike