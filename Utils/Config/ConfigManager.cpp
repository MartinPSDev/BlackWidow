/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Implementación del sistema de configuración
 */

#include "ConfigManager.h"
#include "../Logging/Logger.h"
#include <fstream>
#include <sstream>

namespace Utils::Config {

ConfigManager::ConfigManager() : 
    m_configFilePath("config.json") {
}

ConfigManager::~ConfigManager() {
    // Guardar configuración al destruir
    saveConfig();
}

bool ConfigManager::loadConfig(const std::string& configFilePath) {
    m_configFilePath = configFilePath;
    
    Utils::Logging::Logger::info("Cargando configuración desde: " + configFilePath);
    
    if (!parseConfigFile(configFilePath)) {
        Utils::Logging::Logger::warning("No se pudo cargar el archivo de configuración. Se usarán valores por defecto.");
        // Crear configuración por defecto
        setValue("browser.homepage", "about:blank");
        setValue("browser.user_agent", "BlackWidow/1.0");
        setValue("security.hacking_mode", false);
        setValue("security.intercept_traffic", false);
        setValue("security.auto_scan", true);
        setValue("proxy.port", 8080);
        
        // Guardar configuración por defecto
        saveConfig();
        return false;
    }
    
    return true;
}

bool ConfigManager::saveConfig() {
    Utils::Logging::Logger::info("Guardando configuración en: " + m_configFilePath);
    
    try {
        std::ofstream file(m_configFilePath);
        if (!file.is_open()) {
            Utils::Logging::Logger::error("No se pudo abrir el archivo para guardar la configuración");
            return false;
        }
        
        // Formato JSON simple
        file << "{" << std::endl;
        
        bool first = true;
        for (const auto& pair : m_configValues) {
            if (!first) {
                file << "," << std::endl;
            }
            first = false;
            
            file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
        }
        
        file << std::endl << "}" << std::endl;
        file.close();
        
        Utils::Logging::Logger::info("Configuración guardada correctamente");
        return true;
    } 
    catch (const std::exception& e) {
        Utils::Logging::Logger::error("Error al guardar la configuración: " + std::string(e.what()));
        return false;
    }
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) {
    auto it = m_configValues.find(key);
    if (it != m_configValues.end()) {
        return it->second;
    }
    return defaultValue;
}

int ConfigManager::getInt(const std::string& key, int defaultValue) {
    auto it = m_configValues.find(key);
    if (it != m_configValues.end()) {
        try {
            return std::stoi(it->second);
        } 
        catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) {
    auto it = m_configValues.find(key);
    if (it != m_configValues.end()) {
        std::string value = it->second;
        return (value == "true" || value == "1" || value == "yes");
    }
    return defaultValue;
}

void ConfigManager::setValue(const std::string& key, const std::string& value) {
    m_configValues[key] = value;
}

void ConfigManager::setValue(const std::string& key, int value) {
    m_configValues[key] = std::to_string(value);
}

void ConfigManager::setValue(const std::string& key, bool value) {
    m_configValues[key] = value ? "true" : "false";
}

bool ConfigManager::parseConfigFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        // Implementación simple de parser JSON
        // En un proyecto real se usaría una biblioteca como nlohmann/json
        std::string line;
        std::string content;
        
        while (std::getline(file, line)) {
            content += line;
        }
        
        // Parseo muy básico, solo para demostración
        // Eliminar llaves, comillas y espacios
        content = content.substr(content.find('{') + 1);
        content = content.substr(0, content.rfind('}'));
        
        std::istringstream iss(content);
        std::string pair;
        
        while (std::getline(iss, pair, ',')) {
            size_t colonPos = pair.find(':');
            if (colonPos != std::string::npos) {
                std::string key = pair.substr(0, colonPos);
                std::string value = pair.substr(colonPos + 1);
                
                // Limpiar comillas y espacios
                key.erase(0, key.find_first_not_of(" \t\n\r\f\v\""));
                key.erase(key.find_last_not_of(" \t\n\r\f\v\"") + 1);
                
                value.erase(0, value.find_first_not_of(" \t\n\r\f\v\""));
                value.erase(value.find_last_not_of(" \t\n\r\f\v\"") + 1);
                
                m_configValues[key] = value;
            }
        }
        
        Utils::Logging::Logger::info("Archivo de configuración cargado correctamente");
        return true;
    } 
    catch (const std::exception& e) {
        Utils::Logging::Logger::error("Error al parsear el archivo de configuración: " + std::string(e.what()));
        return false;
    }
}

} // namespace Utils::Config