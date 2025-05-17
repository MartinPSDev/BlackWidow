/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Implementación del sistema de logging
 */

#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Utils::Logging {

// Inicialización de variables estáticas
LogLevel Logger::s_logLevel = LogLevel::INFO;
std::ofstream Logger::s_logFile;
std::mutex Logger::s_mutex;
bool Logger::s_initialized = false;

Logger::Logger() {
    // Constructor privado para el patrón singleton
}

Logger::~Logger() {
    // Destructor
}

bool Logger::initialize(const std::string& logFilePath) {
    // Protección para múltiples inicializaciones
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (s_initialized) {
        std::cerr << "El sistema de logs ya ha sido inicializado" << std::endl;
        return true;
    }
    
    // Abrir archivo de log
    s_logFile.open(logFilePath, std::ios::out | std::ios::app);
    if (!s_logFile.is_open()) {
        std::cerr << "Error al abrir el archivo de log: " << logFilePath << std::endl;
        return false;
    }
    
    s_initialized = true;
    info("Sistema de logs inicializado");
    return true;
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (!s_initialized) {
        return;
    }
    
    info("Finalizando sistema de logs");
    s_logFile.close();
    s_initialized = false;
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_logLevel = level;
    log(LogLevel::INFO, "Nivel de log establecido a: " + logLevelToString(level));
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    // No registrar si el nivel es menor que el configurado
    if (level < s_logLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(s_mutex);
    
    // Obtener timestamp actual
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    // Formato: [TIMESTAMP] [NIVEL] Mensaje
    std::string logMessage = "[" + ss.str() + "] [" + logLevelToString(level) + "] " + message;
    
    // Escribir en archivo y consola
    if (s_initialized && s_logFile.is_open()) {
        s_logFile << logMessage << std::endl;
        s_logFile.flush();
    }
    
    // También mostrar en consola
    std::cout << logMessage << std::endl;
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

} // namespace Utils::Logging