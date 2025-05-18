/**
 * ThemeConfig.h
 * Configuración de temas para BlackWidow Browser
 */

#ifndef BLACKWIDOW_THEME_CONFIG_H
#define BLACKWIDOW_THEME_CONFIG_H

#include <string>

namespace BlackWidow {
namespace UI {
namespace Themes {

// Nombres de temas disponibles
const std::string THEME_DEFAULT = "Default";
const std::string THEME_TOR_PURPLE = "TorPurple";
const std::string THEME_DARK = "Dark";
const std::string THEME_LIGHT = "Light";

// Estructura para la configuración del tema
struct ThemeConfig {
    std::string currentTheme = THEME_TOR_PURPLE; // Tema predeterminado: TorPurple
    bool useSystemTheme = false;
    
    // Rutas a los archivos CSS de los temas
    std::string getThemePath() const {
        if (currentTheme == THEME_TOR_PURPLE) {
            return "UI/Themes/TorPurple.css";
        } else if (currentTheme == THEME_DARK) {
            return "UI/Themes/Dark.css";
        } else if (currentTheme == THEME_LIGHT) {
            return "UI/Themes/Light.css";
        } else {
            // Tema por defecto si no se encuentra el seleccionado
            return "UI/Themes/TorPurple.css";
        }
    }
};

// Instancia global de la configuración del tema
extern ThemeConfig g_themeConfig;

} // namespace Themes
} // namespace UI
} // namespace BlackWidow

#endif // BLACKWIDOW_THEME_CONFIG_H