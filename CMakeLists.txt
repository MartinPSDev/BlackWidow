cmake_minimum_required(VERSION 3.16)
project(BlackWidow VERSION 0.1.0 LANGUAGES CXX)

# Configuración del estándar C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de compilación
option(BUILD_TESTS "Compilar tests" OFF)
option(ENABLE_SECURITY_FEATURES "Habilitar características de seguridad" ON)

# Directorios de inclusión
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Security
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Tools
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils
)

# Subdirectorios del proyecto
add_subdirectory(Core)
add_subdirectory(Security)
add_subdirectory(UI)
add_subdirectory(Tools)
add_subdirectory(Utils)

# Ejecutable principal
add_executable(BlackWidow main.cpp)

# Enlazar con las bibliotecas del proyecto
target_link_libraries(BlackWidow
    Core
    Security
    UI
    Tools
    Utils
)

# Configuración de instalación
install(TARGETS BlackWidow DESTINATION bin)

# Configuración de tests si están habilitados
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()

# Mensaje de configuración
message(STATUS "Configurando BlackWidow Browser v${PROJECT_VERSION}")
message(STATUS "Características de seguridad: ${ENABLE_SECURITY_FEATURES}")