/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Sistema de configuración para el navegador
 */

#pragma once

#include <string>
#include <map>
#include <memory>

namespace Utils::Config {

/**
 * Clase para gestionar la configuración del navegador
 * Implementa un patrón singleton thread-safe
 */
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    /**
     * Carga la configuración desde el archivo
     * @param configFilePath Ruta al archivo de configuración (opcional)
     * @return true si la carga fue exitosa
     */
    bool loadConfig(const std::string& configFilePath = "config.json");

    /**
     * Guarda la configuración actual en el archivo
     * @return true si el guardado fue exitoso
     */
    bool saveConfig();

    /**
     * Obtiene un valor de configuración como string
     * @param key Clave de configuración
     * @param defaultValue Valor por defecto si la clave no existe
     * @return Valor de configuración
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "");

    /**
     * Obtiene un valor de configuración como entero
     * @param key Clave de configuración
     * @param defaultValue Valor por defecto si la clave no existe
     * @return Valor de configuración
     */
    int getInt(const std::string& key, int defaultValue = 0);

    /**
     * Obtiene un valor de configuración como booleano
     * @param key Clave de configuración
     * @param defaultValue Valor por defecto si la clave no existe
     * @return Valor de configuración
     */
    bool getBool(const std::string& key, bool defaultValue = false);

    /**
     * Establece un valor de configuración
     * @param key Clave de configuración
     * @param value Valor a establecer
     */
    void setValue(const std::string& key, const std::string& value);

    /**
     * Establece un valor de configuración entero
     * @param key Clave de configuración
     * @param value Valor a establecer
     */
    void setValue(const std::string& key, int value);

    /**
     * Establece un valor de configuración booleano
     * @param key Clave de configuración
     * @param value Valor a establecer
     */
    void setValue(const std::string& key, bool value);

private:
    // Ruta al archivo de configuración
    std::string m_configFilePath;
    
    // Mapa de configuración
    std::map<std::string, std::string> m_configValues;
    
    // Métodos privados
    bool parseConfigFile(const std::string& filePath);
    std::string valueToString(const std::string& value);
};

} // namespace Utils::Config