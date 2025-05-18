#ifndef BLACKWIDOW_HTMLPARSER_H
#define BLACKWIDOW_HTMLPARSER_H

#include <string>
#include <memory>
#include <vector>
#include "../DOM/DOMTree.h"

namespace BlackWidow {
namespace Core {

/**
 * @brief Analizador de documentos HTML
 * 
 * Esta clase implementa un analizador de HTML que procesa documentos
 * y construye un árbol DOM representando su estructura.
 */
class HTMLParser {
public:
    HTMLParser();
    ~HTMLParser();

    /**
     * @brief Inicializa el analizador HTML
     */
    void initialize();

    /**
     * @brief Analiza un documento HTML y construye su árbol DOM
     * @param html Contenido HTML a analizar
     * @param baseUrl URL base para resolver referencias relativas
     * @return Árbol DOM construido a partir del HTML
     */
    std::unique_ptr<DOMTree> parse(const std::string& html, const std::string& baseUrl);

    /**
     * @brief Analiza un fragmento HTML y lo integra en un árbol DOM existente
     * @param html Fragmento HTML a analizar
     * @param domTree Árbol DOM donde se integrará el fragmento
     * @param parentElement Elemento padre donde se insertará el fragmento
     */
    void parseFragment(const std::string& html, DOMTree* domTree, void* parentElement);

    /**
     * @brief Serializa un árbol DOM a HTML
     * @param domTree Árbol DOM a serializar
     * @return Representación HTML del árbol DOM
     */
    std::string serialize(const DOMTree* domTree);

private:
    // Estructuras internas para el análisis
    struct ParserContext;
    std::unique_ptr<ParserContext> m_context;

    // Métodos privados para el procesamiento interno
    void tokenize(const std::string& html);
    void buildTree();
    void processToken(const std::string& token);
    void handleStartTag(const std::string& tag, const std::vector<std::pair<std::string, std::string>>& attributes);
    void handleEndTag(const std::string& tag);
    void handleText(const std::string& text);
    void handleComment(const std::string& comment);
};

} // namespace Core
} // namespace BlackWidow

#endif // BLACKWIDOW_HTMLPARSER_H