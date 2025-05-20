# Módulo Analyzer de BlackWidow

Este módulo proporciona analizadores para detectar y explotar vulnerabilidades web comunes, trabajando en conjunto con los payloads definidos en el módulo Payloads.

## Analizadores Disponibles

### XxeAnalyzer
Analizador para vulnerabilidades de XML External Entity (XXE).

```cpp
// Ejemplo de uso
#include "Security/Analyzer/XxeAnalyzer.h"

BlackWidow::Security::Analyzer::XxeAnalyzer analyzer;

// Detectar vulnerabilidad XXE
auto result = analyzer.detectVulnerability("http://ejemplo.com/xml-endpoint");
if (result.vulnerable) {
    std::cout << "Vulnerabilidad XXE detectada: " << result.vulnerabilityType << std::endl;
    std::cout << "Payload: " << result.payload << std::endl;
}

// Explotar para leer archivos
auto fileResult = analyzer.exploitFileRead("http://ejemplo.com/xml-endpoint", "/etc/passwd");
```

### XssAnalyzer
Analizador para vulnerabilidades de Cross-Site Scripting (XSS).

```cpp
// Ejemplo de uso
#include "Security/Analyzer/XssAnalyzer.h"

BlackWidow::Security::Analyzer::XssAnalyzer analyzer;

// Detectar vulnerabilidad XSS
std::map<std::string, std::string> params = {
    {"q", "test"},
    {"id", "1"}
};
auto result = analyzer.detectVulnerability("http://ejemplo.com/search", params);
if (result.vulnerable) {
    std::cout << "Vulnerabilidad XSS detectada: " << result.vulnerabilityType << std::endl;
    std::cout << "Contexto: " << result.context << std::endl;
    std::cout << "Nivel de impacto: " << result.impactLevel << std::endl;
}
```

### SqlInjectionAnalyzer
Analizador para vulnerabilidades de inyección SQL.

```cpp
// Ejemplo de uso
#include "Security/Analyzer/SqlInjectionAnalyzer.h"

BlackWidow::Security::Analyzer::SqlInjectionAnalyzer analyzer;

// Detectar vulnerabilidad SQLi
std::map<std::string, std::string> params = {
    {"id", "1"},
    {"user", "admin"}
};
auto result = analyzer.detectVulnerability("http://ejemplo.com/users", params);
if (result.vulnerable) {
    std::cout << "Vulnerabilidad SQLi detectada: " << result.vulnerabilityType << std::endl;
    std::cout << "Base de datos: " << result.databaseType << std::endl;
    
    // Extraer información de la base de datos
    auto dbInfo = analyzer.extractDatabaseInfo("http://ejemplo.com/users", "id", 
                                             BlackWidow::Security::Payloads::DatabaseType::MYSQL);
}
```

### CsrfAnalyzer
Analizador para vulnerabilidades de Cross-Site Request Forgery (CSRF).

```cpp
// Ejemplo de uso
#include "Security/Analyzer/CsrfAnalyzer.h"

BlackWidow::Security::Analyzer::CsrfAnalyzer analyzer;

// Detectar vulnerabilidad CSRF
auto result = analyzer.detectVulnerability("http://ejemplo.com/profile");
if (result.vulnerable) {
    std::cout << "Vulnerabilidad CSRF detectada: " << result.vulnerabilityType << std::endl;
    std::cout << "Nivel de impacto: " << result.impactLevel << std::endl;
    
    // Generar payload para explotar la vulnerabilidad
    std::map<std::string, std::string> params = {
        {"new_email", "hacker@malicioso.com"},
        {"confirm", "yes"}
    };
    std::string payload = analyzer.generateExploitPayload(
        "http://ejemplo.com/change-email",
        BlackWidow::Security::Payloads::CsrfMethod::POST,
        params,
        true // Auto-submit
    );
}
```

## Compilación

El módulo Analyzer se compila como parte del proyecto BlackWidow utilizando CMake:

```bash
mkdir build && cd build
cmake ..
make
```

## Dependencias

- Módulo Payloads de BlackWidow
- Biblioteca estándar de C++

## Notas de Implementación

Los analizadores implementados incluyen:

1. **Detección de vulnerabilidades**: Métodos para identificar si un objetivo es vulnerable.
2. **Análisis de respuestas**: Funciones para analizar respuestas HTTP y detectar signos de vulnerabilidad.
3. **Explotación**: Métodos para aprovechar las vulnerabilidades detectadas.
4. **Generación de informes**: Estructuras de datos para almacenar y presentar los resultados del análisis.

Cada analizador está diseñado para trabajar con los payloads correspondientes del módulo Payloads, proporcionando una solución completa para pruebas de seguridad web.