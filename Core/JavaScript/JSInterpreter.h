#ifndef BLACKWIDOW_JSINTERPRETER_H
#define BLACKWIDOW_JSINTERPRETER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "../DOM/DOMTree.h"

namespace BlackWidow {
namespace Core {

/**
 * @brief Intérprete de JavaScript básico
 * 
 * Esta clase implementa un intérprete de JavaScript que permite
 * ejecutar scripts en el contexto de una página web y manipular
 * el DOM.
 */
class JSInterpreter {
public:
    // Tipo para callbacks de eventos
    using EventCallback = std::function<void(const std::string&, void*)>;
    
    JSInterpreter();
    ~JSInterpreter();

    /**
     * @brief Inicializa el intérprete de JavaScript
     */
    void initialize();

    /**
     * @brief Ejecuta un script JavaScript
     * @param script Código JavaScript a ejecutar
     * @param domTree Árbol DOM en cuyo contexto se ejecutará el script
     * @return Resultado de la ejecución del script (en formato JSON)
     */
    std::string executeScript(const std::string& script, DOMTree* domTree);

    /**
     * @brief Registra una función nativa que puede ser llamada desde JavaScript
     * @param name Nombre de la función en JavaScript
     * @param callback Función C++ a ejecutar
     */
    void registerNativeFunction(const std::string& name, std::function<std::string(const std::string&)> callback);

    /**
     * @brief Establece un manejador de eventos para un elemento del DOM
     * @param element Elemento al que se asignará el manejador
     * @param eventType Tipo de evento ("click", "mouseover", etc.)
     * @param script Código JavaScript a ejecutar cuando ocurra el evento
     */
    void setEventHandler(void* element, const std::string& eventType, const std::string& script);

    /**
     * @brief Dispara un evento en un elemento
     * @param element Elemento en el que se disparará el evento
     * @param eventType Tipo de evento a disparar
     * @param eventData Datos adicionales del evento (en formato JSON)
     */
    void triggerEvent(void* element, const std::string& eventType, const std::string& eventData = "{}");

private:
    // Estructuras internas para el intérprete
    struct InterpreterContext;
    std::unique_ptr<InterpreterContext> m_context;

    // Métodos privados para el procesamiento interno
    std::string evaluateExpression(const std::string& expression);
    void updateDOM(DOMTree* domTree, const std::string& selector, const std::string& property, const std::string& value);
    std::string getElementProperty(void* element, DOMTree* domTree, const std::string& property);
};

} // namespace Core
} // namespace BlackWidow

#endif // BLACKWIDOW_JSINTERPRETER_H