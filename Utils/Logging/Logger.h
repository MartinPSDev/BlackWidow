/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Sistema de logging para el navegador
 */

#pragma once

#include <string>
#include <fstream>
#include <mutex>

namespace Utils::Logging {

/**
 * Niveles de log disponibles
 */
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * Clase estática para gestionar los logs del sistema
 * Implementa un patrón singleton thread-safe
 */
class Logger {
public:
    /**
     * Inicializa el sistema de logs
     * @param logFilePath Ruta al archivo de logs (opcional)
     * @return true si la inicialización fue exitosa
     */
    static bool initialize(const std::string& logFilePath = "blackwidow.log");

    /**
     * Finaliza el sistema de logs
     */
    static void shutdown();

    /**
     * Establece el nivel mínimo de log
     * @param level Nivel mínimo de log a mostrar
     */
    static void setLogLevel(LogLevel level);

    /**
     * Registra un mensaje de debug
     * @param message Mensaje a registrar
     */
    static void debug(const std::string& message);

    /**
     * Registra un mensaje informativo
     * @param message Mensaje a registrar
     */
    static void info(const std::string& message);

    /**
     * Registra un mensaje de advertencia
     * @param message Mensaje a registrar
     */
    static void warning(const std::string& message);

    /**
     * Registra un mensaje de error
     * @param message Mensaje a registrar
     */
    static void error(const std::string& message);

    /**
     * Registra un mensaje crítico
     * @param message Mensaje a registrar
     */
    static void critical(const std::string& message);

private:
    // Singleton - constructor privado
    Logger();
    ~Logger();

    // Método interno para registrar mensajes
    static void log(LogLevel level, const std::string& message);

    // Convierte un nivel de log a string
    static std::string logLevelToString(LogLevel level);

    // Variables estáticas
    static LogLevel s_logLevel;
    static std::ofstream s_logFile;
    static std::mutex s_mutex;
    static bool s_initialized;
};

} // namespace Utils::Logging