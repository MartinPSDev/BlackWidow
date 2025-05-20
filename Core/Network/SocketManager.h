/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Gestor de sockets para conexiones de red de bajo nivel
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace Core::Network {

/**
 * Clase que gestiona las conexiones de socket de bajo nivel
 * Proporciona una interfaz para crear, gestionar y monitorizar sockets
 */
class SocketManager {
public:
    /**
     * Constructor
     */
    SocketManager();
    
    /**
     * Destructor
     */
    ~SocketManager();

    /**
     * Crea un socket TCP
     * @param host Host al que conectarse
     * @param port Puerto al que conectarse
     * @param secure true para conexión SSL/TLS, false para conexión sin cifrar
     * @return ID del socket creado, o -1 en caso de error
     */
    int createTcpSocket(const std::string& host, int port, bool secure = false);

    /**
     * Envía datos a través de un socket
     * @param socket_id ID del socket
     * @param data Datos a enviar
     * @return true si los datos fueron enviados correctamente, false en caso contrario
     */
    bool sendData(int socket_id, const std::vector<uint8_t>& data);

    /**
     * Recibe datos de un socket
     * @param socket_id ID del socket
     * @param callback Función de callback para los datos recibidos
     * @return true si la operación fue iniciada correctamente, false en caso contrario
     */
    bool receiveData(int socket_id, std::function<void(const std::vector<uint8_t>&)> callback);

    /**
     * Cierra un socket
     * @param socket_id ID del socket
     * @return true si el socket fue cerrado correctamente, false en caso contrario
     */
    bool closeSocket(int socket_id);

    /**
     * Configura un proxy para las conexiones de socket
     * @param host Host del proxy
     * @param port Puerto del proxy
     * @param username Usuario para autenticación (opcional)
     * @param password Contraseña para autenticación (opcional)
     * @return true si el proxy fue configurado correctamente, false en caso contrario
     */
    bool setProxy(const std::string& host, int port, const std::string& username = "", const std::string& password = "");

private:
    // Implementación privada del gestor de sockets
    class SocketManagerImpl;
    std::unique_ptr<SocketManagerImpl> m_impl;

    // Mapa de sockets activos
    std::unordered_map<int, bool> m_activeSockets;

    // Configuración del proxy
    struct {
        std::string host;
        int port;
        std::string username;
        std::string password;
        bool enabled;
    } m_proxyConfig;

    // Contador para generar IDs de socket
    int m_nextSocketId;

    // Métodos privados para la gestión de sockets
    bool initializeSocketLibrary();
    void cleanupSocketLibrary();
    bool configureSocketOptions(int socket_id);
};

} // namespace Core::Network