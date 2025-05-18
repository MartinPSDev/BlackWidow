#ifndef BLACKWIDOW_RENDERINGENGINE_H
#define BLACKWIDOW_RENDERINGENGINE_H

#include <string>
#include <memory>
#include <vector>
#include "../HTML/HTMLParser.h"
#include "../CSS/CSSParser.h"
#include "../DOM/DOMTree.h"

namespace BlackWidow {
namespace Core {

/**
 * @brief Motor de renderizado para páginas web
 * 
 * Esta clase implementa el motor de renderizado principal que procesa
 * documentos HTML/CSS y los convierte en representaciones visuales.
 * Se integra con el componente Browser existente y proporciona una
 * interfaz para el renderizado de sitios web.
 */
class RenderingEngine {
public:
    RenderingEngine();
    ~RenderingEngine();

    /**
     * @brief Inicializa el motor de renderizado
     */
    void initialize();

    /**
     * @brief Renderiza una página web a partir de su contenido HTML
     * @param html Contenido HTML de la página
     * @param baseUrl URL base para resolver referencias relativas
     * @return Identificador único de la página renderizada
     */
    std::string renderPage(const std::string& html, const std::string& baseUrl);

    /**
     * @brief Actualiza el renderizado de una página existente
     * @param pageId Identificador de la página
     */
    void updatePage(const std::string& pageId);

    /**
     * @brief Obtiene una captura del estado actual de la página renderizada
     * @param pageId Identificador de la página
     * @return Representación visual de la página (podría ser un buffer de imagen)
     */
    std::vector<uint8_t> getPageSnapshot(const std::string& pageId);

    /**
     * @brief Conecta el motor de renderizado con las herramientas de desarrollo
     * @param pageId Identificador de la página
     * @param devToolsEnabled true para habilitar las herramientas de desarrollo
     */
    void connectDevTools(const std::string& pageId, bool devToolsEnabled);

    /**
     * @brief Ejecuta JavaScript en el contexto de una página
     * @param pageId Identificador de la página
     * @param script Código JavaScript a ejecutar
     * @return Resultado de la ejecución del script
     */
    std::string executeJavaScript(const std::string& pageId, const std::string& script);

private:
    // Estructuras internas para el manejo de páginas renderizadas
    struct RenderPage;
    std::unordered_map<std::string, std::unique_ptr<RenderPage>> m_pages;

    // Componentes del motor de renderizado
    std::unique_ptr<HTMLParser> m_htmlParser;
    std::unique_ptr<CSSParser> m_cssParser;
    std::unique_ptr<DOMTree> m_domTree;

    // Integración con WebAssembly
    std::unique_ptr<WebAssembly::WasmIntegration> m_wasmIntegration;

    // Métodos privados para el procesamiento interno
    void processHTML(const std::string& html, RenderPage* page);
    void applyCSS(RenderPage* page);
    void layoutElements(RenderPage* page);
    void paintElements(RenderPage* page);
};

} // namespace Core
} // namespace BlackWidow

#endif // BLACKWIDOW_RENDERINGENGINE_H