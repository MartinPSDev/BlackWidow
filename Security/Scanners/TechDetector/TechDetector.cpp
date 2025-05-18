/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Implementación del detector de tecnologías similar a Wappalyzer
 */

#include "TechDetector.h"

#include <fstream>
#include <sstream>
#include <regex>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <stdexcept>

// Bibliotecas para manejo de JSON
#include <nlohmann/json.hpp>

// Biblioteca para realizar solicitudes HTTP
#include "../../Core/Network/HttpClient.h"

namespace Security::Scanners::TechDetector {

// Alias para simplificar el código
using json = nlohmann::json;

// Ruta a la base de datos de tecnologías
const std::string TECH_DB_PATH = "Resources/TechDetector/technologies.json";
const std::string TECH_ICONS_PATH = "Resources/TechDetector/icons/";

// Constructor
TechDetector::TechDetector() : m_initialized(false) {
    // Inicializar callback con una función vacía
    m_progressCallback = [](int) {};
}

// Destructor
TechDetector::~TechDetector() {
    // Liberar recursos si es necesario
}

// Inicializa el detector cargando la base de datos de tecnologías
bool TechDetector::initialize() {
    if (m_initialized) {
        return true; // Ya está inicializado
    }
    
    try {
        loadTechnologyDatabase();
        m_initialized = true;
        return true;
    } catch (const std::exception& e) {
        // Log del error
        return false;
    }
}

// Carga la base de datos de tecnologías desde archivos JSON
void TechDetector::loadTechnologyDatabase() {
    try {
        // Verificar si el archivo existe
        if (!std::filesystem::exists(TECH_DB_PATH)) {
            throw std::runtime_error("Base de datos de tecnologías no encontrada: " + TECH_DB_PATH);
        }
        
        // Abrir y leer el archivo JSON
        std::ifstream file(TECH_DB_PATH);
        if (!file.is_open()) {
            throw std::runtime_error("No se pudo abrir la base de datos de tecnologías");
        }
        
        json techDb;
        file >> techDb;
        file.close();
        
        // Procesar cada categoría y tecnología
        for (auto& [category, technologies] : techDb.items()) {
            for (auto& [techName, patterns] : technologies.items()) {
                std::vector<std::string> patternList;
                
                // Convertir los patrones a una lista de strings
                for (const auto& pattern : patterns) {
                    patternList.push_back(pattern.get<std::string>());
                }
                
                // Almacenar en la estructura de datos
                m_techPatterns[category][techName] = patternList;
            }
        }
        
        // Notificar progreso
        m_progressCallback(100);
        
    } catch (const std::exception& e) {
        // Reenviar la excepción
        throw std::runtime_error("Error al cargar la base de datos de tecnologías: " + std::string(e.what()));
    }
}

// Analiza una URL para detectar tecnologías
TechDetectionResult TechDetector::analyzeUrl(const std::string& url) {
    if (!m_initialized) {
        initialize();
    }
    
    // Notificar inicio del análisis
    m_progressCallback(0);
    
    // Crear cliente HTTP y realizar la solicitud
    Core::Network::HttpClient httpClient;
    auto response = httpClient.get(url);
    
    // Notificar progreso
    m_progressCallback(50);
    
    // Analizar la respuesta
    return analyzeResponse(url, response.headers, response.body);
}

// Analiza una respuesta HTTP para detectar tecnologías
TechDetectionResult TechDetector::analyzeResponse(
    const std::string& url, 
    const std::map<std::string, std::string>& headers, 
    const std::string& body) {
    
    if (!m_initialized) {
        initialize();
    }
    
    // Crear el resultado
    TechDetectionResult result;
    result.url = url;
    
    // Obtener timestamp actual
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    result.timestamp = ss.str();
    
    // Detectar tecnologías en diferentes partes de la respuesta
    m_progressCallback(60);
    auto headerTechs = detectTechnologiesInHeaders(headers);
    
    m_progressCallback(70);
    auto htmlTechs = detectTechnologiesInHtml(body);
    
    m_progressCallback(80);
    auto scriptTechs = detectTechnologiesInScripts(body);
    
    m_progressCallback(90);
    auto cookieTechs = detectTechnologiesInCookies(headers);
    
    // Combinar los resultados eliminando duplicados
    std::map<std::string, TechnologyInfo> uniqueTechs;
    
    // Función para agregar tecnologías al mapa de resultados únicos
    auto addTechs = [&uniqueTechs](const std::vector<TechnologyInfo>& techs) {
        for (const auto& tech : techs) {
            // Si la tecnología ya existe, actualizar solo si la nueva tiene más información
            if (uniqueTechs.find(tech.name) != uniqueTechs.end()) {
                auto& existingTech = uniqueTechs[tech.name];
                
                // Actualizar versión si la nueva no está vacía y la existente sí
                if (existingTech.version.empty() && !tech.version.empty()) {
                    existingTech.version = tech.version;
                }
                
                // Combinar detalles
                for (const auto& [key, value] : tech.details) {
                    existingTech.details[key] = value;
                }
            } else {
                // Agregar nueva tecnología
                uniqueTechs[tech.name] = tech;
            }
        }
    };
    
    // Agregar todas las tecnologías detectadas
    addTechs(headerTechs);
    addTechs(htmlTechs);
    addTechs(scriptTechs);
    addTechs(cookieTechs);
    
    // Convertir el mapa a vector para el resultado final
    for (const auto& [name, tech] : uniqueTechs) {
        result.technologies.push_back(tech);
    }
    
    // Notificar finalización
    m_progressCallback(100);
    
    return result;
}

// Detecta tecnologías en las cabeceras HTTP
std::vector<TechnologyInfo> TechDetector::detectTechnologiesInHeaders(
    const std::map<std::string, std::string>& headers) {
    
    std::vector<TechnologyInfo> result;
    
    // Iterar por cada cabecera
    for (const auto& [headerName, headerValue] : headers) {
        // Convertir el nombre de la cabecera a minúsculas para comparación
        std::string lowerHeaderName = headerName;
        std::transform(lowerHeaderName.begin(), lowerHeaderName.end(), 
                      lowerHeaderName.begin(), ::tolower);
        
        // Buscar patrones en cada categoría y tecnología
        for (const auto& [category, technologies] : m_techPatterns) {
            for (const auto& [techName, patterns] : technologies) {
                for (const auto& pattern : patterns) {
                    // Verificar si el patrón comienza con "header:"
                    if (pattern.substr(0, 7) == "header:") {
                        std::string headerPattern = pattern.substr(7);
                        std::string::size_type colonPos = headerPattern.find(':');
                        
                        if (colonPos != std::string::npos) {
                            std::string patternHeaderName = headerPattern.substr(0, colonPos);
                            std::string patternHeaderValue = headerPattern.substr(colonPos + 1);
                            
                            // Convertir a minúsculas para comparación
                            std::transform(patternHeaderName.begin(), patternHeaderName.end(), 
                                          patternHeaderName.begin(), ::tolower);
                            
                            // Verificar si coincide el nombre de la cabecera
                            if (lowerHeaderName == patternHeaderName) {
                                // Crear expresión regular para el valor
                                try {
                                    std::regex valueRegex(patternHeaderValue);
                                    if (std::regex_search(headerValue, valueRegex)) {
                                        // Crear información de tecnología detectada
                                        TechnologyInfo tech;
                                        tech.name = techName;
                                        tech.category = category;
                                        tech.website = ""; // Se podría obtener de otra fuente
                                        tech.icon = TECH_ICONS_PATH + techName + ".png";
                                        
                                        // Intentar detectar versión
                                        tech.version = detectVersion(headerValue, techName);
                                        
                                        // Agregar detalles
                                        tech.details["detected_in"] = "HTTP Header: " + headerName;
                                        tech.details["header_value"] = headerValue;
                                        
                                        result.push_back(tech);
                                    }
                                } catch (const std::regex_error&) {
                                    // Ignorar errores de expresión regular
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

// Detecta tecnologías en el contenido HTML
std::vector<TechnologyInfo> TechDetector::detectTechnologiesInHtml(const std::string& html) {
    std::vector<TechnologyInfo> result;
    
    // Buscar patrones en cada categoría y tecnología
    for (const auto& [category, technologies] : m_techPatterns) {
        for (const auto& [techName, patterns] : technologies) {
            for (const auto& pattern : patterns) {
                // Verificar si el patrón es para HTML (no comienza con "header:" o "script:")
                if (pattern.substr(0, 7) != "header:" && pattern.substr(0, 7) != "script:" && 
                    pattern.substr(0, 7) != "cookie:") {
                    try {
                        std::regex htmlRegex(pattern);
                        if (std::regex_search(html, htmlRegex)) {
                            // Crear información de tecnología detectada
                            TechnologyInfo tech;
                            tech.name = techName;
                            tech.category = category;
                            tech.website = ""; // Se podría obtener de otra fuente
                            tech.icon = TECH_ICONS_PATH + techName + ".png";
                            
                            // Intentar detectar versión
                            tech.version = detectVersion(html, techName);
                            
                            // Agregar detalles
                            tech.details["detected_in"] = "HTML Content";
                            tech.details["pattern"] = pattern;
                            
                            result.push_back(tech);
                            
                            // Una vez detectada la tecnología con un patrón, no seguir buscando
                            // con otros patrones para la misma tecnología
                            break;
                        }
                    } catch (const std::regex_error&) {
                        // Ignorar errores de expresión regular
                    }
                }
            }
        }
    }
    
    return result;
}

// Detecta tecnologías en los scripts del HTML
std::vector<TechnologyInfo> TechDetector::detectTechnologiesInScripts(const std::string& html) {
    std::vector<TechnologyInfo> result;
    
    // Extraer todos los scripts del HTML
    std::vector<std::string> scripts;
    std::regex scriptRegex("<script[^>]*>(.*?)</script>", std::regex::icase | std::regex::dotall);
    
    auto scriptBegin = std::sregex_iterator(html.begin(), html.end(), scriptRegex);
    auto scriptEnd = std::sregex_iterator();
    
    for (std::sregex_iterator i = scriptBegin; i != scriptEnd; ++i) {
        std::smatch match = *i;
        if (match.size() > 1) {
            scripts.push_back(match[1].str());
        }
    }
    
    // También extraer scripts externos
    std::regex srcRegex("<script[^>]*src=['\"]([^'\"]+)['\"][^>]*>", std::regex::icase);
    auto srcBegin = std::sregex_iterator(html.begin(), html.end(), srcRegex);
    auto srcEnd = std::sregex_iterator();
    
    for (std::sregex_iterator i = srcBegin; i != srcEnd; ++i) {
        std::smatch match = *i;
        if (match.size() > 1) {
            std::string src = match[1].str();
            // Agregar a la lista de scripts para análisis de patrones
            scripts.push_back("src=" + src);
        }
    }
    
    // Buscar patrones en cada script
    for (const auto& script : scripts) {
        for (const auto& [category, technologies] : m_techPatterns) {
            for (const auto& [techName, patterns] : technologies) {
                for (const auto& pattern : patterns) {
                    // Verificar si el patrón es para scripts
                    if (pattern.substr(0, 7) == "script:") {
                        std::string scriptPattern = pattern.substr(7);
                        try {
                            std::regex scriptRegex(scriptPattern);
                            if (std::regex_search(script, scriptRegex)) {
                                // Crear información de tecnología detectada
                                TechnologyInfo tech;
                                tech.name = techName;
                                tech.category = category;
                                tech.website = ""; // Se podría obtener de otra fuente
                                tech.icon = TECH_ICONS_PATH + techName + ".png";
                                
                                // Intentar detectar versión
                                tech.version = detectVersion(script, techName);
                                
                                // Agregar detalles
                                tech.details["detected_in"] = "JavaScript";
                                tech.details["pattern"] = pattern;
                                
                                result.push_back(tech);
                                
                                // Una vez detectada la tecnología con un patrón, no seguir buscando
                                // con otros patrones para la misma tecnología
                                break;
                            }
                        } catch (const std::regex_error&) {
                            // Ignorar errores de expresión regular
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

// Detecta tecnologías en las cookies
std::vector<TechnologyInfo> TechDetector::detectTechnologiesInCookies(
    const std::map<std::string, std::string>& headers) {
    
    std::vector<TechnologyInfo> result;
    
    // Buscar cabeceras de cookies
    auto it = headers.find("Set-Cookie");
    if (it == headers.end()) {
        // Buscar también con variaciones de mayúsculas/minúsculas
        for (const auto& [headerName, headerValue] : headers) {
            std::string lowerHeaderName = headerName;
            std::transform(lowerHeaderName.begin(), lowerHeaderName.end(), 
                          lowerHeaderName.begin(), ::tolower);
            
            if (lowerHeaderName == "set-cookie") {
                it = headers.find(headerName);
                break;
            }
        }
        
        if (it == headers.end()) {
            return result; // No hay cookies
        }
    }
    
    const std::string& cookieHeader = it->second;
    
    // Buscar patrones en cada categoría y tecnología
    for (const auto& [category, technologies] : m_techPatterns) {
        for (const auto& [techName, patterns] : technologies) {
            for (const auto& pattern : patterns) {
                // Verificar si el patrón es para cookies
                if (pattern.substr(0, 7) == "cookie:") {
                    std::string cookiePattern = pattern.substr(7);
                    try {
                        std::regex cookieRegex(cookiePattern);
                        if (std::regex_search(cookieHeader, cookieRegex)) {
                            // Crear información de tecnología detectada
                            TechnologyInfo tech;
                            tech.name = techName;
                            tech.category = category;
                            tech.website = ""; // Se podría obtener de otra fuente
                            tech.icon = TECH_ICONS_PATH + techName + ".png";
                            
                            // Intentar detectar versión
                            tech.version = detectVersion(cookieHeader, techName);
                            
                            // Agregar detalles
                            tech.details["detected_in"] = "Cookie";
                            tech.details["pattern"] = pattern;
                            
                            result.push_back(tech);
                            
                            // Una vez detectada la tecnología con un patrón, no seguir buscando
                            // con otros patrones para la misma tecnología
                            break;
                        }
                    } catch (const std::regex_error&) {
                        // Ignorar errores de expresión regular
                    }
                }
            }
        }
    }
    
    return result;
}

// Detecta la versión de una tecnología en el contenido
std::string TechDetector::detectVersion(const std::string& content, const std::string& technology) {
    // Patrones comunes para detectar versiones
    std::vector<std::regex> versionPatterns = {
        // Patrón para versiones en formato v1.2.3
        std::regex(technology + "\\s+v?([0-9]+(?:\\.[0-9]+){0,2})", std::regex::icase),
        // Patrón para versiones en comentarios HTML
        std::regex("<!--\\s*" + technology + "\\s+v?([0-9]+(?:\\.[0-9]+){0,2})\\s*-->", std::regex::icase),
        // Patrón para versiones en atributos de metadatos
        std::regex("<meta[^>]*" + technology + "[^>]*v?([0-9]+(?:\\.[0-9]+){0,2})[^>]*>", std::regex::icase),
        // Patrón para versiones en variables JavaScript
        std::regex(technology + "(?:Version|VERSION)\\s*[=:]\\s*['\"]([0-9]+(?:\\.[0-9]+){0,2})['\"]")
    };
    
    // Buscar coincidencias con los patrones
    for (const auto& pattern : versionPatterns) {
        std::smatch matches;
        if (std::regex_search(content, matches, pattern) && matches.size() > 1) {
            return matches[1].str();
        }
    }
    
    return ""; // No se pudo detectar la versión
}

// Registra una función de callback para notificar progreso
void TechDetector::setProgressCallback(std::function<void(int)> callback) {
    m_progressCallback = callback;
}

// Genera un informe del análisis en formato HTML
std::string TechDetector::generateReport(const TechDetectionResult& result) {
    std::stringstream html;
    
    // Encabezado del informe
    html << "<!DOCTYPE html>\n"
         << "<html>\n"
         << "<head>\n"
         << "    <meta charset=\"UTF-8\">\n"
         << "    <title>Informe de Tecnologías - BlackWidow</title>\n"
         << "    <style>\n"
         << "        body { font-family: Arial, sans-serif; margin: 20px; }\n"
         << "        h1 { color: #333; }\n"
         << "        .tech-item { border: 1px solid #ddd; margin: 10px 0; padding: 10px; border-radius: 5px; }\n"
         << "        .tech-header { display: flex; align-items: center; }\n"
         << "        .tech-icon { width: 32px; height: 32px; margin-right: 10px; }\n"
         << "        .tech-name { font-weight: bold; font-size: 18px; }\n"
         << "        .tech-version { color: #666; margin-left: 10px; }\n"
         << "        .tech-category { background: #f0f0f0; padding: 3px 8px; border-radius: 3px; font-size: 12px; }\n"
         << "        .tech-details { margin-top: 10px; }\n"
         << "        .tech-details table { border-collapse: collapse; width: 100%; }\n"
         << "        .tech-details th, .tech-details td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
         << "        .tech-details th { background-color: #f2f2f2; }\n"
         << "    </style>\n"
         << "</head>\n"
         << "<body>\n"
         << "    <h1>Informe de Tecnologías Detectadas</h1>\n"
         << "    <p><strong>URL:</strong> " << result.url << "</p>\n"
         << "    <p><strong>Fecha del análisis:</strong> " << result.timestamp << "</p>\n"
         << "    <p><strong>Tecnologías detectadas:</strong> " << result.technologies.size() << "</p>\n\n";
    
    // Agrupar tecnologías por categoría
    std::map<std::string, std::vector<TechnologyInfo>> techsByCategory;
    for (const auto& tech : result.technologies) {
        techsByCategory[tech.category].push_back(tech);
    }
    
    // Mostrar tecnologías por categoría
    for (const auto& [category, techs] : techsByCategory) {
        html << "    <h2>" << category << " (" << techs.size() << ")</h2>\n";
        
        for (const auto& tech : techs) {
            html << "    <div class=\"tech-item\">\n"
                 << "        <div class=\"tech-header\">\n"
                 << "            <img class=\"tech-icon\" src=\"" << tech.icon << "\" alt=\"" << tech.name << "\" onerror=\"this.src='Resources/TechDetector/icons/default.png';\">\n"
                 << "            <span class=\"tech-name\">" << tech.name << "</span>\n";
            
            if (!tech.version.empty()) {
                html << "            <span class=\"tech-version\">v" << tech.version << "</span>\n";
            }
            
            html << "            <span class=\"tech-category\" style=\"margin-left: auto;\">" << tech.category << "</span>\n"
                 << "        </div>\n";
            
            if (!tech.website.empty() || !tech.details.empty()) {
                html << "        <div class=\"tech-details\">\n";
                
                if (!tech.website.empty()) {
                    html << "            <p><strong>Sitio web:</strong> <a href=\"" << tech.website << "\" target=\"_blank\">" << tech.website << "</a></p>\n";
                }
                
                if (!tech.details.empty()) {
                    html << "            <table>\n"
                         << "                <tr><th>Propiedad</th><th>Valor</th></tr>\n";
                    
                    for (const auto& [key, value] : tech.details) {
                        html << "                <tr><td>" << key << "</td><td>" << value << "</td></tr>\n";
                    }
                    
                    html << "            </table>\n";
                }
                
                html << "        </div>\n";
            }
            
            html << "    </div>\n";
        }
    }
    
    // Pie del informe
    html << "    <p style=\"margin-top: 30px; color: #666; font-size: 12px;\">Generado por BlackWidow Browser - Detector de Tecnologías</p>\n"
         << "</body>\n"
         << "</html>";
    
    return html.str();
}

// Obtiene la lista de tecnologías soportadas
std::map<std::string, std::vector<std::string>> TechDetector::getSupportedTechnologies() const {
    std::map<std::string, std::vector<std::string>> result;
    
    // Convertir la estructura interna a un formato más simple
    for (const auto& [category, technologies] : m_techPatterns) {
        std::vector<std::string> techNames;
        for (const auto& [techName, patterns] : technologies) {
            techNames.push_back(techName);
        }
        result[category] = techNames;
    }
    
    return result;
}

} // namespace Security::Scanners::TechDetector