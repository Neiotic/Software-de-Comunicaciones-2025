#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <vector>
#include <map>

// Estructura para almacenar las partes de una URL
struct URL_parts {
    std::string protocol;
    std::string domain;
    std::string path;
    std::string query;
    std::string fragment;
    std::map<std::string, std::string> query_params;
};

// Estructura para almacenar las partes de una URL usando string_view
struct URL_parts_view {
    std::string_view protocol;
    std::string_view domain;
    std::string_view path;
    std::string_view query;
    std::string_view fragment;
    std::map<std::string_view, std::string_view> query_params;
};

// Función para separar los parámetros de query (versión string)
void parse_query_params(const std::string& query, std::map<std::string, std::string>& params) {
    size_t start = 0;
    size_t end;
    
    while (start < query.length()) {
        // Busca el final del par clave-valor ('&' o final de la cadena)
        end = query.find('&', start);
        if (end == std::string::npos) {
            end = query.length();
        }
        
        // Encontrar el separador = entre clave y valor
        size_t equalPos = query.find('=', start);
        if (equalPos != std::string::npos && equalPos < end) {
            std::string key = query.substr(start, equalPos - start);
            std::string value = query.substr(equalPos + 1, end - equalPos - 1);
            params[key] = value;
        }
        
        // Avanzar al siguiente par
        start = end + 1;
    }
}

// Versión de parser de una URL con std::string
URL_parts parse_URL_string(const std::string& url) {
    URL_parts parts;
    
    // Busca el campo protocolo
    size_t protocol_end = url.find("://");
    if (protocol_end != std::string::npos) {
        parts.protocol = url.substr(0, protocol_end);
        protocol_end += 3; // Salta el "://"
    } else {
        protocol_end = 0;
    }
    
    // Busca el dominio
    size_t domain_end = url.find("/", protocol_end);
    if (domain_end != std::string::npos) {
        parts.domain = url.substr(protocol_end, domain_end - protocol_end);
    } else {
        parts.domain = url.substr(protocol_end);
        return parts; // No hay más componentes
    }
    
    // Busca la query
    size_t path_end = url.find("?", domain_end);
    if (path_end != std::string::npos) {
        parts.path = url.substr(domain_end, path_end - domain_end);
        
        // Busca el fragmento
        size_t query_end = url.find("#", path_end);
        if (query_end != std::string::npos) {
            parts.query = url.substr(path_end + 1, query_end - path_end - 1);
            parts.fragment = url.substr(query_end + 1);
        } else {
            parts.query = url.substr(path_end + 1);
        }
        
        // Procesa los parámetros de query
        parse_query_params(parts.query, parts.query_params);
    } else {
        // Busca el fragmento directamente
        size_t fragment_start = url.find("#", domain_end);
        if (fragment_start != std::string::npos) {
            parts.path = url.substr(domain_end, fragment_start - domain_end);
            parts.fragment = url.substr(fragment_start + 1);
        } else {
            parts.path = url.substr(domain_end);
        }
    }
    
    return parts;
}

// Función para separar los parámetros de query (versión string_view)
void parse_query_params(std::string_view query, std::map<std::string_view, std::string_view> & params) {
    //RELLENA ESTA FUNCION PARA QUE DEVUELVA EN EL MAPA params LOS PARAMETROS DE LA query
}

// Versión de parser de una URL con std::string_view
URL_parts_view parse_URL_string_view(std::string_view url) {
    URL_parts_view parts;

    //RELLENA ESTA FUNCION PARA QUE LOS CAMPOS DE LA ESTRUCTURA parts ESTEN COMPLETOS
    
    return parts;
}

// Función para mostrar las partes de una URL (versión string)
void print_URL_parts(const URL_parts & parts) {
    std::cout << "Protocolo: " << parts.protocol << std::endl;
    std::cout << "Dominio: " << parts.domain << std::endl;
    std::cout << "Ruta: " << parts.path << std::endl;
    std::cout << "Query: " << parts.query << std::endl;
    std::cout << "Fragmento: " << parts.fragment << std::endl;
    
    // Mostrar los parámetros de query separados
    if (!parts.query_params.empty()) {
        std::cout << "\nParámetros de Query:" << std::endl;
        //Notacion moderna para acceder a un par separando sus valores first y second
        for (auto & [key, value] : parts.query_params) {
            std::cout << "  " << key << " = " << value << std::endl;
        }
        //O también de forma clásica:
        /* for (auto & par_key_value : parts.query_params) {
            std::cout << "  " << par_key_value.first << " = " << par_key_value.second << std::endl;
        }*/
    }
}

// Función para mostrar las partes de una URL (versión string_view)
void print_URL_parts_view(const URL_parts_view & parts) {
    std::cout << "Protocolo: " << parts.protocol << std::endl;
    std::cout << "Dominio: " << parts.domain << std::endl;
    std::cout << "Ruta: " << parts.path << std::endl;
    std::cout << "Query: " << parts.query << std::endl;
    std::cout << "Fragmento: " << parts.fragment << std::endl;
    
    // Muestra los parámetros de query separados
    if (!parts.query_params.empty()) {
        std::cout << "\nParámetros de Query:" << std::endl;
        //Notacion moderna para acceder a un par separando sus valores first y second
        for (auto & [key, value] : parts.query_params) {
            std::cout << "  " << key << " = " << value << std::endl;
        }
        //O también de forma clásica:
        /* for (auto & par_key_value : parts.query_params) {
            std::cout << "  " << par_key_value.first << " = " << par_key_value.second << std::endl;
        }*/
        
        
    }
}

// Función para comparar el rendimiento de las dos funciones
void comparacion_benchmark(const std::string& test_url) {
    const int iterations = 100000;
    
    std::vector<std::string> urls;
    urls.reserve(iterations); //esto es para reservar espacio en el heap para todos los elementos de una vez
    
    // Se crean múltiples copias de la URL para simular procesamiento de datos
    for (int i = 0; i < iterations; ++i) {
        urls.push_back(test_url);
    }
    
    // Benchmark para std::string
    auto start_s = std::chrono::high_resolution_clock::now();
    for (const auto& url : urls) {
        auto parts = parse_URL_string(url);
        // Uso la variable para evitar que el compilador optimice y me quite hasta el bucle!
        if (parts.protocol.empty()) std::cout << "Sin protocolo???\n"; 
    }
    auto end_s = std::chrono::high_resolution_clock::now();
    
    // Benchmark para std::string_view
    auto start_sv = std::chrono::high_resolution_clock::now();
    for (const auto& url : urls) {
        auto parts = parse_URL_string_view(url);
        // Uso la variable para evitar que el compilador optimice y me quite hasta el bucle!
        if (parts.protocol.empty()) std::cout << "Sin protocolo???\n";
    }
    auto end_sv = std::chrono::high_resolution_clock::now();
    
    // Calcular y mostrar resultados
    auto duration_s = std::chrono::duration_cast<std::chrono::milliseconds>(end_s - start_s);
    auto duration_sv = std::chrono::duration_cast<std::chrono::milliseconds>(end_sv - start_sv);
    
    std::cout << "Tiempo con std::string: " << duration_s.count() << " ms" << std::endl;
    std::cout << "Tiempo con std::string_view: " << duration_sv.count() << " ms" << std::endl;
    std::cout << "Diferencia: " << duration_s.count() - duration_sv.count() << " ms" << std::endl;
    
    if (duration_s > duration_sv) {
        float porcentaje = (float)(duration_s.count() - duration_sv.count()) / duration_s.count() * 100;
        std::cout << "std::string_view es aproximadamente " << porcentaje << "% más rápido" << std::endl;
    } else {
        float porcentaje = (float)(duration_sv.count() - duration_s.count()) / duration_sv.count() * 100;
        std::cout << "std::string es aproximadamente " << porcentaje << "% más rápido" << std::endl;
    }
}

int main() {
    std::string url = "https://mascotas-programadoras.com/gatos/compiladores?lenguaje=cpp&experiencia=junior#gato-debugging";

    std::cout << "=== Parsing con std::string ===" << std::endl;
    URL_parts parts = parse_URL_string(url);
    print_URL_parts(parts);
    
    std::cout << "\n=== Parsing con std::string_view ===" << std::endl;
    URL_parts_view partsv = parse_URL_string_view(url);
    print_URL_parts_view(partsv);
    
    std::cout << "\n=== Comparación de rendimiento ===" << std::endl;
    //DESCOMENTA LA SIGUIENTE LÍNEA CUANDO TENGAS IMPLEMENTADAS LAS FUNCIONES QUE FALTAN (Y AGÁRRATE A LA SILLA)
    //comparacion_benchmark(url);
    
    return 0;
}