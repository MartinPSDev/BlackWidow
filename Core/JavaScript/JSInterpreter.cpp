#include "JSInterpreter.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace BlackWidow {
namespace Core {

// Estructura interna para el contexto del intérprete
struct JSInterpreter::InterpreterContext {
    // Funciones nativas registradas
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> nativeFunctions;
    
    // Manejadores de eventos para elementos
    std::unordered_map<void*, std::unordered_map<std::string, std::string>> eventHandlers;
    
    // Variables globales del contexto
    std::unordered_map<std::string, std::string> globalVariables;
    
    // Árbol DOM actual
    DOMTree* currentDomTree;
    
    InterpreterContext() : currentDomTree(nullptr) {}
};

JSInterpreter::JSInterpreter() : m_context(std::make_unique<InterpreterContext>()) {
}

JSInterpreter::~JSInterpreter() {
    // Limpieza de recursos
}

void JSInterpreter::initialize() {
    // Configuración inicial del intérprete
    
    // Reiniciar el contexto
    m_context = std::make_unique<InterpreterContext>();
    
    // Registrar funciones nativas básicas
    registerNativeFunction("console.log", [](const std::string& args) {
        // En una implementación real, esto podría enviar mensajes a la consola de desarrollo
        return "null";
    });
    
    registerNativeFunction("document.getElementById", [this](const std::string& args) {
        if (!m_context->currentDomTree) return "null";
        
        // Extraer el ID del argumento
        std::string id = args;
        // Eliminar comillas si existen
        if (id.size() >= 2 && (id.front() == '"' || id.front() == '\'') && 
            id.front() == id.back()) {
            id = id.substr(1, id.size() - 2);
        }
        
        // Buscar el elemento por ID
        void* element = m_context->currentDomTree->getElementById(id);
        if (element) {
            // Devolver una referencia al elemento (en una implementación real, esto sería un objeto JavaScript)
            return "{ \"_elementRef\": \"" + id + "\" }";
        }
        
        return "null";
    });
    
    registerNativeFunction("document.createElement", [this](const std::string& args) {
        if (!m_context->currentDomTree) return "null";
        
        // Extraer el nombre de la etiqueta del argumento
        std::string tagName = args;
        // Eliminar comillas si existen
        if (tagName.size() >= 2 && (tagName.front() == '"' || tagName.front() == '\'') && 
            tagName.front() == tagName.back()) {
            tagName = tagName.substr(1, tagName.size() - 2);
        }
        
        // Crear el elemento
        void* element = m_context->currentDomTree->createElement(tagName);
        if (element) {
            // Generar un ID único para el elemento
            std::string id = "js_element_" + std::to_string(reinterpret_cast<uintptr_t>(element));
            m_context->currentDomTree->setAttribute(element, "id", id);
            
            // Devolver una referencia al elemento
            return "{ \"_elementRef\": \"" + id + "\" }";
        }
        
        return "null";
    });
}

std::string JSInterpreter::executeScript(const std::string& script, DOMTree* domTree) {
    if (!domTree) return "{ \"error\": \"No DOM tree provided\" }";
    
    // Establecer el árbol DOM actual
    m_context->currentDomTree = domTree;
    
    // En una implementación real, aquí se analizaría y ejecutaría el script
    // Por ahora, implementamos un procesamiento muy básico
    
    try {
        // Dividir el script en líneas
        std::istringstream stream(script);
        std::string line;
        std::string result;
        
        while (std::getline(stream, line)) {
            // Eliminar espacios en blanco al inicio y al final
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            
            if (line.empty() || line.front() == '/') continue; // Ignorar líneas vacías y comentarios
            
            // Procesar la línea
            if (line.back() == ';') {
                line.pop_back(); // Eliminar el punto y coma final
            }
            
            // Detectar llamadas a funciones nativas
            size_t openParenPos = line.find('(');
            if (openParenPos != std::string::npos && line.back() == ')') {
                std::string funcName = line.substr(0, openParenPos);
                std::string args = line.substr(openParenPos + 1, line.size() - openParenPos - 2);
                
                // Buscar la función nativa
                auto it = m_context->nativeFunctions.find(funcName);
                if (it != m_context->nativeFunctions.end()) {
                    // Llamar a la función nativa
                    result = it->second(args);
                }
            }
            // Aquí se implementarían más casos para otros tipos de instrucciones
        }
        
        // Devolver el resultado de la última operación
        if (result.empty()) {
            result = "null";
        }
        
        return "{ \"result\": " + result + " }";
    } catch (const std::exception& e) {
        return "{ \"error\": \"" + std::string(e.what()) + "\" }";
    }
}

void JSInterpreter::registerNativeFunction(const std::string& name, std::function<std::string(const std::string&)> callback) {
    m_context->nativeFunctions[name] = callback;
}

void JSInterpreter::setEventHandler(void* element, const std::string& eventType, const std::string& script) {
    if (!element) return;
    
    // Almacenar el manejador de eventos
    m_context->eventHandlers[element][eventType] = script;
}

void JSInterpreter::triggerEvent(void* element, const std::string& eventType, const std::string& eventData) {
    if (!element || !m_context->currentDomTree) return;
    
    // Buscar el manejador de eventos
    auto elementIt = m_context->eventHandlers.find(element);
    if (elementIt != m_context->eventHandlers.end()) {
        auto eventIt = elementIt->second.find(eventType);
        if (eventIt != elementIt->second.end()) {
            // Ejecutar el script del manejador
            executeScript(eventIt->second, m_context->currentDomTree);
        }
    }
}

std::string JSInterpreter::evaluateExpression(const std::string& expression) {
    // En una implementación real, aquí se evaluaría la expresión JavaScript
    // Por ahora, devolvemos un valor predeterminado
    return "null";
}

void JSInterpreter::updateDOM(DOMTree* domTree, const std::string& selector, const std::string& property, const std::string& value) {
    if (!domTree) return;
    
    // Implementación simplificada: solo soportamos selectores de ID
    if (selector.size() > 1 && selector.front() == '#') {
        std::string id = selector.substr(1);
        void* element = domTree->getElementById(id);
        
        if (element) {
            // Actualizar la propiedad del elemento
            if (property == "textContent") {
                domTree->setTextContent(element, value);
            } else {
                domTree->setAttribute(element, property, value);
            }
        }
    }
}

std::string JSInterpreter::getElementProperty(void* element, DOMTree* domTree, const std::string& property) {
    if (!element || !domTree) return "null";
    
    // Obtener la propiedad del elemento
    if (property == "textContent") {
        return "\"" + domTree->getTextContent(element) + "\"";
    } else if (property == "tagName") {
        return "\"" + domTree->getTagName(element) + "\"";
    } else {
        return "\"" + domTree->getAttribute(element, property) + "\"";
    }
}

} // namespace Core
} // namespace BlackWidow