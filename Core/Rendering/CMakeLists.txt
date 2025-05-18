# CMakeLists.txt para el motor de renderizado

set(RENDERING_SOURCES
    RenderingEngine.cpp
)

set(RENDERING_HEADERS
    RenderingEngine.h
)

# Agregar la biblioteca del motor de renderizado
add_library(BlackWidowRendering STATIC ${RENDERING_SOURCES} ${RENDERING_HEADERS})

# Incluir directorios necesarios
target_include_directories(BlackWidowRendering PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Vincular con otras bibliotecas del proyecto
target_link_libraries(BlackWidowRendering
    BlackWidowDOM
    BlackWidowHTML
    BlackWidowCSS
    BlackWidowWebAssembly
)

# Instalar la biblioteca
install(TARGETS BlackWidowRendering
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Instalar los archivos de cabecera
install(FILES ${RENDERING_HEADERS}
    DESTINATION include/BlackWidow/Core/Rendering
)