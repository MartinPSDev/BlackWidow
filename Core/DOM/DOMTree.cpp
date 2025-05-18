#include "DOMTree.h"
#include <algorithm>
#include <queue>

namespace BlackWidow {
namespace Core {

DOMTree::DOMTree() {
    // Crear el nodo documento raíz
    m_document = std::make_unique<Node>(NodeType::DOCUMENT_NODE);
    m_document->tagName = "#document";
}

DOMTree::~DOMTree() {
    // La limpieza se realiza automáticamente gracias a los smart pointers
}

void* DOMTree::createDocumentElement() {
    return m_document.get();
}

void* DOMTree::createElement(const std::string& tagName) {
    auto element = std::make_unique<Node>(NodeType::ELEMENT_NODE);
    element->tagName = tagName;
    
    // Devolver el puntero al nodo, pero mantener la propiedad
    Node* elementPtr = element.get();
    
    // Almacenar el elemento (se transferirá al padre cuando se llame a appendChild)
    m_document->children.push_back(std::move(element));
    
    return elementPtr;
}

void* DOMTree::createTextNode(const std::string& text) {
    auto textNode = std::make_unique<Node>(NodeType::TEXT_NODE);
    textNode->textContent = text;
    
    // Devolver el puntero al nodo, pero mantener la propiedad
    Node* textNodePtr = textNode.get();
    
    // Almacenar el nodo (se transferirá al padre cuando se llame a appendChild)
    m_document->children.push_back(std::move(textNode));
    
    return textNodePtr;
}

void* DOMTree::createCommentNode(const std::string& comment) {
    auto commentNode = std::make_unique<Node>(NodeType::COMMENT_NODE);
    commentNode->textContent = comment;
    
    // Devolver el puntero al nodo, pero mantener la propiedad
    Node* commentNodePtr = commentNode.get();
    
    // Almacenar el nodo (se transferirá al padre cuando se llame a appendChild)
    m_document->children.push_back(std::move(commentNode));
    
    return commentNodePtr;
}

void DOMTree::appendChild(void* parent, void* child) {
    if (!parent || !child) return;
    
    Node* parentNode = static_cast<Node*>(parent);
    Node* childNode = static_cast<Node*>(child);
    
    // Buscar el nodo hijo en los hijos del documento
    auto it = std::find_if(m_document->children.begin(), m_document->children.end(),
                          [childNode](const std::unique_ptr<Node>& node) {
                              return node.get() == childNode;
                          });
    
    if (it != m_document->children.end()) {
        // Transferir la propiedad del nodo hijo al padre
        std::unique_ptr<Node> childPtr = std::move(*it);
        m_document->children.erase(it);
        
        // Establecer el padre del nodo hijo
        childPtr->parent = parentNode;
        
        // Agregar el nodo hijo a los hijos del padre
        parentNode->children.push_back(std::move(childPtr));
    }
}

void DOMTree::setAttribute(void* element, const std::string& name, const std::string& value) {
    if (!element) return;
    
    Node* elementNode = static_cast<Node*>(element);
    if (elementNode->type != NodeType::ELEMENT_NODE) return;
    
    elementNode->attributes[name] = value;
}

std::string DOMTree::getAttribute(void* element, const std::string& name) const {
    if (!element) return "";
    
    Node* elementNode = static_cast<Node*>(element);
    if (elementNode->type != NodeType::ELEMENT_NODE) return "";
    
    auto it = elementNode->attributes.find(name);
    if (it != elementNode->attributes.end()) {
        return it->second;
    }
    
    return "";
}

std::string DOMTree::getTagName(void* element) const {
    if (!element) return "";
    
    Node* elementNode = static_cast<Node*>(element);
    if (elementNode->type != NodeType::ELEMENT_NODE) return "";
    
    return elementNode->tagName;
}

DOMTree::NodeType DOMTree::getNodeType(void* node) const {
    if (!node) return NodeType::ELEMENT_NODE; // Valor predeterminado
    
    Node* domNode = static_cast<Node*>(node);
    return domNode->type;
}

std::string DOMTree::getTextContent(void* node) const {
    if (!node) return "";
    
    Node* domNode = static_cast<Node*>(node);
    
    if (domNode->type == NodeType::TEXT_NODE || domNode->type == NodeType::COMMENT_NODE) {
        return domNode->textContent;
    } else if (domNode->type == NodeType::ELEMENT_NODE) {
        // Para elementos, concatenar el contenido de texto de todos los nodos hijos
        std::string result;
        for (const auto& child : domNode->children) {
            result += getTextContent(child.get());
        }
        return result;
    }
    
    return "";
}

void DOMTree::setTextContent(void* node, const std::string& text) {
    if (!node) return;
    
    Node* domNode = static_cast<Node*>(node);
    
    if (domNode->type == NodeType::TEXT_NODE || domNode->type == NodeType::COMMENT_NODE) {
        domNode->textContent = text;
    } else if (domNode->type == NodeType::ELEMENT_NODE) {
        // Para elementos, eliminar todos los nodos hijos y crear un nuevo nodo de texto
        domNode->children.clear();
        
        if (!text.empty()) {
            auto textNode = std::make_unique<Node>(NodeType::TEXT_NODE);
            textNode->textContent = text;
            textNode->parent = domNode;
            domNode->children.push_back(std::move(textNode));
        }
    }
}

void* DOMTree::getDocumentElement() const {
    return m_document.get();
}

std::vector<void*> DOMTree::getElementsByTagName(const std::string& tagName) const {
    std::vector<void*> result;
    collectElementsByTagName(m_document.get(), tagName, result);
    return result;
}

void* DOMTree::getElementById(const std::string& id) const {
    return findElementById(m_document.get(), id);
}

void DOMTree::collectElementsByTagName(Node* node, const std::string& tagName, std::vector<void*>& result) const {
    if (!node) return;
    
    // Buscar en todos los nodos usando BFS (Breadth-First Search)
    std::queue<Node*> queue;
    queue.push(node);
    
    while (!queue.empty()) {
        Node* current = queue.front();
        queue.pop();
        
        if (current->type == NodeType::ELEMENT_NODE) {
            if (tagName == "*" || current->tagName == tagName) {
                result.push_back(current);
            }
            
            // Agregar los hijos a la cola
            for (const auto& child : current->children) {
                queue.push(child.get());
            }
        }
    }
}

void* DOMTree::findElementById(Node* node, const std::string& id) const {
    if (!node) return nullptr;
    
    // Buscar en todos los nodos usando BFS (Breadth-First Search)
    std::queue<Node*> queue;
    queue.push(node);
    
    while (!queue.empty()) {
        Node* current = queue.front();
        queue.pop();
        
        if (current->type == NodeType::ELEMENT_NODE) {
            auto it = current->attributes.find("id");
            if (it != current->attributes.end() && it->second == id) {
                return current;
            }
            
            // Agregar los hijos a la cola
            for (const auto& child : current->children) {
                queue.push(child.get());
            }
        }
    }
    
    return nullptr;
}

} // namespace Core
} // namespace BlackWidow