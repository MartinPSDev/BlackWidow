# CMakeLists.txt para el directorio Core

# Crear biblioteca para los componentes del núcleo
add_library(Core
    Browser/Browser.cpp
    # Aquí se añadirán más archivos fuente a medida que se implementen
)

# Subdirectorios
add_subdirectory(Browser)
add_subdirectory(DOM)
add_subdirectory(HTML)
add_subdirectory(CSS)
add_subdirectory(Rendering)
add_subdirectory(JavaScript)
add_subdirectory(Network)
add_subdirectory(WebAssembly)

# Configuración de la biblioteca
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Core
    # Dependencias externas e internas
)