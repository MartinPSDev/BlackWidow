/**
 * XssPayloads.h
 * Definición de la clase para gestionar payloads de Cross-Site Scripting (XSS)
 */

#ifndef BLACKWIDOW_SECURITY_PAYLOADS_XSS_H
#define BLACKWIDOW_SECURITY_PAYLOADS_XSS_H

#include <vector>
#include <string>

namespace BlackWidow {
namespace Security {
namespace Payloads {

/**
 * Enumeración de contextos donde pueden aplicarse payloads XSS
 */
enum class XssContext {
    HTML_ATTRIBUTE,  // Dentro de atributos HTML
    HTML_TAG,        // Dentro de etiquetas HTML
    JAVASCRIPT,      // Dentro de bloques JavaScript
    URL,             // Dentro de URLs
    CSS              // Dentro de bloques CSS
};

/**
 * Clase que proporciona payloads para pruebas de vulnerabilidades XSS
 */
class XssPayloads {
public:
    /**
     * Obtiene una lista de payloads XSS básicos
     * @return Vector con payloads básicos
     */
    static std::vector<std::string> getBasicPayloads();
    
    /**
     * Obtiene una lista de payloads XSS avanzados
     * @return Vector con payloads avanzados
     */
    static std::vector<std::string> getAdvancedPayloads();
    
    /**
     * Obtiene una lista de payloads XSS políglotas
     * @return Vector con payloads políglotas
     */
    static std::vector<std::string> getPolyglotPayloads();
    
    /**
     * Obtiene payloads específicos para un contexto determinado
     * @param context Contexto específico para los payloads
     * @return Vector con payloads específicos para el contexto
     */
    static std::vector<std::string> getContextSpecificPayloads(XssContext context);
};

} // namespace Payloads
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_PAYLOADS_XSS_H