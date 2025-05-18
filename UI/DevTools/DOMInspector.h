#ifndef BLACKWIDOW_DOMINSPECTOR_H
#define BLACKWIDOW_DOMINSPECTOR_H

#include <string>
#include <vector>
#include <functional>
#include <set>

namespace BlackWidow {

namespace Core {
    class RenderingEngine; // Declaración adelantada
}

namespace UI {

/**
 * @brief Inspector del DOM para BlackWidow
 * 
 * Permite inspeccionar, modificar y manipular el DOM de las páginas web
 * cargadas en el navegador.
 */
class DOMInspector {
public:
    DOMInspector();
    ~DOMInspector();

    /**
     * @brief Conecta el inspector DOM al motor de renderizado
     * @param engine Puntero al motor de renderizado
     * @param pageId ID de la página a inspeccionar
     */
    void connectToRenderingEngine(Core::RenderingEngine* engine, const std::string& pageId);

    /**
     * @brief Selecciona un elemento del DOM usando un selector CSS
     * @param selector Selector CSS para encontrar el elemento
     * @return ID del elemento seleccionado o cadena vacía si no se encuentra
     */
    std::string selectElement(const std::string& selector);

    /**
     * @brief Obtiene el HTML de un elemento
     * @param elementId ID del elemento
     * @return Código HTML del elemento
     */
    std::string getElementHTML(const std::string& elementId);

    /**
     * @brief Modifica el HTML de un elemento
     * @param elementId ID del elemento
     * @param html Nuevo HTML para el elemento
     * @return true si se modificó correctamente, false en caso contrario
     */
    bool setElementHTML(const std::string& elementId, const std::string& html);

    /**
     * @brief Obtiene los estilos computados de un elemento
     * @param elementId ID del elemento
     * @return Mapa de propiedades CSS y sus valores en formato JSON
     */
    std::string getComputedStyles(const std::string& elementId);

    /**
     * @brief Modifica un estilo de un elemento
     * @param elementId ID del elemento
     * @param property Propiedad CSS a modificar
     * @param value Nuevo valor para la propiedad
     * @return true si se modificó correctamente, false en caso contrario
     */
    bool setElementStyle(const std::string& elementId, const std::string& property, const std::string& value);

    /**
     * @brief Resalta un elemento en la página
     * @param elementId ID del elemento a resaltar
     */
    void highlightElement(const std::string& elementId);

    /**
     * @brief Elimina el resaltado de todos los elementos
     */
    void clearHighlights();

private:
    // Estructura para almacenar información sobre elementos seleccionados
    struct ElementInfo {
        std::string id;
        std::string tagName;
        std::string selector;
    };

    std::vector<ElementInfo> m_selectedElements;
    std::set<std::string> m_highlightedElements; // Elementos actualmente resaltados
    Core::RenderingEngine* m_renderEngine; // Puntero al motor de renderizado
    std::string m_currentPageId; // ID de la página actual
};

} // namespace UI
} // namespace BlackWidow

#endif // BLACKWIDOW_DOMINSPECTOR_H