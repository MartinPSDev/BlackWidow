#include "DOMInspector.h"
#include "../../Core/Rendering/RenderingEngine.h"
#include <iostream>
#include <sstream>
#include <json/json.h>

namespace BlackWidow {
namespace UI {

DOMInspector::DOMInspector() {
    // Inicialización del inspector DOM
    m_currentPageId = "";
    m_renderEngine = nullptr;
    m_highlightedElements.clear();
}

DOMInspector::~DOMInspector() {
    // Limpieza de recursos
    clearHighlights();
}

void DOMInspector::connectToRenderingEngine(Core::RenderingEngine* engine, const std::string& pageId) {
    m_renderEngine = engine;
    m_currentPageId = pageId;
    
    if (m_renderEngine) {
        // Conectar las herramientas de desarrollo al motor de renderizado
        m_renderEngine->connectDevTools(m_currentPageId, true);
        std::cout << "DOMInspector conectado a la página: " << m_currentPageId << std::endl;
    }
}

std::string DOMInspector::selectElement(const std::string& selector) {
    // Implementación para seleccionar un elemento usando un selector CSS
    // Esto interactuará con el motor de renderizado del navegador
    
    if (!m_renderEngine || m_currentPageId.empty()) {
        std::cerr << "Error: DOMInspector no está conectado a ninguna página" << std::endl;
        return "";
    }
    
    // Ejecutar un script para seleccionar el elemento usando el selector CSS
    std::string script = "(function() { "
                        "  var element = document.querySelector('" + selector + "'); "
                        "  if (!element) return null; "
                        "  var id = element.id || 'auto_id_' + Math.random().toString(36).substr(2, 9); "
                        "  if (!element.id) element.id = id; "
                        "  return { "
                        "    id: id, "
                        "    tagName: element.tagName.toLowerCase(), "
                        "    selector: '" + selector + "' "
                        "  }; "
                        "})()";
    
    std::string result = m_renderEngine->executeJavaScript(m_currentPageId, script);
    
    // Parsear el resultado JSON
    Json::Value root;
    Json::Reader reader;
    if (reader.parse(result, root) && !root.isNull() && root.isMember("result")) {
        Json::Value elementData = root["result"];
        if (!elementData.isNull()) {
            ElementInfo element;
            element.id = elementData["id"].asString();
            element.tagName = elementData["tagName"].asString();
            element.selector = elementData["selector"].asString();
            
            // Almacenar el elemento seleccionado
            m_selectedElements.push_back(element);
            return element.id;
        }
    }
    
    return "";
}

std::string DOMInspector::getElementHTML(const std::string& elementId) {
    // Obtener el HTML de un elemento específico
    if (!m_renderEngine || m_currentPageId.empty()) {
        return "<div>Error: No hay conexión con el motor de renderizado</div>";
    }
    
    std::string script = "(function() { "
                        "  var element = document.getElementById('" + elementId + "'); "
                        "  return element ? element.outerHTML : null; "
                        "})()";
    
    std::string result = m_renderEngine->executeJavaScript(m_currentPageId, script);
    
    // Parsear el resultado JSON
    Json::Value root;
    Json::Reader reader;
    if (reader.parse(result, root) && !root.isNull() && root.isMember("result")) {
        return root["result"].asString();
    }
    
    return "<div class=\"error\">Elemento no encontrado</div>";
}

bool DOMInspector::setElementHTML(const std::string& elementId, const std::string& html) {
    // Modificar el HTML de un elemento
    if (!m_renderEngine || m_currentPageId.empty()) {
        return false;
    }
    
    // Escapar las comillas en el HTML para el script JavaScript
    std::string escapedHtml = html;
    size_t pos = 0;
    while ((pos = escapedHtml.find("\"", pos)) != std::string::npos) {
        escapedHtml.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    std::string script = "(function() { "
                        "  var element = document.getElementById('" + elementId + "'); "
                        "  if (!element) return false; "
                        "  element.outerHTML = \"" + escapedHtml + "\"; "
                        "  return true; "
                        "})()";
    
    std::string result = m_renderEngine->executeJavaScript(m_currentPageId, script);
    
    // Parsear el resultado JSON
    Json::Value root;
    Json::Reader reader;
    if (reader.parse(result, root) && !root.isNull() && root.isMember("result")) {
        return root["result"].asBool();
    }
    
    return false;
}

std::string DOMInspector::getComputedStyles(const std::string& elementId) {
    // Obtener los estilos computados de un elemento
    if (!m_renderEngine || m_currentPageId.empty()) {
        return "{}";
    }
    
    std::string script = "(function() { "
                        "  var element = document.getElementById('" + elementId + "'); "
                        "  if (!element) return {}; "
                        "  var styles = window.getComputedStyle(element); "
                        "  var result = {}; "
                        "  for (var i = 0; i < styles.length; i++) { "
                        "    var prop = styles[i]; "
                        "    result[prop] = styles.getPropertyValue(prop); "
                        "  } "
                        "  return result; "
                        "})()";
    
    std::string result = m_renderEngine->executeJavaScript(m_currentPageId, script);
    
    // Parsear el resultado JSON
    Json::Value root;
    Json::Reader reader;
    if (reader.parse(result, root) && !root.isNull() && root.isMember("result")) {
        // Convertir el objeto de resultado a una cadena JSON formateada
        Json::StyledWriter writer;
        return writer.write(root["result"]);
    }
    
    return "{}";
}

bool DOMInspector::setElementStyle(const std::string& elementId, const std::string& property, const std::string& value) {
    // Modificar un estilo específico de un elemento
    if (!m_renderEngine || m_currentPageId.empty()) {
        return false;
    }
    
    std::string script = "(function() { "
                        "  var element = document.getElementById('" + elementId + "'); "
                        "  if (!element) return false; "
                        "  element.style['" + property + "'] = '" + value + "'; "
                        "  return true; "
                        "})()";
    
    std::string result = m_renderEngine->executeJavaScript(m_currentPageId, script);
    
    // Parsear el resultado JSON
    Json::Value root;
    Json::Reader reader;
    if (reader.parse(result, root) && !root.isNull() && root.isMember("result")) {
        return root["result"].asBool();
    }
    
    return false;
}

void DOMInspector::highlightElement(const std::string& elementId) {
    // Resaltar visualmente un elemento en la página
    if (!m_renderEngine || m_currentPageId.empty()) {
        return;
    }
    
    // Agregar el elemento a la lista de elementos resaltados
    m_highlightedElements.insert(elementId);
    
    // Crear un estilo de resaltado con un borde y fondo semitransparente
    std::string script = "(function() { "
                        "  var element = document.getElementById('" + elementId + "'); "
                        "  if (!element) return false; "
                        "  var originalOutline = element.style.outline; "
                        "  var originalOutlineOffset = element.style.outlineOffset; "
                        "  var originalZIndex = element.style.zIndex; "
                        "  var originalPosition = element.style.position; "
                        "  element._bw_original_styles = { "
                        "    outline: originalOutline, "
                        "    outlineOffset: originalOutlineOffset, "
                        "    zIndex: originalZIndex, "
                        "    position: originalPosition "
                        "  }; "
                        "  element.style.outline = '2px solid #7b1fa2'; "
                        "  element.style.outlineOffset = '-2px'; "
                        "  if (element.style.position === 'static') { "
                        "    element.style.position = 'relative'; "
                        "  } "
                        "  element.style.zIndex = '9999'; "
                        "  return true; "
                        "})()";
    
    m_renderEngine->executeJavaScript(m_currentPageId, script);
}

void DOMInspector::clearHighlights() {
    // Eliminar todos los resaltados
    if (!m_renderEngine || m_currentPageId.empty() || m_highlightedElements.empty()) {
        return;
    }
    
    // Restaurar los estilos originales de todos los elementos resaltados
    std::string script = "(function() { "
                        "  var elements = [";
    
    bool first = true;
    for (const auto& elementId : m_highlightedElements) {
        if (!first) script += ",";
        script += "'" + elementId + "'";
        first = false;
    }
    
    script += "]; "
              "  for (var i = 0; i < elements.length; i++) { "
              "    var element = document.getElementById(elements[i]); "
              "    if (element && element._bw_original_styles) { "
              "      element.style.outline = element._bw_original_styles.outline; "
              "      element.style.outlineOffset = element._bw_original_styles.outlineOffset; "
              "      element.style.zIndex = element._bw_original_styles.zIndex; "
              "      element.style.position = element._bw_original_styles.position; "
              "      delete element._bw_original_styles; "
              "    } "
              "  } "
              "  return true; "
              "})()";
    
    m_renderEngine->executeJavaScript(m_currentPageId, script);
    
    // Limpiar la lista de elementos resaltados
    m_highlightedElements.clear();
}

} // namespace UI
} // namespace BlackWidow