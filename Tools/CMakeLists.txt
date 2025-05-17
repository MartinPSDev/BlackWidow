# CMakeLists.txt para las herramientas del navegador

# Crear la biblioteca de herramientas
add_library(blackwidow_tools STATIC)

# Incluir subdirectorios
add_subdirectory(BurpLike)
# add_subdirectory(NmapLike)
# add_subdirectory(NucleiLike)

# Configuración de la biblioteca
target_include_directories(blackwidow_tools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(blackwidow_tools
    # Dependencias externas e internas
    Core
    Security
    Utils
)