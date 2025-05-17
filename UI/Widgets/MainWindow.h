/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Ventana principal de la aplicación
 */

#pragma once

#include <memory>
#include <string>

// Forward declarations
namespace Core::Browser {
    class Browser;
}

namespace Tools::BurpLike {
    class Repeater;
    class Intruder;
    struct HttpRequest;
}

namespace UI::Widgets {

// Forward declaration
class BurpLikeTools;

/**
 * Clase que implementa la ventana principal del navegador
 * Gestiona la interfaz de usuario y los controles principales
 */
class MainWindow {
public:
    /**
     * Constructor
     * @param browser Puntero al navegador principal
     */
    explicit MainWindow(std::shared_ptr<Core::Browser::Browser> browser);
    
    /**
     * Destructor
     */
    ~MainWindow();

    /**
     * Muestra la ventana principal
     */
    void show();

    /**
     * Ejecuta el bucle principal de la aplicación
     * @return Código de salida de la aplicación
     */
    int run();

    /**
     * Navega a una URL
     * @param url URL a la que navegar
     */
    void navigateTo(const std::string& url);

    /**
     * Activa o desactiva el modo hacking
     * @param enable true para activar, false para desactivar
     */
    void enableHackingMode(bool enable);

    /**
     * Abre la consola de hacking
     */
    void openHackConsole();
    
    /**
     * Abre la herramienta Repeater
     */
    void openRepeater();
    
    /**
     * Abre la herramienta Intruder
     */
    void openIntruder();
    
    /**
     * Envía una solicitud HTTP capturada a Repeater
     * @param request Solicitud HTTP capturada
     */
    void sendToRepeater(const Tools::BurpLike::HttpRequest& request);
    
    /**
     * Envía una solicitud HTTP capturada a Intruder
     * @param request Solicitud HTTP capturada
     */
    void sendToIntruder(const Tools::BurpLike::HttpRequest& request);

private:
    // Puntero al navegador principal
    std::shared_ptr<Core::Browser::Browser> m_browser;
    
    // Herramientas BurpLike (Repeater, Intruder, etc.)
    std::shared_ptr<BurpLikeTools> m_burpLikeTools;

    // Estado de la ventana
    bool m_isVisible;

    // Métodos privados para la inicialización de componentes
    void initializeUI();
    void setupMenus();
    void setupToolbars();
    void setupStatusBar();
    void setupShortcuts();
    void connectSignals();
};

} // namespace UI::Widgets