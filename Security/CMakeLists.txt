# CMakeLists.txt para el directorio Security

# Crear biblioteca para los componentes de seguridad
add_library(Security
    Interceptor/ProxyInterceptor.cpp
    # Aquí se añadirán más archivos fuente a medida que se implementen
)

# Subdirectorios
add_subdirectory(Interceptor)
add_subdirectory(Scanners)
add_subdirectory(Payloads)
add_subdirectory(Analyzer)
add_subdirectory(Reports)

# Configuración de la biblioteca
target_include_directories(Security PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Security
    # Dependencias externas e internas
    Utils
)