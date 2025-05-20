/**
 * XxePayloads.h
 * Definición de la clase para gestionar payloads de XML External Entity (XXE)
 */

#ifndef BLACKWIDOW_SECURITY_PAYLOADS_XXE_H
#define BLACKWIDOW_SECURITY_PAYLOADS_XXE_H

#include <vector>
#include <string>

namespace BlackWidow {
namespace Security {
namespace Payloads {

/**
 * Enumeración de tipos de ataques XXE
 */
enum class XxeAttackType {
    FILE_READ,           // Lectura de archivos locales
    SSRF,                // Server-Side Request Forgery a través de XXE
    DENIAL_OF_SERVICE,   // Ataques de denegación de servicio
    ERROR_BASED,         // Ataques basados en errores
    OUT_OF_BAND          // Ataques fuera de banda (OOB)
};

/**
 * Clase que proporciona payloads para pruebas de vulnerabilidades XXE
 */
class XxePayloads {
public:
    /**
     * Obtiene una lista de payloads XXE básicos
     * @return Vector con payloads básicos
     */
    static std::vector<std::string> getBasicPayloads();
    
    /**
     * Obtiene una lista de payloads XXE avanzados
     * @return Vector con payloads avanzados
     */
    static std::vector<std::string> getAdvancedPayloads();
    
    /**
     * Obtiene payloads específicos para un tipo de ataque XXE
     * @param attackType Tipo de ataque XXE
     * @return Vector con payloads específicos para el tipo de ataque
     */
    static std::vector<std::string> getAttackSpecificPayloads(XxeAttackType attackType);
    
    /**
     * Genera un payload XXE para leer un archivo específico
     * @param filePath Ruta del archivo a leer
     * @return Payload XXE para leer el archivo
     */
    static std::string generateFileReadPayload(const std::string& filePath);
    
    /**
     * Genera un payload XXE para realizar una petición SSRF
     * @param url URL a la que se realizará la petición
     * @return Payload XXE para realizar la petición SSRF
     */
    static std::string generateSsrfPayload(const std::string& url);
    
    /**
     * Genera un payload XXE para exfiltración de datos fuera de banda (OOB)
     * @param url URL del servidor de exfiltración
     * @param data Datos a exfiltrar (opcional)
     * @return Payload XXE para exfiltración OOB
     */
    static std::string generateOobExfiltrationPayload(
        const std::string& url,
        const std::string& data = ""
    );
    
    /**
     * Genera un payload XXE con técnicas de evasión de protecciones
     * @param payload Payload base a modificar para evasión
     * @return Payload XXE con técnicas de evasión
     */
    static std::string applyEvasionTechniques(const std::string& payload);
};

} // namespace Payloads
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_PAYLOADS_XXE_H