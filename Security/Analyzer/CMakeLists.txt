# CMakeLists.txt para el módulo Analyzer

# Definir los archivos fuente para el módulo Analyzer
set(ANALYZER_SOURCES
    XxeAnalyzer.cpp
    XssAnalyzer.cpp
    SqlInjectionAnalyzer.cpp
    CsrfAnalyzer.cpp
)

# Definir los archivos de cabecera para el módulo Analyzer
set(ANALYZER_HEADERS
    XxeAnalyzer.h
    XssAnalyzer.h
    SqlInjectionAnalyzer.h
    CsrfAnalyzer.h
)

# Crear la biblioteca para el módulo Analyzer
add_library(BlackWidowAnalyzer STATIC ${ANALYZER_SOURCES} ${ANALYZER_HEADERS})

# Incluir directorios necesarios
target_include_directories(BlackWidowAnalyzer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Vincular con otras bibliotecas del proyecto
target_link_libraries(BlackWidowAnalyzer PUBLIC BlackWidowPayloads)