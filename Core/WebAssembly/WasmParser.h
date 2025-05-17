/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Parser de WebAssembly
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Core::WebAssembly {

/**
 * Clase que implementa el parser de WebAssembly
 * Analiza y valida módulos WebAssembly
 */
class WasmParser {
public:
    /**
     * Constructor
     */
    WasmParser();
    
    /**
     * Destructor
     */
    ~WasmParser();

    /**
     * Parsea un módulo WebAssembly desde código binario
     * @param binary_code Código binario WebAssembly
     * @return true si el parsing fue exitoso, false en caso contrario
     */
    bool parseModule(const std::vector<uint8_t>& binary_code);

    /**
     * Parsea un módulo WebAssembly desde código textual (WAT)
     * @param text_code Código textual WebAssembly
     * @return true si el parsing fue exitoso, false en caso contrario
     */
    bool parseModuleFromText(const std::string& text_code);

    /**
     * Valida la estructura de un módulo WebAssembly
     * @param binary_code Código binario WebAssembly
     * @return true si la estructura es válida, false en caso contrario
     */
    bool validateStructure(const std::vector<uint8_t>& binary_code);

    /**
     * Extrae las funciones exportadas de un módulo WebAssembly
     * @return Lista de nombres de funciones exportadas
     */
    std::vector<std::string> getExportedFunctions();

    /**
     * Extrae las importaciones requeridas por un módulo WebAssembly
     * @return Lista de nombres de importaciones requeridas
     */
    std::vector<std::string> getRequiredImports();

private:
    // Implementación interna del parser WebAssembly
    class WasmParserImpl;
    std::unique_ptr<WasmParserImpl> m_impl;

    // Métodos privados para el análisis
    bool parseHeader(const std::vector<uint8_t>& binary_code);
    bool parseSections(const std::vector<uint8_t>& binary_code);
};

} // namespace Core::WebAssembly