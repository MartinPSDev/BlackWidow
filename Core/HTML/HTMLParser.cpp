#include "HTMLParser.h"
#include <sstream>
#include <stack>
#include <unordered_map>
#include <algorithm>

namespace BlackWidow {
namespace Core {

// Estructura interna para el contexto del analizador
struct HTMLParser::ParserContext {
    std::vector<std::string> tokens;
    std::stack<void*> elementStack;
    std::unique_ptr<DOMTree> domTree;
    std::string baseUrl;
    
    ParserContext() : domTree(std::make_unique<DOMTree>()) {}
};

HTMLParser::HTMLParser() : m_context(std::make_unique<ParserContext>()) {
}

HTMLParser::~HTMLParser() {
    // Limpieza de recursos
}

void HTMLParser::initialize() {
    // Configuración inicial del analizador HTML
    
    // Reiniciar el contexto
    m_context = std::make_unique<ParserContext>();
}

std::unique_ptr<DOMTree> HTMLParser::parse(const std::string& html, const std::string& baseUrl) {
    // Reiniciar el contexto para un nuevo análisis
    m_context = std::make_unique<ParserContext>();
    m_context->baseUrl = baseUrl;
    
    // Tokenizar el HTML
    tokenize(html);
    
    // Construir el árbol DOM
    buildTree();
    
    // Devolver el árbol DOM construido
    return std::move(m_context->domTree);
}

void HTMLParser::parseFragment(const std::string& html, DOMTree* domTree, void* parentElement) {
    if (!domTree || !parentElement) return;
    
    // Guardar el contexto actual
    auto savedContext = std::move(m_context);
    
    // Crear un nuevo contexto para el fragmento
    m_context = std::make_unique<ParserContext>();
    m_context->domTree.reset(domTree); // No tomar posesión, solo usar
    m_context->elementStack.push(parentElement);
    
    // Tokenizar y procesar el fragmento
    tokenize(html);
    buildTree();
    
    // Restaurar el contexto original
    m_context = std::move(savedContext);
}

std::string HTMLParser::serialize(const DOMTree* domTree) {
    if (!domTree) return "";
    
    std::stringstream output;
    
    // Aquí se implementaría la serialización real del árbol DOM a HTML
    // Por ahora, devolvemos un HTML básico
    output << "<!DOCTYPE html>\n<html>\n<head>\n";
    output << "  <title>Documento</title>\n";
    output << "</head>\n<body>\n";
    
    // Aquí se recorrería el árbol DOM y se generaría el HTML correspondiente
    
    output << "</body>\n</html>";
    
    return output.str();
}

void HTMLParser::tokenize(const std::string& html) {
    m_context->tokens.clear();
    
    // Implementación simplificada de tokenización
    // En una implementación real, se utilizaría un tokenizador más sofisticado
    
    std::string currentToken;
    bool inTag = false;
    bool inComment = false;
    
    for (size_t i = 0; i < html.length(); ++i) {
        char c = html[i];
        
        if (inComment) {
            currentToken += c;
            
            // Verificar fin de comentario
            if (i >= 2 && html[i-2] == '-' && html[i-1] == '-' && c == '>') {
                m_context->tokens.push_back(currentToken);
                currentToken.clear();
                inComment = false;
                inTag = false;
            }
            continue;
        }
        
        if (inTag) {
            currentToken += c;
            
            // Verificar inicio de comentario
            if (currentToken == "<!--") {
                inComment = true;
                continue;
            }
            
            if (c == '>') {
                m_context->tokens.push_back(currentToken);
                currentToken.clear();
                inTag = false;
            }
        } else {
            if (c == '<') {
                if (!currentToken.empty()) {
                    m_context->tokens.push_back(currentToken);
                    currentToken.clear();
                }
                currentToken += c;
                inTag = true;
            } else {
                currentToken += c;
            }
        }
    }
    
    // Agregar el último token si existe
    if (!currentToken.empty()) {
        m_context->tokens.push_back(currentToken);
    }
}

void HTMLParser::buildTree() {
    // Crear el documento raíz
    void* documentElement = m_context->domTree->createDocumentElement();
    m_context->elementStack.push(documentElement);
    
    // Procesar todos los tokens
    for (const auto& token : m_context->tokens) {
        processToken(token);
    }
    
    // Asegurarse de que solo quede el documento raíz en la pila
    while (m_context->elementStack.size() > 1) {
        m_context->elementStack.pop();
    }
}

void HTMLParser::processToken(const std::string& token) {
    if (token.empty()) return;
    
    if (token[0] == '<') {
        // Es una etiqueta
        if (token.size() > 1 && token[1] == '/') {
            // Es una etiqueta de cierre
            std::string tagName = token.substr(2, token.size() - 3);
            // Eliminar posibles espacios y atributos
            size_t spacePos = tagName.find(' ');
            if (spacePos != std::string::npos) {
                tagName = tagName.substr(0, spacePos);
            }
            handleEndTag(tagName);
        } else if (token.size() > 3 && token.substr(0, 4) == "<!--") {
            // Es un comentario
            std::string comment = token.substr(4, token.size() - 7);
            handleComment(comment);
        } else {
            // Es una etiqueta de apertura
            std::string tagContent = token.substr(1, token.size() - 2);
            // Verificar si es una etiqueta auto-cerrada
            bool selfClosing = false;
            if (!tagContent.empty() && tagContent.back() == '/') {
                selfClosing = true;
                tagContent.pop_back();
            }
            
            // Extraer el nombre de la etiqueta y los atributos
            std::string tagName;
            std::vector<std::pair<std::string, std::string>> attributes;
            
            size_t spacePos = tagContent.find(' ');
            if (spacePos != std::string::npos) {
                tagName = tagContent.substr(0, spacePos);
                std::string attributesStr = tagContent.substr(spacePos + 1);
                
                // Parsear atributos (implementación simplificada)
                std::istringstream attributeStream(attributesStr);
                std::string attribute;
                while (attributeStream >> attribute) {
                    size_t equalsPos = attribute.find('=');
                    if (equalsPos != std::string::npos) {
                        std::string name = attribute.substr(0, equalsPos);
                        std::string value = attribute.substr(equalsPos + 1);
                        
                        // Eliminar comillas si existen
                        if (value.size() >= 2 && (value.front() == '"' || value.front() == '\'') && 
                            value.front() == value.back()) {
                            value = value.substr(1, value.size() - 2);
                        }
                        
                        attributes.emplace_back(name, value);
                    } else {
                        // Atributo sin valor
                        attributes.emplace_back(attribute, "");
                    }
                }
            } else {
                tagName = tagContent;
            }
            
            // Procesar la etiqueta
            handleStartTag(tagName, attributes);
            
            // Si es auto-cerrada, procesar también el cierre
            if (selfClosing) {
                handleEndTag(tagName);
            }
        }
    } else {
        // Es texto
        handleText(token);
    }
}

void HTMLParser::handleStartTag(const std::string& tag, const std::vector<std::pair<std::string, std::string>>& attributes) {
    if (m_context->elementStack.empty()) return;
    
    void* parentElement = m_context->elementStack.top();
    void* newElement = m_context->domTree->createElement(tag);
    
    // Agregar atributos al elemento
    for (const auto& attr : attributes) {
        m_context->domTree->setAttribute(newElement, attr.first, attr.second);
    }
    
    // Agregar el elemento al árbol
    m_context->domTree->appendChild(parentElement, newElement);
    
    // Elementos que no tienen etiqueta de cierre
    static const std::unordered_map<std::string, bool> voidElements = {
        {"area", true}, {"base", true}, {"br", true}, {"col", true},
        {"embed", true}, {"hr", true}, {"img", true}, {"input", true},
        {"link", true}, {"meta", true}, {"param", true}, {"source", true},
        {"track", true}, {"wbr", true}
    };
    
    // Si no es un elemento vacío, agregarlo a la pila
    if (voidElements.find(tag) == voidElements.end()) {
        m_context->elementStack.push(newElement);
    }
}

void HTMLParser::handleEndTag(const std::string& tag) {
    // Buscar la etiqueta correspondiente en la pila y cerrarla
    std::stack<void*> tempStack;
    bool found = false;
    
    while (!m_context->elementStack.empty() && !found) {
        void* element = m_context->elementStack.top();
        m_context->elementStack.pop();
        
        std::string elementTag = m_context->domTree->getTagName(element);
        if (elementTag == tag) {
            found = true;
            break;
        }
        
        tempStack.push(element);
    }
    
    // Si no se encontró la etiqueta, restaurar la pila
    if (!found) {
        while (!tempStack.empty()) {
            m_context->elementStack.push(tempStack.top());
            tempStack.pop();
        }
    } else {
        // Descartar los elementos que quedaron sin cerrar
        tempStack = std::stack<void*>();
    }
}

void HTMLParser::handleText(const std::string& text) {
    if (m_context->elementStack.empty()) return;
    
    // Verificar si el texto solo contiene espacios en blanco
    bool onlyWhitespace = true;
    for (char c : text) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            onlyWhitespace = false;
            break;
        }
    }
    
    // Si no es solo espacios en blanco, crear un nodo de texto
    if (!onlyWhitespace) {
        void* parentElement = m_context->elementStack.top();
        void* textNode = m_context->domTree->createTextNode(text);
        m_context->domTree->appendChild(parentElement, textNode);
    }
}

void HTMLParser::handleComment(const std::string& comment) {
    if (m_context->elementStack.empty()) return;
    
    void* parentElement = m_context->elementStack.top();
    void* commentNode = m_context->domTree->createCommentNode(comment);
    m_context->domTree->appendChild(parentElement, commentNode);
}

} // namespace Core
} // namespace BlackWidow