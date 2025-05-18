#ifndef BLACKWIDOW_DEVCONSOLE_H
#define BLACKWIDOW_DEVCONSOLE_H

#include <string>
#include <vector>
#include <functional>

namespace BlackWidow {
namespace UI {

/**
 * @brief Consola de desarrollador para BlackWidow
 * 
 * Proporciona una interfaz para ejecutar comandos JavaScript,
 * ver mensajes de consola y depurar aplicaciones web.
 */
class DevConsole {
public:
    DevConsole();
    ~DevConsole();

    /**
     * @brief Ejecuta código JavaScript en el contexto actual
     * @param code Código JavaScript a ejecutar
     * @return Resultado de la ejecución
     */
    std::string executeJavaScript(const std::string& code);

    /**
     * @brief Registra un mensaje en la consola
     * @param message Mensaje a registrar
     * @param level Nivel del mensaje (log, warn, error, info)
     */
    void logMessage(const std::string& message, const std::string& level = "log");

    /**
     * @brief Limpia todos los mensajes de la consola
     */
    void clearConsole();

    /**
     * @brief Establece una función de callback para cuando se registre un mensaje
     * @param callback Función a llamar cuando se registre un mensaje
     */
    void setMessageCallback(std::function<void(const std::string&, const std::string&)> callback);

private:
    struct ConsoleMessage {
        std::string content;
        std::string level;
        // Timestamp y otros metadatos podrían añadirse aquí
    };

    std::vector<ConsoleMessage> m_messages;
    std::function<void(const std::string&, const std::string&)> m_messageCallback;
};

} // namespace UI
} // namespace BlackWidow

#endif // BLACKWIDOW_DEVCONSOLE_H