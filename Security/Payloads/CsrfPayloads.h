/**
 * CsrfPayloads.h
 * Definición de la clase para gestionar payloads de Cross-Site Request Forgery (CSRF)
 */

#ifndef BLACKWIDOW_SECURITY_PAYLOADS_CSRF_H
#define BLACKWIDOW_SECURITY_PAYLOADS_CSRF_H

#include <vector>
#include <string>
#include <map>

namespace BlackWidow {
namespace Security {
namespace Payloads {

/**
 * Enumeración de métodos HTTP para payloads CSRF
 */
enum class CsrfMethod {
    GET,
    POST,
    PUT,
    DELETE
};

/**
 * Clase que proporciona payloads para pruebas de vulnerabilidades CSRF
 */
class CsrfPayloads {
public:
    /**
     * Obtiene plantillas HTML para explotar vulnerabilidades CSRF
     * @return Vector con plantillas HTML
     */
    static std::vector<std::string> getHtmlTemplates();
    
    /**
     * Obtiene plantillas JavaScript para explotar vulnerabilidades CSRF
     * @return Vector con plantillas JavaScript
     */
    static std::vector<std::string> getJavaScriptTemplates();
    
    /**
     * Genera un payload CSRF para un método HTTP específico
     * @param method Método HTTP (GET, POST, etc.)
     * @param targetUrl URL objetivo del ataque
     * @param params Parámetros a incluir en la petición
     * @return Código HTML que ejecuta el ataque CSRF
     */
    static std::string generateCsrfPayload(
        CsrfMethod method, 
        const std::string& targetUrl, 
        const std::map<std::string, std::string>& params
    );
    
    /**
     * Genera un payload CSRF que utiliza técnicas de auto-envío
     * @param method Método HTTP (GET, POST, etc.)
     * @param targetUrl URL objetivo del ataque
     * @param params Parámetros a incluir en la petición
     * @return Código HTML que ejecuta el ataque CSRF automáticamente
     */
    static std::string generateAutoSubmitPayload(
        CsrfMethod method, 
        const std::string& targetUrl, 
        const std::map<std::string, std::string>& params
    );
    
    /**
     * Genera un payload CSRF que utiliza XMLHttpRequest
     * @param method Método HTTP (GET, POST, etc.)
     * @param targetUrl URL objetivo del ataque
     * @param params Parámetros a incluir en la petición
     * @param withCredentials Si se deben incluir credenciales en la petición
     * @return Código JavaScript que ejecuta el ataque CSRF usando XHR
     */
    static std::string generateXhrPayload(
        CsrfMethod method, 
        const std::string& targetUrl, 
        const std::map<std::string, std::string>& params,
        bool withCredentials = true
    );
    
    /**
     * Genera un payload CSRF que utiliza Fetch API
     * @param method Método HTTP (GET, POST, etc.)
     * @param targetUrl URL objetivo del ataque
     * @param params Parámetros a incluir en la petición
     * @param withCredentials Si se deben incluir credenciales en la petición
     * @return Código JavaScript que ejecuta el ataque CSRF usando Fetch API
     */
    static std::string generateFetchPayload(
        CsrfMethod method, 
        const std::string& targetUrl, 
        const std::map<std::string, std::string>& params,
        bool withCredentials = true
    );
};

} // namespace Payloads
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_PAYLOADS_CSRF_H