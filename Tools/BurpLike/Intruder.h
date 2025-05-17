/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Herramienta Intruder - Similar a Burp Suite Intruder
 * Permite realizar ataques automatizados de fuerza bruta y fuzzing
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "Repeater.h" // Reutilizamos las estructuras HttpRequest y HttpResponse

namespace Tools::BurpLike {

/**
 * Enumeración que define los tipos de ataques disponibles
 */
enum class AttackType {
    SNIPER,     // Ataque en un solo punto de inserción
    BATTERING_RAM, // Mismo payload en todos los puntos de inserción
    PITCHFORK,  // Diferentes payloads sincronizados
    CLUSTER_BOMB // Todas las combinaciones posibles de payloads
};

/**
 * Estructura que representa un punto de inserción en una solicitud
 */
struct InsertionPoint {
    enum class Type {
        URL_PATH,
        URL_PARAMETER,
        BODY_PARAMETER,
        COOKIE,
        HEADER,
        CUSTOM
    };
    
    Type type;              // Tipo de punto de inserción
    std::string name;       // Nombre del parámetro o cabecera
    int startPosition;      // Posición de inicio en la solicitud
    int endPosition;        // Posición de fin en la solicitud
    
    InsertionPoint(Type t, const std::string& n, int start, int end) :
        type(t), name(n), startPosition(start), endPosition(end) {}
};

/**
 * Clase que implementa la funcionalidad de Intruder
 * Permite realizar ataques automatizados de fuerza bruta y fuzzing
 */
class Intruder {
public:
    /**
     * Constructor
     */
    Intruder();
    
    /**
     * Destructor
     */
    ~Intruder();
    
    /**
     * Establece la solicitud base para el ataque
     * @param request Solicitud HTTP base
     */
    void setBaseRequest(const HttpRequest& request);
    
    /**
     * Obtiene la solicitud base actual
     * @return Solicitud HTTP base
     */
    HttpRequest getBaseRequest() const;
    
    /**
     * Establece el tipo de ataque
     * @param type Tipo de ataque a realizar
     */
    void setAttackType(AttackType type);
    
    /**
     * Añade un punto de inserción a la solicitud
     * @param point Punto de inserción a añadir
     */
    void addInsertionPoint(const InsertionPoint& point);
    
    /**
     * Elimina todos los puntos de inserción
     */
    void clearInsertionPoints();
    
    /**
     * Añade una lista de payloads para un punto de inserción
     * @param position Índice del punto de inserción
     * @param payloads Lista de payloads a utilizar
     */
    void addPayloadList(int position, const std::vector<std::string>& payloads);
    
    /**
     * Carga una lista de payloads desde un archivo
     * @param position Índice del punto de inserción
     * @param filePath Ruta al archivo con los payloads
     * @return true si se cargó correctamente, false en caso contrario
     */
    bool loadPayloadFile(int position, const std::string& filePath);
    
    /**
     * Inicia el ataque con la configuración actual
     * @param callback Función a llamar por cada respuesta recibida
     * @return true si el ataque se inició correctamente, false en caso contrario
     */
    bool startAttack(std::function<void(const HttpRequest&, const HttpResponse&)> callback);
    
    /**
     * Detiene el ataque en curso
     */
    void stopAttack();
    
    /**
     * Comprueba si hay un ataque en curso
     * @return true si hay un ataque en curso, false en caso contrario
     */
    bool isAttackRunning() const;
    
    /**
     * Obtiene los resultados del ataque
     * @return Vector con los resultados (solicitud, respuesta)
     */
    std::vector<std::pair<HttpRequest, HttpResponse>> getResults() const;
    
    /**
     * Establece el intervalo entre solicitudes (en milisegundos)
     * @param interval Intervalo en milisegundos
     */
    void setRequestInterval(int interval);
    
    /**
     * Establece el número máximo de hilos concurrentes
     * @param threads Número de hilos
     */
    void setThreadCount(int threads);

private:
    // Solicitud base para el ataque
    HttpRequest m_baseRequest;
    
    // Tipo de ataque
    AttackType m_attackType;
    
    // Puntos de inserción
    std::vector<InsertionPoint> m_insertionPoints;
    
    // Listas de payloads para cada punto de inserción
    std::map<int, std::vector<std::string>> m_payloads;
    
    // Resultados del ataque
    std::vector<std::pair<HttpRequest, HttpResponse>> m_results;
    
    // Estado del ataque
    bool m_attackRunning;
    
    // Intervalo entre solicitudes (ms)
    int m_requestInterval;
    
    // Número de hilos concurrentes
    int m_threadCount;
    
    /**
     * Genera las solicitudes para el ataque según el tipo seleccionado
     * @return Vector con las solicitudes generadas
     */
    std::vector<HttpRequest> generateRequests();
    
    /**
     * Aplica un payload a una solicitud en un punto de inserción específico
     * @param request Solicitud base
     * @param point Punto de inserción
     * @param payload Payload a insertar
     * @return Solicitud modificada con el payload
     */
    HttpRequest applyPayload(const HttpRequest& request, const InsertionPoint& point, const std::string& payload);
};

} // namespace Tools::BurpLike