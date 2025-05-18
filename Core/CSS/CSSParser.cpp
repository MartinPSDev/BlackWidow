#include "CSSParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace BlackWidow {
namespace Core {

// Estructura interna para el contexto del analizador
struct CSSParser::ParserContext {
    std::vector<StyleSheet> styleSheets;
    std::unordered_map<void*, std::unordered_map<std::string, std::string>> elementStyles;
    
    ParserContext() {}
};

CSSParser::CSSParser() : m_context(std::make_unique<ParserContext>()) {
}

CSSParser::~CSSParser() {
    // Limpieza de recursos
}

void CSSParser::initialize() {
    // Configuración inicial del analizador CSS
    
    // Reiniciar el contexto
    m_context = std::make_unique<ParserContext>();
}

CSSParser::StyleSheet CSSParser::parse(const std::string& css, const std::string& baseUrl) {
    StyleSheet styleSheet;
    styleSheet.sourceUrl = baseUrl;
    
    // Parsear las reglas CSS
    parseRules(css, styleSheet);
    
    return styleSheet;
}

void CSSParser::parseAndApply(const std::string& css, DOMTree* domTree) {
    if (!domTree) return;
    
    // Parsear la hoja de estilo
    StyleSheet styleSheet = parse(css);
    
    // Aplicar la hoja de estilo al árbol DOM
    applyStyleSheet(styleSheet, domTree);
}

void CSSParser::applyStyleSheet(const StyleSheet& styleSheet, DOMTree* domTree) {
    if (!domTree) return;
    
    // Obtener el elemento raíz del documento
    void* documentElement = domTree->getDocumentElement();
    if (!documentElement) return;
    
    // Aplicar cada regla a los elementos que coincidan con su selector
    for (const auto& rule : styleSheet.rules) {
        // Buscar elementos que coincidan con el selector
        std::vector<void*> matchingElements;
        
        // Implementación simplificada: solo soportamos selectores de etiqueta básicos
        if (rule.selector.front() != '.' && rule.selector.front() != '#') {
            // Selector de etiqueta
            matchingElements = domTree->getElementsByTagName(rule.selector);
        } else if (rule.selector.front() == '#') {
            // Selector de ID
            void* element = domTree->getElementById(rule.selector.substr(1));
            if (element) {
                matchingElements.push_back(element);
            }
        }
        // Nota: Los selectores de clase y otros más complejos requerirían una implementación más sofisticada
        
        // Aplicar la regla a cada elemento coincidente
        for (void* element : matchingElements) {
            applyRuleToElement(rule, element, domTree);
        }
    }
}

std::unordered_map<std::string, std::string> CSSParser::getComputedStyles(void* element) const {
    if (!element) return {};
    
    auto it = m_context->elementStyles.find(element);
    if (it != m_context->elementStyles.end()) {
        return it->second;
    }
    
    return {};
}

void CSSParser::parseRules(const std::string& css, StyleSheet& styleSheet) {
    // Implementación simplificada del análisis de reglas CSS
    // En una implementación real, se utilizaría un analizador más sofisticado
    
    std::string currentSelector;
    std::string currentDeclarations;
    bool inSelector = true;
    bool inComment = false;
    
    std::istringstream stream(css);
    std::string line;
    std::string buffer;
    
    while (std::getline(stream, line)) {
        // Eliminar espacios en blanco al inicio y al final
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        
        if (line.empty()) continue;
        
        // Procesar la línea
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            if (inComment) {
                if (i > 0 && line[i-1] == '*' && c == '/') {
                    inComment = false;
                }
                continue;
            }
            
            if (i < line.length() - 1 && c == '/' && line[i+1] == '*') {
                inComment = true;
                ++i; // Saltar el siguiente carácter
                continue;
            }
            
            if (inSelector) {
                if (c == '{') {
                    inSelector = false;
                    currentSelector = buffer;
                    buffer.clear();
                } else {
                    buffer += c;
                }
            } else {
                if (c == '}') {
                    inSelector = true;
                    currentDeclarations = buffer;
                    buffer.clear();
                    
                    // Crear una nueva regla
                    CSSRule rule;
                    rule.selector = currentSelector;
                    parseDeclarations(currentDeclarations, rule.declarations);
                    styleSheet.rules.push_back(rule);
                    
                    currentSelector.clear();
                    currentDeclarations.clear();
                } else {
                    buffer += c;
                }
            }
        }
    }
}

void CSSParser::parseDeclarations(const std::string& declarationsStr, std::unordered_map<std::string, std::string>& declarations) {
    // Implementación simplificada del análisis de declaraciones CSS
    
    std::istringstream stream(declarationsStr);
    std::string declaration;
    
    // Dividir por punto y coma
    while (std::getline(stream, declaration, ';')) {
        // Eliminar espacios en blanco al inicio y al final
        declaration.erase(0, declaration.find_first_not_of(" \t\n\r\f\v"));
        declaration.erase(declaration.find_last_not_of(" \t\n\r\f\v") + 1);
        
        if (declaration.empty()) continue;
        
        // Buscar el separador de propiedad y valor
        size_t colonPos = declaration.find(':');
        if (colonPos != std::string::npos) {
            std::string property = declaration.substr(0, colonPos);
            std::string value = declaration.substr(colonPos + 1);
            
            // Eliminar espacios en blanco
            property.erase(0, property.find_first_not_of(" \t\n\r\f\v"));
            property.erase(property.find_last_not_of(" \t\n\r\f\v") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
            value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);
            
            // Almacenar la declaración
            declarations[property] = value;
        }
    }
}

bool CSSParser::selectorMatches(const std::string& selector, void* element, DOMTree* domTree) {
    if (!element || !domTree) return false;
    
    // Implementación simplificada: solo soportamos selectores básicos
    
    if (selector.front() != '.' && selector.front() != '#') {
        // Selector de etiqueta
        return domTree->getTagName(element) == selector;
    } else if (selector.front() == '#') {
        // Selector de ID
        std::string id = domTree->getAttribute(element, "id");
        return id == selector.substr(1);
    } else if (selector.front() == '.') {
        // Selector de clase
        std::string className = domTree->getAttribute(element, "class");
        // Verificar si la clase está presente (podría haber múltiples clases)
        std::istringstream stream(className);
        std::string cls;
        while (stream >> cls) {
            if (cls == selector.substr(1)) {
                return true;
            }
        }
    }
    
    return false;
}

void CSSParser::applyRuleToElement(const CSSRule& rule, void* element, DOMTree* domTree) {
    if (!element || !domTree) return;
    
    // Verificar si el selector coincide con el elemento
    if (!selectorMatches(rule.selector, element, domTree)) return;
    
    // Aplicar las declaraciones al elemento
    auto& elementStyle = m_context->elementStyles[element];
    for (const auto& declaration : rule.declarations) {
        elementStyle[declaration.first] = declaration.second;
    }
}

} // namespace Core
} // namespace BlackWidow