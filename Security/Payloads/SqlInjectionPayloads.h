/**
 * SqlInjectionPayloads.h
 * Definición de la clase para gestionar payloads de inyección SQL
 */

#ifndef BLACKWIDOW_SECURITY_PAYLOADS_SQLINJECTION_H
#define BLACKWIDOW_SECURITY_PAYLOADS_SQLINJECTION_H

#include <vector>
#include <string>

namespace BlackWidow {
namespace Security {
namespace Payloads {

/**
 * Enumeración de tipos de bases de datos para payloads específicos
 */
enum class DatabaseType {
    MYSQL,
    MSSQL,
    ORACLE,
    POSTGRESQL,
    SQLITE,
    GENERIC
};

/**
 * Enumeración de contextos donde pueden aplicarse payloads SQLi
 */
enum class SqlContext {
    STRING_LITERAL,   // Dentro de cadenas de texto
    NUMERIC_CONTEXT,  // En contexto numérico
    IDENTIFIER,       // Como identificador (nombre de tabla/columna)
    COMMENT,          // En comentarios
    PROCEDURE_CALL    // En llamadas a procedimientos almacenados
};

/**
 * Clase que proporciona payloads para pruebas de vulnerabilidades de inyección SQL
 */
class SqlInjectionPayloads {
public:
    /**
     * Obtiene una lista de payloads SQLi básicos
     * @return Vector con payloads básicos
     */
    static std::vector<std::string> getBasicPayloads();
    
    /**
     * Obtiene una lista de payloads SQLi avanzados
     * @return Vector con payloads avanzados
     */
    static std::vector<std::string> getAdvancedPayloads();
    
    /**
     * Obtiene payloads específicos para un tipo de base de datos
     * @param dbType Tipo de base de datos objetivo
     * @return Vector con payloads específicos para el tipo de base de datos
     */
    static std::vector<std::string> getDatabaseSpecificPayloads(DatabaseType dbType);
    
    /**
     * Obtiene payloads para extracción de datos
     * @param dbType Tipo de base de datos objetivo
     * @return Vector con payloads para extracción de datos
     */
    static std::vector<std::string> getDataExtractionPayloads(DatabaseType dbType);
    
    /**
     * Obtiene payloads para evasión de filtros WAF
     * @return Vector con payloads para evasión de WAF
     */
    static std::vector<std::string> getWafBypassPayloads();
    
    /**
     * Obtiene payloads específicos para un contexto determinado
     * @param context Contexto específico para los payloads
     * @return Vector con payloads específicos para el contexto
     */
    static std::vector<std::string> getContextSpecificPayloads(SqlContext context);
    
    /**
     * Obtiene payloads para inyecciones ciegas (blind)
     * @param dbType Tipo de base de datos objetivo
     * @return Vector con payloads para inyecciones ciegas
     */
    static std::vector<std::string> getBlindInjectionPayloads(DatabaseType dbType);
};

} // namespace Payloads
} // namespace Security
} // namespace BlackWidow

#endif // BLACKWIDOW_SECURITY_PAYLOADS_SQLINJECTION_H