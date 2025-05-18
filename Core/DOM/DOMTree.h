#ifndef BLACKWIDOW_DOMTREE_H
#define BLACKWIDOW_DOMTREE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace BlackWidow {
namespace Core {

/**
 * @brief Árbol de Modelo de Objetos del Documento (DOM)
 * 
 * Esta clase implementa un árbol DOM que representa la estructura
 * de un documento HTML, permitiendo su manipulación y consulta.
 */
class DOMTree {
public:
    // Tipos de nodos DOM
    enum class NodeType {
        ELEMENT_NODE = 1,
        TEXT_NODE = 3,
        COMMENT_NODE = 8,
        DOCUMENT_NODE = 9
    };
    
    // Estructura para representar un nodo DOM
    struct Node {
        NodeType type;
        std::string tagName;  // Para elementos
        std::string textContent;  // Para texto y comentarios
        std::unordered_map<std::string, std::string> attributes;
        std::vector<std::unique_ptr<Node>> children;
        Node* parent;
        
        Node(NodeType nodeType) : type(nodeType), parent(nullptr) {}
    };

    DOMTree();
    ~DOMTree();

    /**
     * @brief Crea el elemento raíz del documento
     * @return Puntero al elemento documento
     */
    void* createDocumentElement();

    /**
     * @brief Crea un elemento con la etiqueta especificada
     * @param tagName Nombre de la etiqueta
     * @return Puntero al elemento creado
     */
    void* createElement(const std::string& tagName);

    /**
     * @brief Crea un nodo de texto
     * @param text Contenido del texto
     * @return Puntero al nodo de texto creado
     */
    void* createTextNode(const std::string& text);

    /**
     * @brief Crea un nodo de comentario
     * @param comment Contenido del comentario
     * @return Puntero al nodo de comentario creado
     */
    void* createCommentNode(const std::string& comment);

    /**
     * @brief Agrega un nodo hijo a un elemento padre
     * @param parent Elemento padre
     * @param child Nodo hijo a agregar
     */
    void appendChild(void* parent, void* child);

    /**
     * @brief Establece un atributo en un elemento
     * @param element Elemento al que se le asignará el atributo
     * @param name Nombre del atributo
     * @param value Valor del atributo
     */
    void setAttribute(void* element, const std::string& name, const std::string& value);

    /**
     * @brief Obtiene el valor de un atributo de un elemento
     * @param element Elemento del que se obtendrá el atributo
     * @param name Nombre del atributo
     * @return Valor del atributo o cadena vacía si no existe
     */
    std::string getAttribute(void* element, const std::string& name) const;

    /**
     * @brief Obtiene el nombre de la etiqueta de un elemento
     * @param element Elemento del que se obtendrá el nombre
     * @return Nombre de la etiqueta
     */
    std::string getTagName(void* element) const;

    /**
     * @brief Obtiene el tipo de un nodo
     * @param node Nodo del que se obtendrá el tipo
     * @return Tipo del nodo
     */
    NodeType getNodeType(void* node) const;

    /**
     * @brief Obtiene el contenido de texto de un nodo
     * @param node Nodo del que se obtendrá el texto
     * @return Contenido de texto
     */
    std::string getTextContent(void* node) const;

    /**
     * @brief Establece el contenido de texto de un nodo
     * @param node Nodo al que se le asignará el texto
     * @param text Contenido de texto
     */
    void setTextContent(void* node, const std::string& text);

    /**
     * @brief Obtiene el elemento raíz del documento
     * @return Puntero al elemento documento
     */
    void* getDocumentElement() const;

    /**
     * @brief Busca elementos por etiqueta
     * @param tagName Nombre de la etiqueta a buscar
     * @return Vector de punteros a los elementos encontrados
     */
    std::vector<void*> getElementsByTagName(const std::string& tagName) const;

    /**
     * @brief Busca un elemento por ID
     * @param id Valor del atributo id a buscar
     * @return Puntero al elemento encontrado o nullptr si no existe
     */
    void* getElementById(const std::string& id) const;

private:
    std::unique_ptr<Node> m_document;
    
    // Métodos auxiliares
    void collectElementsByTagName(Node* node, const std::string& tagName, std::vector<void*>& result) const;
    void* findElementById(Node* node, const std::string& id) const;
};

} // namespace Core
} // namespace BlackWidow

#endif // BLACKWIDOW_DOMTREE_H