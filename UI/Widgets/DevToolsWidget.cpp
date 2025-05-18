#include "DevToolsWidget.h"
#include "../../Core/Rendering/RenderingEngine.h"

namespace BlackWidow {
namespace UI {

DevToolsWidget::DevToolsWidget() : m_isVisible(false) {
    // Inicialización del widget
    m_devToolsPanel = std::make_unique<DevToolsPanel>();
    m_renderingEngine = std::make_unique<Core::RenderingEngine>();
}

DevToolsWidget::~DevToolsWidget() {
    // Limpieza de recursos
}

void DevToolsWidget::initialize() {
    // Inicializar el panel de herramientas de desarrollo
    m_devToolsPanel->initialize();
    
    // Inicializar el motor de renderizado
    m_renderingEngine->initialize();
    
    // Configuración adicional del widget
    // Aquí se implementaría la creación de la interfaz de usuario con los colores de Tor
    // utilizando los estilos definidos en TorPurple.css
}

void DevToolsWidget::setVisible(bool visible) {
    m_isVisible = visible;
    m_devToolsPanel->setVisible(visible);
    // Aquí se implementaría la lógica para mostrar u ocultar el widget en la interfaz de usuario
}

bool DevToolsWidget::isVisible() const {
    return m_isVisible;
}

DevToolsPanel& DevToolsWidget::getDevToolsPanel() {
    return *m_devToolsPanel;
}

void DevToolsWidget::openForPage(const std::string& pageId) {
    m_currentPageId = pageId;
    setVisible(true);
    
    // Conectar las herramientas de desarrollo con el motor de renderizado
    m_renderingEngine->connectDevTools(pageId, true);
    
    // Actualizar la interfaz de las herramientas de desarrollo con la información de la página
    m_devToolsPanel->switchToTab("elements");
}

void DevToolsWidget::update() {
    // Actualizar el contenido del widget
    if (m_isVisible && !m_currentPageId.empty()) {
        // Obtener una captura del estado actual de la página
        auto snapshot = m_renderingEngine->getPageSnapshot(m_currentPageId);
        
        // Actualizar la visualización en las herramientas de desarrollo
        // Esto podría incluir actualizar el árbol DOM, los estilos aplicados, etc.
        
        // Ejecutar cualquier script de depuración pendiente
        if (!m_pendingDebugScript.empty()) {
            std::string result = m_renderingEngine->executeJavaScript(m_currentPageId, m_pendingDebugScript);
            m_devToolsPanel->getConsole().logMessage("Resultado: " + result, "info");
            m_pendingDebugScript.clear();
        }
    }
}

// Método para ejecutar JavaScript en la página actual
void DevToolsWidget::executeScript(const std::string& script) {
    if (!m_currentPageId.empty()) {
        std::string result = m_renderingEngine->executeJavaScript(m_currentPageId, script);
        m_devToolsPanel->getConsole().logMessage("Ejecutado: " + script, "command");
        m_devToolsPanel->getConsole().logMessage("Resultado: " + result, "info");
    } else {
        m_devToolsPanel->getConsole().logMessage("Error: No hay página activa", "error");
    }
}

// Método para renderizar una nueva página
std::string DevToolsWidget::renderPage(const std::string& html, const std::string& baseUrl) {
    std::string pageId = m_renderingEngine->renderPage(html, baseUrl);
    return pageId;
}

} // namespace UI
} // namespace BlackWidow