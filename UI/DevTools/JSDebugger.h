#ifndef BLACKWIDOW_JSDEBUGGER_H
#define BLACKWIDOW_JSDEBUGGER_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace BlackWidow {
namespace UI {

/**
 * @brief Depurador de JavaScript para BlackWidow
 * 
 * Permite depurar código JavaScript, establecer puntos de interrupción,
 * inspeccionar variables y ejecutar código paso a paso.
 */
class JSDebugger {
public:
    JSDebugger();
    ~JSDebugger();

    /**
     * @brief Establece un punto de interrupción en un archivo JavaScript
     * @param url URL del archivo JavaScript
     * @param lineNumber Número de línea para el punto de interrupción
     * @return ID del punto de interrupción o cadena vacía si no se pudo establecer
     */
    std::string setBreakpoint(const std::string& url, int lineNumber);

    /**
     * @brief Elimina un punto de interrupción
     * @param breakpointId ID del punto de interrupción a eliminar
     * @return true si se eliminó correctamente, false en caso contrario
     */
    bool removeBreakpoint(const std::string& breakpointId);

    /**
     * @brief Continúa la ejecución después de una pausa
     */
    void continueExecution();

    /**
     * @brief Ejecuta la siguiente instrucción
     */
    void stepOver();

    /**
     * @brief Entra en la función actual
     */
    void stepInto();

    /**
     * @brief Sale de la función actual
     */
    void stepOut();

    /**
     * @brief Pausa la ejecución de JavaScript
     */
    void pauseExecution();

    /**
     * @brief Evalúa una expresión en el contexto actual
     * @param expression Expresión a evaluar
     * @return Resultado de la evaluación
     */
    std::string evaluateExpression(const std::string& expression);

    /**
     * @brief Obtiene la pila de llamadas actual
     * @return Vector con la información de la pila de llamadas
     */
    std::vector<std::string> getCallStack();

    /**
     * @brief Obtiene las variables en el ámbito actual
     * @return Mapa de nombres de variables y sus valores
     */
    std::map<std::string, std::string> getLocalVariables();

    /**
     * @brief Establece una función de callback para cuando se alcance un punto de interrupción
     * @param callback Función a llamar cuando se alcance un punto de interrupción
     */
    void setBreakpointCallback(std::function<void(const std::string&, int)> callback);

private:
    // Estructura para almacenar información sobre puntos de interrupción
    struct Breakpoint {
        std::string id;
        std::string url;
        int lineNumber;
        bool enabled;
    };

    std::vector<Breakpoint> m_breakpoints;
    std::function<void(const std::string&, int)> m_breakpointCallback;
    bool m_isPaused;
};

} // namespace UI
} // namespace BlackWidow

#endif // BLACKWIDOW_JSDEBUGGER_H