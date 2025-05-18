#include "../UI/Widgets/DevToolsWidget.h"
#include "../Core/Rendering/RenderingEngine.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace BlackWidow;

// Ejemplo de cómo utilizar el motor de renderizado
int main() {
    // Inicializar el motor de renderizado
    Core::RenderingEngine renderEngine;
    renderEngine.initialize();
    
    // Inicializar las herramientas de desarrollo
    UI::DevToolsWidget devTools;
    devTools.initialize();
    
    // Cargar un archivo HTML de ejemplo
    std::ifstream htmlFile("Examples/test_page.html");
    if (!htmlFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo HTML de ejemplo" << std::endl;
        return 1;
    }
    
    // Leer el contenido del archivo
    std::string html((std::istreambuf_iterator<char>(htmlFile)), std::istreambuf_iterator<char>());
    htmlFile.close();
    
    // Renderizar la página
    std::string pageId = renderEngine.renderPage(html, "file:///Examples/test_page.html");
    std::cout << "Página renderizada con ID: " << pageId << std::endl;
    
    // Conectar las herramientas de desarrollo a la página
    devTools.openForPage(pageId);
    
    // Ejecutar un script JavaScript en la página
    std::string script = "document.getElementById('result').innerHTML = '<p>Prueba ejecutada desde C++</p>'";
    std::string result = renderEngine.executeJavaScript(pageId, script);
    std::cout << "Resultado de la ejecución del script: " << result << std::endl;
    
    // Obtener una captura del estado actual de la página
    auto snapshot = renderEngine.getPageSnapshot(pageId);
    std::cout << "Tamaño de la captura: " << snapshot.size() << " bytes" << std::endl;
    
    // En una aplicación real, aquí se integraría con el bucle principal de la aplicación
    std::cout << "Ejemplo completado con éxito" << std::endl;
    
    return 0;
}