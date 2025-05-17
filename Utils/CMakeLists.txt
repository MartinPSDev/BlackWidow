# CMakeLists.txt para el directorio Utils

# Crear biblioteca para los componentes de utilidades
add_library(Utils
    Logging/Logger.cpp
    # Aquí se añadirán más archivos fuente a medida que se implementen
)

# Subdirectorios
add_subdirectory(Logging)
add_subdirectory(Config)

# Configuración de la biblioteca
target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Utils
    # Dependencias externas e internas
)