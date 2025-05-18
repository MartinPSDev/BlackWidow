#ifndef BLACKWIDOW_CSSPARSER_H
#define BLACKWIDOW_CSSPARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../DOM/DOMTree.h"

namespace BlackWidow {
namespace Core {

/**
 * @brief Analizador de hojas de estilo CSS
 * 
 * Esta clase implementa un analizador de CSS que procesa hojas de estilo
 * y aplica los estilos a los elementos de un árbol DOM.
 */
class CSSParser {
public:
    // Estructura para representar una regla CSS
    struct CSSRule {
        std::string selector;
        std::unordered_map<std::string, std::string> declarations;
    };
    
    // Estructura para representar una hoja de estilo
    struct StyleSheet {
        std::vector<CSSRule> rules;
        std::string sourceUrl;
    };

    CSSParser();
    ~CSSParser();

    /**
     * @brief Inicializa el analizador CSS
     */
    void initialize();

    /**
     * @brief Analiza una hoja de estilo CSS
     * @param css Contenido CSS a analizar
     * @param baseUrl URL base para resolver referencias relativas
     * @return Hoja de estilo analizada
     */
    StyleSheet parse(const std::string& css, const std::string& baseUrl = "");

    /**
     * @brief Analiza y aplica una hoja de estilo a un árbol DOM
     * @param css Contenido CSS a analizar y aplicar
     * @param domTree Árbol DOM al que se aplicarán los estilos
     */
    void parseAndApply(const std::string& css, DOMTree* domTree);

    /**
     * @brief Aplica una hoja de estilo a un árbol DOM
     * @param styleSheet Hoja de estilo a aplicar
     * @param domTree Árbol DOM al que se aplicarán los estilos
     */
    void applyStyleSheet(const StyleSheet& styleSheet, DOMTree* domTree);

    /**
     * @brief Obtiene los estilos computados para un elemento
     * @param element Elemento del que se obtendrán los estilos
     * @return Mapa de propiedades y valores de estilo
     */
    std::unordered_map<std::string, std::string> getComputedStyles(void* element) const;

private:
    // Estructuras internas para el análisis
    struct ParserContext;
    std::unique_ptr<ParserContext> m_context;

    // Métodos privados para el procesamiento interno
    void parseRules(const std::string& css, StyleSheet& styleSheet);
    void parseDeclarations(const std::string& declarationsStr, std::unordered_map<std::string, std::string>& declarations);
    bool selectorMatches(const std::string& selector, void* element, DOMTree* domTree);
    void applyRuleToElement(const CSSRule& rule, void* element, DOMTree* domTree);
};

} // namespace Core
} // namespace BlackWidow

#endif // BLACKWIDOW_CSSPARSER_H