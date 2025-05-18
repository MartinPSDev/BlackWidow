#ifndef BLACKWIDOW_DEVTOOLSWIDGET_H
#define BLACKWIDOW_DEVTOOLSWIDGET_H

#include "../DevTools/DevToolsPanel.h"
#include "../../Core/Rendering/RenderingEngine.h"
#include <string>
#include <memory>

namespace BlackWidow {
namespace UI {

/**
 * @brief Widget para integrar las herramientas de desarrollo en la interfaz principal
 * 
 * Este widget proporciona la interfaz gráfica para las herramientas de desarrollo
 * y se integra con la ventana principal del navegador.
 */
class DevToolsWidget {
public:
    DevToolsWidget();
    ~DevToolsWidget();

    /**
     * @brief Inicializa el widget de herramientas de desarrollo
     */
    void initialize();

    /**
     * @brief Muestra u oculta el widget
     * @param visible true para mostrar, false para ocultar
     */
    void setVisible(bool visible);

    /**
     * @brief Verifica si el widget está visible
     * @return true si está visible, false en caso contrario
     */
    bool isVisible() const;

    /**
     * @brief Obtiene el panel de herramientas de desarrollo
     * @return Referencia al panel de herramientas
     */
    DevToolsPanel& getDevToolsPanel();

    /**
     * @brief Abre las herramientas de desarrollo para una página específica
     * @param pageId Identificador de la página
     */
    void openForPage(const std::string& pageId);

    /**
     * @brief Actualiza el contenido del widget
     */
    void update();

    /**
     * @brief Ejecuta un script JavaScript en la página actual
     * @param script Código JavaScript a ejecutar
     */
    void executeScript(const std::string& script);

    /**
     * @brief Renderiza una nueva página web
     * @param html Contenido HTML de la página
     * @param baseUrl URL base para resolver referencias relativas
     * @return Identificador único de la página renderizada
     */
    std::string renderPage(const std::string& html, const std::string& baseUrl);

private:
    std::unique_ptr<DevToolsPanel> m_devToolsPanel;
    std::unique_ptr<Core::RenderingEngine> m_renderingEngine;
    bool m_isVisible;
    std::string m_currentPageId;
    std::string m_pendingDebugScript;
};

} // namespace UI
} // namespace BlackWidow

#endif // BLACKWIDOW_DEVTOOLSWIDGET_H