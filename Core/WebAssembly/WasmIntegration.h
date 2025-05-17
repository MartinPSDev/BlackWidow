/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Integración de WebAssembly con el navegador
 */

#pragma once

#include "WasmEngine.h"
#include "WasmParser.h"
#include <memory>
#include <string>

namespace Core::Browser {
    class Browser;
}

namespace Core::WebAssembly {

/**
 * Clase que integra WebAssembly con el navegador
 * Proporciona una interfaz para que el navegador interactúe con el motor WebAssembly
 */
class WasmIntegration {
public:
    /**
     * Constructor
     * @param browser Puntero al navegador principal
     */
    explicit WasmIntegration(std::shared_ptr<Core::Browser::Browser> browser);
    
    /**
     * Destructor
     */
    ~WasmIntegration();

    /**
     * Inicializa el entorno WebAssembly
     * @return true si la inicialización fue exitosa, false en caso contrario
     */
    bool initialize();

    /**
     * Ejecuta un módulo WebAssembly desde una URL
     * @param url URL del módulo WebAssembly
     * @return true si la ejecución fue exitosa, false en caso contrario
     */
    bool executeModuleFromUrl(const std::string& url);

    /**
     * Ejecuta un módulo WebAssembly desde código binario
     * @param binary_code Código binario WebAssembly
     * @return true si la ejecución fue exitosa, false en caso contrario
     */
    bool executeModule(const std::vector<uint8_t>& binary_code);

    /**
     * Analiza un módulo WebAssembly en busca de vulnerabilidades
     * @param binary_code Código binario WebAssembly
     * @return true si el módulo es seguro, false en caso contrario
     */
    bool analyzeModuleSecurity(const std::vector<uint8_t>& binary_code);

private:
    // Puntero al navegador principal
    std::shared_ptr<Core::Browser::Browser> m_browser;
    
    // Motor WebAssembly
    std::unique_ptr<WasmEngine> m_engine;
    
    // Parser WebAssembly
    std::unique_ptr<WasmParser> m_parser;

    // Métodos privados para la gestión de módulos
    bool loadModule(const std::vector<uint8_t>& binary_code);
    bool setupEnvironment();
};

} // namespace Core::WebAssembly