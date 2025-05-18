#include "DevConsole.h"
#include <iostream>

namespace BlackWidow {
namespace UI {

DevConsole::DevConsole() {
    // Inicialización de la consola de desarrollador
}

DevConsole::~DevConsole() {
    // Limpieza de recursos
}

std::string DevConsole::executeJavaScript(const std::string& code) {
    // Aquí se implementará la ejecución de JavaScript en el contexto del navegador
    // Por ahora, devolvemos un mensaje de placeholder
    logMessage("Ejecutando: " + code, "info");
    return "[Resultado de ejecución pendiente de implementación]"; 
}

void DevConsole::logMessage(const std::string& message, const std::string& level) {
    ConsoleMessage msg;
    msg.content = message;
    msg.level = level;
    
    m_messages.push_back(msg);
    
    // Notificar a través del callback si está configurado
    if (m_messageCallback) {
        m_messageCallback(message, level);
    }
}

void DevConsole::clearConsole() {
    m_messages.clear();
    logMessage("Consola limpiada", "system");
}

void DevConsole::setMessageCallback(std::function<void(const std::string&, const std::string&)> callback) {
    m_messageCallback = callback;
}

} // namespace UI
} // namespace BlackWidow