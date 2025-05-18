# CMakeLists.txt para el directorio UI

# Crear biblioteca para los componentes de interfaz de usuario
add_library(UI
    # Aquí se añadirán archivos fuente a medida que se implementen
)

# Subdirectorios
add_subdirectory(Widgets)
add_subdirectory(HackConsole)
add_subdirectory(Themes)
add_subdirectory(DevTools)

# Configuración de la biblioteca
target_include_directories(UI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(UI
    # Dependencias externas e internas
    Core
    Utils
)
