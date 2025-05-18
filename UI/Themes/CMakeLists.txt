# CMakeLists.txt para el directorio UI/Themes

# Obtener todos los archivos fuente en este directorio
file(GLOB THEMES_SOURCES "*.cpp")
file(GLOB THEMES_HEADERS "*.h")
file(GLOB THEMES_CSS "*.css")

# Añadir los archivos a la biblioteca UI
target_sources(UI
    PRIVATE
        ${THEMES_SOURCES}
        ${THEMES_HEADERS}
)

# Instalar archivos CSS de temas en el directorio de destino
install(FILES ${THEMES_CSS}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/blackwidow/themes
)

# Exportar variables para el directorio principal
set(THEMES_SOURCES ${THEMES_SOURCES} PARENT_SCOPE)
set(THEMES_HEADERS ${THEMES_HEADERS} PARENT_SCOPE)