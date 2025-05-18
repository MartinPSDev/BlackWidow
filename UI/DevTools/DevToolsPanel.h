#ifndef BLACKWIDOW_DEVTOOLSPANEL_H
#define BLACKWIDOW_DEVTOOLSPANEL_H

#include "DevConsole.h"
#include "DOMInspector.h"
#include "JSDebugger.h"
#include <string>
#include <memory>

namespace BlackWidow {
namespace UI {

/**
 * @brief Panel principal de herramientas de desarrollo
 * 
 * Integra todas las herramientas de desarrollo en un panel unificado
 * que permite a los desarrolladores inspeccionar y manipular el DOM,
 * depurar JavaScript y utilizar la consola de desarrollo.
 */
class DevToolsPanel {
public:
    DevToolsPanel();
    ~DevToolsPanel();

    /**
     * @brief Inicializa el panel de herramientas de desarrollo
     */
    void initialize();

    /**
     * @brief Muestra u oculta el panel de herramientas
     * @param visible true para mostrar, false para ocultar
     */
    void setVisible(bool visible);

    /**
     * @brief Verifica si el panel está visible
     * @return true si está visible, false en caso contrario
     */
    bool isVisible() const;

    /**
     * @brief Obtiene la consola de desarrollo
     * @return Referencia a la consola de desarrollo
     */
    DevConsole& getConsole();

    /**
     * @brief Obtiene el inspector DOM
     * @return Referencia al inspector DOM
     */
    DOMInspector& getDOMInspector();

    /**
     * @brief Obtiene el depurador JavaScript
     * @return Referencia al depurador JavaScript
     */
    JSDebugger& getJSDebugger();

    /**
     * @brief Cambia a una pestaña específica del panel
     * @param tabName Nombre de la pestaña ("console", "elements", "sources", "network")
     */
    void switchToTab(const std::string& tabName);

private:
    std::unique_ptr<DevConsole> m_console;
    std::unique_ptr<DOMInspector> m_domInspector;
    std::unique_ptr<JSDebugger> m_jsDebugger;
    bool m_isVisible;
    std::string m_activeTab;
};

} // namespace UI
} // namespace BlackWidow

#endif // BLACKWIDOW_DEVTOOLSPANEL_H