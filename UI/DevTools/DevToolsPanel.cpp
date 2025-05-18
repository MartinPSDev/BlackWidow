#include "DevToolsPanel.h"

namespace BlackWidow {
namespace UI {

DevToolsPanel::DevToolsPanel() : m_isVisible(false), m_activeTab("console") {
    // Inicialización de componentes
    m_console = std::make_unique<DevConsole>();
    m_domInspector = std::make_unique<DOMInspector>();
    m_jsDebugger = std::make_unique<JSDebugger>();
}

DevToolsPanel::~DevToolsPanel() {
    // Limpieza de recursos
}

void DevToolsPanel::initialize() {
    // Configuración inicial del panel de herramientas
    
    // Configurar callback para mensajes de consola
    m_console->setMessageCallback([this](const std::string& message, const std::string& level) {
        // Aquí se podría actualizar la interfaz de usuario con los nuevos mensajes
    });
    
    // Configurar callback para puntos de interrupción
    m_jsDebugger->setBreakpointCallback([this](const std::string& url, int lineNumber) {
        // Aquí se podría actualizar la interfaz de usuario cuando se alcance un punto de interrupción
        m_console->logMessage("Punto de interrupción alcanzado en " + url + ":" + std::to_string(lineNumber), "debug");
    });
}

void DevToolsPanel::setVisible(bool visible) {
    m_isVisible = visible;
    // Aquí se implementaría la lógica para mostrar u ocultar el panel en la interfaz de usuario
}

bool DevToolsPanel::isVisible() const {
    return m_isVisible;
}

DevConsole& DevToolsPanel::getConsole() {
    return *m_console;
}

DOMInspector& DevToolsPanel::getDOMInspector() {
    return *m_domInspector;
}

JSDebugger& DevToolsPanel::getJSDebugger() {
    return *m_jsDebugger;
}

void DevToolsPanel::switchToTab(const std::string& tabName) {
    if (tabName == "console" || tabName == "elements" || tabName == "sources" || tabName == "network") {
        m_activeTab = tabName;
        // Aquí se implementaría la lógica para cambiar la pestaña activa en la interfaz de usuario
    }
}

} // namespace UI
} // namespace BlackWidow