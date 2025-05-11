#include <iostream>
#include <array>

void salir(){
    std::exit(0); //salida del programa
}

// Declaración de funciones para las opciones del menú
void mensaje() {
    std::cout << "Configurado: \"Estoy ausente, no se moleste en contactar\"" << std::endl;
}

void recupera() {
    std::cout << "Mensaje de Johhny Fuego: \"quiero mi dinero AHORA\"" << std::endl;
}

void ayuda() {
    std::cout << "AYUDA:" << std::endl;
    std::cout << "Seleccione un número entre 1 y 3. Pulse 0 para salir." << std::endl;
}

// Función principal
int main() {
    // Punteros a funciones: declaramos un array de punteros a funciones
    // Cada elemento apunta a una función que devuelve void y no tiene argumentos
    std::array funciones_menu = { salir, mensaje, recupera, ayuda };
    
    int opcion;
    
    while(1){
        // Muestra el menú
        std::cout << "\n----- CONTESTADOR AUTOMÁTICO -----" << std::endl;
        std::cout << "1. Activar mensaje automático de respuesta" << std::endl;
        std::cout << "2. Recuperar el último mensaje recibido" << std::endl;
        std::cout << "3. Ayuda" << std::endl;
        std::cout << "0. Salir del programa" << std::endl;
        std::cout << "Introduzca opción: " << std::endl;
        
        // Lee la opcion del teclado
        std::cin >> opcion;
        
        // Filtra opcion
        if (opcion >= 0 && opcion <= 3) {
            // Con el operador paréntesis, ejecuto la función que está en la casilla del array de punteros a funcion
            funciones_menu[opcion]();
        } else {
            std::cout << "Opción no válida. Inténtelo de nuevo." << std::endl;
        }    
    }
    //aquí no llegas
    return 0;
}