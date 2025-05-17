/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Implementación de la interfaz de usuario para las herramientas tipo Burp Suite
 */

#include "BurpLikeTools.h"
#include "../../Utils/Logging/Logger.h"

namespace UI::Widgets {

BurpLikeTools::BurpLikeTools() : 
    m_requestCaptureEnabled(false) {
    Utils::Logging::Logger::info("Inicializando interfaz de herramientas BurpLike");
    
    // Crear instancias de las herramientas
    m_repeater = std::make_shared<Tools::BurpLike::Repeater>();
    m_intruder = std::make_shared<Tools::BurpLike::Intruder>();
}

BurpLikeTools::~BurpLikeTools() {
    Utils::Logging::Logger::info("Finalizando interfaz de herramientas BurpLike");
}

void BurpLikeTools::initialize() {
    Utils::Logging::Logger::info("Configurando interfaz de herramientas BurpLike");
    
    setupRepeaterUI();
    setupIntruderUI();
}

void BurpLikeTools::showRepeater() {
    Utils::Logging::Logger::info("Mostrando interfaz de Repeater");
    
    // Aquí iría el código para mostrar la interfaz de Repeater
    // Depende de la biblioteca gráfica que se utilice
}

void BurpLikeTools::showIntruder() {
    Utils::Logging::Logger::info("Mostrando interfaz de Intruder");
    
    // Aquí iría el código para mostrar la interfaz de Intruder
    // Depende de la biblioteca gráfica que se utilice
}

void BurpLikeTools::setRepeaterRequest(const Tools::BurpLike::HttpRequest& request) {
    Utils::Logging::Logger::info("Estableciendo solicitud en Repeater: " + request.url);
    m_repeater->setRequest(request);
}

void BurpLikeTools::setIntruderRequest(const Tools::BurpLike::HttpRequest& request) {
    Utils::Logging::Logger::info("Estableciendo solicitud en Intruder: " + request.url);
    m_intruder->setBaseRequest(request);
}

std::shared_ptr<Tools::BurpLike::Repeater> BurpLikeTools::getRepeater() const {
    return m_repeater;
}

std::shared_ptr<Tools::BurpLike::Intruder> BurpLikeTools::getIntruder() const {
    return m_intruder;
}

void BurpLikeTools::setRequestCaptureCallback(std::function<void(const Tools::BurpLike::HttpRequest&)> callback) {
    m_requestCaptureCallback = callback;
    Utils::Logging::Logger::info("Callback de captura de solicitudes establecido");
}

void BurpLikeTools::enableRequestCapture(bool enable) {
    m_requestCaptureEnabled = enable;
    Utils::Logging::Logger::info(std::string("Captura de solicitudes ") + 
                               (enable ? "activada" : "desactivada"));
}

bool BurpLikeTools::isRequestCaptureEnabled() const {
    return m_requestCaptureEnabled;
}

// Métodos privados

void BurpLikeTools::setupRepeaterUI() {
    Utils::Logging::Logger::info("Configurando interfaz de Repeater");
    
    // Aquí iría el código para configurar la interfaz de Repeater
    // Depende de la biblioteca gráfica que se utilice
}

void BurpLikeTools::setupIntruderUI() {
    Utils::Logging::Logger::info("Configurando interfaz de Intruder");
    
    // Aquí iría el código para configurar la interfaz de Intruder
    // Depende de la biblioteca gráfica que se utilice
}

} // namespace UI::Widgets