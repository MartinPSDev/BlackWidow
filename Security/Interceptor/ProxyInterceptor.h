/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Interceptor de tráfico para análisis y manipulación de peticiones HTTP
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace Security::Interceptor {

/**
 * Clase que implementa un interceptor de tráfico HTTP/HTTPS
 * Permite analizar y modificar peticiones y respuestas en tiempo real
 */
class ProxyInterceptor {
public:
    ProxyInterceptor();
    ~ProxyInterceptor();

    /**
     * Inicializa el interceptor de tráfico
     * @return true si la inicialización fue exitosa
     */
    bool initialize();

    /**
     * Activa o desactiva la intercepción de tráfico
     * @param enable true para activar, false para desactivar
     */
    void enableInterception(bool enable);

    /**
     * Verifica si la intercepción está activa
     * @return true si la intercepción está activa
     */
    bool isInterceptionEnabled() const;

    /**
     * Establece reglas de filtrado para las peticiones
     * @param rules Mapa de reglas de filtrado
     */
    void setFilterRules(const std::map<std::string, std::string>& rules);

    /**
     * Añade una regla de modificación de cabeceras HTTP
     * @param header Nombre de la cabecera
     * @param value Valor de la cabecera
     */
    void addHeaderModification(const std::string& header, const std::string& value);

    /**
     * Establece un payload personalizado para inyectar en las peticiones
     * @param payload Payload a inyectar
     */
    void setInjectionPayload(const std::string& payload);

private:
    // Estado del interceptor
    bool m_initialized;
    bool m_interceptionEnabled;

    // Configuración
    std::map<std::string, std::string> m_filterRules;
    std::map<std::string, std::string> m_headerModifications;
    std::string m_injectionPayload;

    // Métodos privados
    bool setupProxy();
    bool installCertificates();
};

} // namespace Security::Interceptor