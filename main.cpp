/**
 * BlackWidow Browser - Navegador orientado al bugbounty y hacking ético
 * 
 * Archivo principal que inicializa la aplicación del navegador
 */

#include <iostream>
#include <memory>
#include <string>

// Inclusiones de componentes principales
#include "Browser/Browser.h"
#include "UI/Widgets/MainWindow.h"
#include "Security/Interceptor/ProxyInterceptor.h"
#include "Utils/Config/ConfigManager.h"
#include "Utils/Logging/Logger.h"

int main(int argc, char* argv[]) {
    // Inicializar sistema de logs
    Utils::Logging::Logger::initialize();
    Utils::Logging::Logger::info("Iniciando BlackWidow Browser");
    
    try {
        // Cargar configuración
        auto configManager = std::make_shared<Utils::Config::ConfigManager>();
        configManager->loadConfig();
        
        // Inicializar componentes de seguridad
        auto interceptor = std::make_shared<Security::Interceptor::ProxyInterceptor>();
        interceptor->initialize();
        
        // Inicializar el navegador
        auto browser = std::make_shared<Core::Browser::Browser>();
        browser->setInterceptor(interceptor);
        browser->initialize();
        
        // Inicializar la interfaz de usuario
        auto mainWindow = std::make_shared<UI::Widgets::MainWindow>(browser);
        mainWindow->show();
        
        // Ejecutar el bucle principal de la aplicación
        return mainWindow->run();
    } 
    catch (const std::exception& e) {
        Utils::Logging::Logger::error("Error fatal: " + std::string(e.what()));
        std::cerr << "Error fatal: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}