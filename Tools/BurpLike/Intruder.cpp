/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Implementación de la herramienta Intruder
 */

#include "Intruder.h"
#include "../../Utils/Logging/Logger.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

namespace Tools::BurpLike {

Intruder::Intruder() : 
    m_attackType(AttackType::SNIPER),
    m_attackRunning(false),
    m_requestInterval(500),
    m_threadCount(1) {
    Utils::Logging::Logger::info("Inicializando herramienta Intruder");
}

Intruder::~Intruder() {
    // Asegurarse de detener cualquier ataque en curso
    if (m_attackRunning) {
        stopAttack();
    }
    Utils::Logging::Logger::info("Finalizando herramienta Intruder");
}

void Intruder::setBaseRequest(const HttpRequest& request) {
    m_baseRequest = request;
    Utils::Logging::Logger::info("Intruder: Solicitud base establecida para " + request.url);
}

HttpRequest Intruder::getBaseRequest() const {
    return m_baseRequest;
}

void Intruder::setAttackType(AttackType type) {
    m_attackType = type;
    
    std::string typeStr;
    switch (type) {
        case AttackType::SNIPER:
            typeStr = "Sniper";
            break;
        case AttackType::BATTERING_RAM:
            typeStr = "Battering Ram";
            break;
        case AttackType::PITCHFORK:
            typeStr = "Pitchfork";
            break;
        case AttackType::CLUSTER_BOMB:
            typeStr = "Cluster Bomb";
            break;
    }
    
    Utils::Logging::Logger::info("Intruder: Tipo de ataque establecido a " + typeStr);
}

void Intruder::addInsertionPoint(const InsertionPoint& point) {
    m_insertionPoints.push_back(point);
    Utils::Logging::Logger::info("Intruder: Punto de inserción añadido: " + point.name);
}

void Intruder::clearInsertionPoints() {
    m_insertionPoints.clear();
    Utils::Logging::Logger::info("Intruder: Puntos de inserción eliminados");
}

void Intruder::addPayloadList(int position, const std::vector<std::string>& payloads) {
    if (position >= 0 && position < static_cast<int>(m_insertionPoints.size())) {
        m_payloads[position] = payloads;
        Utils::Logging::Logger::info("Intruder: Lista de payloads añadida para la posición " + std::to_string(position));
    } else {
        Utils::Logging::Logger::error("Intruder: Posición de inserción inválida: " + std::to_string(position));
    }
}

bool Intruder::loadPayloadFile(int position, const std::string& filePath) {
    if (position < 0 || position >= static_cast<int>(m_insertionPoints.size())) {
        Utils::Logging::Logger::error("Intruder: Posición de inserción inválida: " + std::to_string(position));
        return false;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Utils::Logging::Logger::error("Intruder: No se pudo abrir el archivo de payloads: " + filePath);
        return false;
    }
    
    std::vector<std::string> payloads;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            payloads.push_back(line);
        }
    }
    
    file.close();
    
    if (payloads.empty()) {
        Utils::Logging::Logger::warning("Intruder: El archivo de payloads está vacío: " + filePath);
        return false;
    }
    
    m_payloads[position] = payloads;
    Utils::Logging::Logger::info("Intruder: Cargados " + std::to_string(payloads.size()) + 
                               " payloads desde " + filePath);
    return true;
}

bool Intruder::startAttack(std::function<void(const HttpRequest&, const HttpResponse&)> callback) {
    if (m_attackRunning) {
        Utils::Logging::Logger::warning("Intruder: Ya hay un ataque en curso");
        return false;
    }
    
    if (m_insertionPoints.empty()) {
        Utils::Logging::Logger::error("Intruder: No hay puntos de inserción definidos");
        return false;
    }
    
    // Verificar que hay payloads para todos los puntos de inserción necesarios
    for (size_t i = 0; i < m_insertionPoints.size(); i++) {
        if (m_payloads.find(i) == m_payloads.end() || m_payloads[i].empty()) {
            Utils::Logging::Logger::error("Intruder: No hay payloads definidos para el punto de inserción " + 
                                        std::to_string(i));
            return false;
        }
    }
    
    // Generar las solicitudes para el ataque
    std::vector<HttpRequest> requests = generateRequests();
    
    if (requests.empty()) {
        Utils::Logging::Logger::error("Intruder: No se generaron solicitudes para el ataque");
        return false;
    }
    
    Utils::Logging::Logger::info("Intruder: Iniciando ataque con " + std::to_string(requests.size()) + 
                               " solicitudes");
    
    m_attackRunning = true;
    m_results.clear();
    
    // Aquí se implementaría el código para ejecutar el ataque en hilos separados
    // Por ahora, simulamos el envío de solicitudes
    
    // Simulación de envío de solicitudes
    for (const auto& request : requests) {
        if (!m_attackRunning) {
            break; // El ataque fue detenido
        }
        
        // Simular envío de solicitud
        HttpResponse response;
        response.statusCode = 200;
        response.statusText = "OK";
        response.headers = "Content-Type: text/html\r\nServer: BlackWidow/1.0";
        response.body = "<html><body><h1>Respuesta de prueba</h1></body></html>";
        response.timeMs = 150; // Tiempo simulado
        
        // Guardar resultado
        m_results.push_back(std::make_pair(request, response));
        
        // Llamar al callback
        if (callback) {
            callback(request, response);
        }
        
        // Esperar el intervalo configurado
        std::this_thread::sleep_for(std::chrono::milliseconds(m_requestInterval));
    }
    
    m_attackRunning = false;
    Utils::Logging::Logger::info("Intruder: Ataque completado con " + std::to_string(m_results.size()) + 
                               " resultados");
    
    return true;
}

void Intruder::stopAttack() {
    if (m_attackRunning) {
        Utils::Logging::Logger::info("Intruder: Deteniendo ataque en curso");
        m_attackRunning = false;
    }
}

bool Intruder::isAttackRunning() const {
    return m_attackRunning;
}

std::vector<std::pair<HttpRequest, HttpResponse>> Intruder::getResults() const {
    return m_results;
}

void Intruder::setRequestInterval(int interval) {
    m_requestInterval = interval;
    Utils::Logging::Logger::info("Intruder: Intervalo entre solicitudes establecido a " + 
                               std::to_string(interval) + " ms");
}

void Intruder::setThreadCount(int threads) {
    m_threadCount = threads;
    Utils::Logging::Logger::info("Intruder: Número de hilos establecido a " + std::to_string(threads));
}

std::vector<HttpRequest> Intruder::generateRequests() {
    std::vector<HttpRequest> requests;
    
    switch (m_attackType) {
        case AttackType::SNIPER: {
            // En modo Sniper, se ataca un solo punto de inserción a la vez
            for (size_t pointIndex = 0; pointIndex < m_insertionPoints.size(); pointIndex++) {
                const auto& point = m_insertionPoints[pointIndex];
                const auto& payloads = m_payloads[pointIndex];
                
                for (const auto& payload : payloads) {
                    HttpRequest req = applyPayload(m_baseRequest, point, payload);
                    requests.push_back(req);
                }
            }
            break;
        }
        
        case AttackType::BATTERING_RAM: {
            // En modo Battering Ram, se usa el mismo payload en todos los puntos de inserción
            const auto& payloads = m_payloads[0]; // Usamos la primera lista de payloads
            
            for (const auto& payload : payloads) {
                HttpRequest req = m_baseRequest;
                
                // Aplicar el mismo payload a todos los puntos de inserción
                for (const auto& point : m_insertionPoints) {
                    req = applyPayload(req, point, payload);
                }
                
                requests.push_back(req);
            }
            break;
        }
        
        case AttackType::PITCHFORK: {
            // En modo Pitchfork, se usan payloads sincronizados
            // Necesitamos determinar cuántas iteraciones hacer (el mínimo de todas las listas)
            size_t minPayloads = SIZE_MAX;
            for (size_t i = 0; i < m_insertionPoints.size(); i++) {
                minPayloads = std::min(minPayloads, m_payloads[i].size());
            }
            
            for (size_t i = 0; i < minPayloads; i++) {
                HttpRequest req = m_baseRequest;
                
                for (size_t pointIndex = 0; pointIndex < m_insertionPoints.size(); pointIndex++) {
                    req = applyPayload(req, m_insertionPoints[pointIndex], m_payloads[pointIndex][i]);
                }
                
                requests.push_back(req);
            }
            break;
        }
        
        case AttackType::CLUSTER_BOMB: {
            // En modo Cluster Bomb, se prueban todas las combinaciones posibles
            // Implementación recursiva para generar todas las combinaciones
            std::vector<size_t> indices(m_insertionPoints.size(), 0);
            bool done = false;
            
            while (!done) {
                HttpRequest req = m_baseRequest;
                
                // Aplicar los payloads actuales a cada punto de inserción
                for (size_t pointIndex = 0; pointIndex < m_insertionPoints.size(); pointIndex++) {
                    req = applyPayload(req, m_insertionPoints[pointIndex], 
                                      m_payloads[pointIndex][indices[pointIndex]]);
                }
                
                requests.push_back(req);
                
                // Incrementar los índices para la siguiente combinación
                done = true;
                for (int i = static_cast<int>(indices.size()) - 1; i >= 0; i--) {
                    indices[i]++;
                    if (indices[i] < m_payloads[i].size()) {
                        done = false;
                        break;
                    }
                    indices[i] = 0;
                }
            }
            break;
        }
    }
    
    return requests;
}

HttpRequest Intruder::applyPayload(const HttpRequest& request, const InsertionPoint& point, const std::string& payload) {
    HttpRequest modifiedRequest = request;
    
    // Aplicar el payload según el tipo de punto de inserción
    switch (point.type) {
        case InsertionPoint::Type::URL_PATH: {
            // Modificar la ruta de la URL
            size_t pathStart = modifiedRequest.url.find('/', 8); // Buscar después de http://
            if (pathStart != std::string::npos) {
                std::string baseUrl = modifiedRequest.url.substr(0, pathStart);
                std::string path = modifiedRequest.url.substr(pathStart);
                
                // Reemplazar en la ruta
                size_t replaceStart = point.startPosition - pathStart;
                size_t replaceLength = point.endPosition - point.startPosition;
                
                if (replaceStart < path.length()) {
                    path = path.substr(0, replaceStart) + payload + 
                           path.substr(replaceStart + replaceLength);
                    modifiedRequest.url = baseUrl + path;
                }
            }
            break;
        }
        
        case InsertionPoint::Type::URL_PARAMETER: {
            // Modificar un parámetro en la URL
            size_t queryStart = modifiedRequest.url.find('?');
            if (queryStart != std::string::npos) {
                std::string baseUrl = modifiedRequest.url.substr(0, queryStart + 1);
                std::string query = modifiedRequest.url.substr(queryStart + 1);
                
                // Buscar el parámetro
                std::string paramPrefix = point.name + "=";
                size_t paramPos = query.find(paramPrefix);
                
                if (paramPos != std::string::npos) {
                    size_t valueStart = paramPos + paramPrefix.length();
                    size_t valueEnd = query.find('&', valueStart);
                    if (valueEnd == std::string::npos) valueEnd = query.length();
                    
                    // Reemplazar el valor
                    query = query.substr(0, valueStart) + payload + 
                            (valueEnd < query.length() ? query.substr(valueEnd) : "");
                    
                    modifiedRequest.url = baseUrl + query;
                }
            }
            break;
        }
        
        case InsertionPoint::Type::BODY_PARAMETER: {
            // Modificar un parámetro en el cuerpo
            std::string paramPrefix = point.name + "=";
            size_t paramPos = modifiedRequest.body.find(paramPrefix);
            
            if (paramPos != std::string::npos) {
                size_t valueStart = paramPos + paramPrefix.length();
                size_t valueEnd = modifiedRequest.body.find('&', valueStart);
                if (valueEnd == std::string::npos) valueEnd = modifiedRequest.body.length();
                
                // Reemplazar el valor
                modifiedRequest.body = modifiedRequest.body.substr(0, valueStart) + payload + 
                                      (valueEnd < modifiedRequest.body.length() ? 
                                       modifiedRequest.body.substr(valueEnd) : "");
            }
            break;
        }
        
        case InsertionPoint::Type::COOKIE: {
            // Modificar una cookie en las cabeceras
            std::string cookieHeader = "Cookie: ";
            size_t cookiePos = modifiedRequest.headers.find(cookieHeader);
            
            if (cookiePos != std::string::npos) {
                size_t cookieStart = cookiePos + cookieHeader.length();
                size_t cookieEnd = modifiedRequest.headers.find("\r\n", cookieStart);
                if (cookieEnd == std::string::npos) cookieEnd = modifiedRequest.headers.length();
                
                std::string cookies = modifiedRequest.headers.substr(cookieStart, cookieEnd - cookieStart);
                
                // Buscar la cookie específica
                std::string cookiePrefix = point.name + "=";
                size_t cookieValuePos = cookies.find(cookiePrefix);
                
                if (cookieValuePos != std::string::npos) {
                    size_t valueStart = cookieValuePos + cookiePrefix.length();
                    size_t valueEnd = cookies.find(';', valueStart);
                    if (valueEnd == std::string::npos) valueEnd = cookies.length();
                    
                    // Reemplazar el valor
                    cookies = cookies.substr(0, valueStart) + payload + 
                             (valueEnd < cookies.length() ? cookies.substr(valueEnd) : "");
                    
                    // Reconstruir las cabeceras
                    modifiedRequest.headers = modifiedRequest.headers.substr(0, cookieStart) + 
                                             cookies + 
                                             modifiedRequest.headers.substr(cookieEnd);
                }
            }
            break;
        }
        
        case InsertionPoint::Type::HEADER: {
            // Modificar una cabecera específica
            std::string headerPrefix = point.name + ": ";
            size_t headerPos = modifiedRequest.headers.find(headerPrefix);
            
            if (headerPos != std::string::npos) {
                size_t valueStart = headerPos + headerPrefix.length();
                size_t valueEnd = modifiedRequest.headers.find("\r\n", valueStart);
                if (valueEnd == std::string::npos) valueEnd = modifiedRequest.headers.length();
                
                // Reemplazar el valor
                modifiedRequest.headers = modifiedRequest.headers.substr(0, valueStart) + 
                                         payload + 
                                         modifiedRequest.headers.substr(valueEnd);
            }
            break;
        }
        
        case InsertionPoint::Type::CUSTOM: {
            // Punto de inserción personalizado, usar las posiciones directamente
            // Primero determinar si está en la URL, cabeceras o cuerpo
            
            // Comprobar si está en la URL
            if (point.startPosition < modifiedRequest.url.length()) {
                modifiedRequest.url = modifiedRequest.url.substr(0, point.startPosition) + 
                                     payload + 
                                     modifiedRequest.url.substr(point.endPosition);
            }
            // Comprobar si está en las cabeceras
            else if (point.startPosition < modifiedRequest.url.length() + modifiedRequest.headers.length()) {
                size_t headerOffset = point.startPosition - modifiedRequest.url.length();
                size_t headerEndOffset = point.endPosition - modifiedRequest.url.length();
                
                modifiedRequest.headers = modifiedRequest.headers.substr(0, headerOffset) + 
                                         payload + 
                                         modifiedRequest.headers.substr(headerEndOffset);
            }
            // Debe estar en el cuerpo
            else {
                size_t bodyOffset = point.startPosition - 
                                   (modifiedRequest.url.length() + modifiedRequest.headers.length());
                size_t bodyEndOffset = point.endPosition - 
                                     (modifiedRequest.url.length() + modifiedRequest.headers.length());
                
                modifiedRequest.body = modifiedRequest.body.substr(0, bodyOffset) + 
                                     payload + 
                                     modifiedRequest.body.substr(bodyEndOffset);
            }
            break;
        }
    }
    
    return modifiedRequest;
}

} // namespace Tools::BurpLike