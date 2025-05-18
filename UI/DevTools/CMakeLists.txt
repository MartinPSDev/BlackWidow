# CMakeLists.txt para el directorio UI/DevTools

# Obtener todos los archivos fuente en este directorio
file(GLOB DEVTOOLS_SOURCES "*.cpp")
file(GLOB DEVTOOLS_HEADERS "*.h")

# Añadir los archivos a la biblioteca UI
target_sources(UI
    PRIVATE
        ${DEVTOOLS_SOURCES}
        ${DEVTOOLS_HEADERS}
)

# Exportar variables para el directorio principal
set(DEVTOOLS_SOURCES ${DEVTOOLS_SOURCES} PARENT_SCOPE)
set(DEVTOOLS_HEADERS ${DEVTOOLS_HEADERS} PARENT_SCOPE)