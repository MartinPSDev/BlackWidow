/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Interfaz de usuario para las herramientas tipo Burp Suite
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "../../Tools/BurpLike/Repeater.h"
#include "../../Tools/BurpLike/Intruder.h"

namespace UI::Widgets {

/**
 * Clase que implementa la interfaz de usuario para las herramientas tipo Burp Suite
 * Proporciona acceso a Repeater e Intruder desde la interfaz principal
 */
class BurpLikeTools {
public:
    /**
     * Constructor
     */
    BurpLikeTools();
    
    /**
     * Destructor
     */
    ~BurpLikeTools();
    
    /**
     * Inicializa la interfaz de usuario de las herramientas
     */
    void initialize();
    
    /**
     * Muestra la interfaz de Repeater
     */
    void showRepeater();
    
    /**
     * Muestra la interfaz de Intruder
     */
    void showIntruder();
    
    /**
     * Establece una solicitud HTTP en Repeater
     * @param request Solicitud HTTP a establecer
     */
    void setRepeaterRequest(const Tools::BurpLike::HttpRequest& request);
    
    /**
     * Establece una solicitud HTTP en Intruder
     * @param request Solicitud HTTP a establecer
     */
    void setIntruderRequest(const Tools::BurpLike::HttpRequest& request);
    
    /**
     * Obtiene una referencia al objeto Repeater
     * @return Referencia al objeto Repeater
     */
    std::shared_ptr<Tools::BurpLike::Repeater> getRepeater() const;
    
    /**
     * Obtiene una referencia al objeto Intruder
     * @return Referencia al objeto Intruder
     */
    std::shared_ptr<Tools::BurpLike::Intruder> getIntruder() const;
    
    /**
     * Establece una función de callback para cuando se captura una solicitud HTTP
     * @param callback Función a llamar cuando se captura una solicitud
     */
    void setRequestCaptureCallback(std::function<void(const Tools::BurpLike::HttpRequest&)> callback);
    
    /**
     * Activa o desactiva la captura de solicitudes HTTP
     * @param enable true para activar, false para desactivar
     */
    void enableRequestCapture(bool enable);
    
    /**
     * Comprueba si la captura de solicitudes está activada
     * @return true si está activada, false en caso contrario
     */
    bool isRequestCaptureEnabled() const;

private:
    // Instancias de las herramientas
    std::shared_ptr<Tools::BurpLike::Repeater> m_repeater;
    std::shared_ptr<Tools::BurpLike::Intruder> m_intruder;
    
    // Estado de la captura de solicitudes
    bool m_requestCaptureEnabled;
    
    // Callback para la captura de solicitudes
    std::function<void(const Tools::BurpLike::HttpRequest&)> m_requestCaptureCallback;
    
    // Métodos privados para la inicialización de componentes
    void setupRepeaterUI();
    void setupIntruderUI();
};

} // namespace UI::Widgets