# CMakeLists.txt para las herramientas BurpLike (Repeater e Intruder)

# Definir los archivos fuente para las herramientas BurpLike
set(BURPLIKE_SOURCES
    Repeater.cpp
    Intruder.cpp
)

# Definir los archivos de cabecera para las herramientas BurpLike
set(BURPLIKE_HEADERS
    Repeater.h
    Intruder.h
)

# Añadir los archivos a la biblioteca
target_sources(blackwidow_tools PRIVATE ${BURPLIKE_SOURCES} ${BURPLIKE_HEADERS})

# Incluir directorios necesarios
target_include_directories(blackwidow_tools PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})