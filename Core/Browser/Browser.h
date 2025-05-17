/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Clase principal del navegador que coordina todos los componentes
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

// Forward declarations
namespace Security::Interceptor {
    class ProxyInterceptor;
}

namespace Core::Browser {

/**
 * Clase principal del navegador BlackWidow
 * Coordina todos los componentes y gestiona el ciclo de vida del navegador
 */
class Browser {
public:
    Browser();
    ~Browser();

    /**
     * Inicializa todos los componentes del navegador
     * @return true si la inicialización fue exitosa
     */
    bool initialize();

    /**
     * Establece el interceptor de tráfico
     * @param interceptor Puntero al interceptor de tráfico
     */
    void setInterceptor(std::shared_ptr<Security::Interceptor::ProxyInterceptor> interceptor);

    /**
     * Navega a una URL específica
     * @param url URL a la que navegar
     * @return true si la navegación fue exitosa
     */
    bool navigateTo(const std::string& url);

    /**
     * Activa el modo de hacking con características de seguridad avanzadas
     * @param enable true para activar, false para desactivar
     */
    void enableHackingMode(bool enable);

    /**
     * Obtiene el estado actual del modo hacking
     * @return true si el modo hacking está activado
     */
    bool isHackingModeEnabled() const;

private:
    // Estado del navegador
    bool m_initialized;
    bool m_hackingModeEnabled;

    // Componentes principales
    std::shared_ptr<Security::Interceptor::ProxyInterceptor> m_interceptor;

    // Métodos privados de inicialización
    bool initializeNetworkStack();
    bool initializeRenderingEngine();
    bool initializeSecurityComponents();
};

} // namespace Core::Browser