# BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
# Componente WebAssembly

# Definir los archivos fuente del componente WebAssembly
set(WASM_SOURCES
    WasmEngine.cpp
    WasmParser.cpp
    WasmIntegration.cpp
)

# Definir los archivos de cabecera del componente WebAssembly
set(WASM_HEADERS
    WasmEngine.h
    WasmParser.h
    WasmIntegration.h
)

# Crear la biblioteca del componente WebAssembly
add_library(BlackWidowWebAssembly STATIC ${WASM_SOURCES} ${WASM_HEADERS})

# Configurar las propiedades de la biblioteca
set_target_properties(BlackWidowWebAssembly PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Incluir directorios necesarios
target_include_directories(BlackWidowWebAssembly PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Agregar la biblioteca a la lista de componentes del núcleo
set(CORE_COMPONENTS ${CORE_COMPONENTS} BlackWidowWebAssembly PARENT_SCOPE)