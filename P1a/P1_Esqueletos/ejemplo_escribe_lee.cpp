#include <iostream>
#include <fstream>
#include <array>

/* Este programa crea un fichero binario, escribe dos bytes en él, y luego lo abre para lectura
y recupera esa información */

int main(){

    std::ofstream out;
    out.open("prueba.bin", std::ios::binary);
    if(out.fail()){
        std::cout << "Imposible abrir/crear el fichero para escritura\n";
        return 1;
    }
    std::array<char,2> a_escribir = { 'x', 'y' };

    out.write(a_escribir.data(), 2);
    if(out.fail()){
        std::cout << "error en escritura\n";
    }

    out.close();

    std::array<char, 2048> a_leer;

    //LECTURA, MÉTODO 1: NO SÉ CUANTOS BYTES TENGO QUE LEER
    std::ifstream in("prueba.bin", std::ios::binary);
    if(in.fail()){
        std::cout << "Imposible abrir el fichero para lectura\n";
        return 1;
    }
    //UTILIZO readsome(). OJO al segundo argumento y a la condicion de error
    ssize_t leidos = in.readsome(a_leer.data(),2048);
    if(leidos < 0){
        std::cout << "error en lectura con readsome()\n";
    }

    in.close();

    //LECTURA, MÉTODO 2: SÉ EXACTAMENTE CUANTOS BYTES TENGO QUE LEER
    in.open("prueba.bin", std::ios::binary);
    if(in.fail()){
        std::cout << "Imposible abrir el fichero para lectura\n";
        return 1;
    }
    //UTILIZO read(). OJO al segundo argumento y a la condición de error
    in.read(a_leer.data(),2);
    if(in.fail()){
        std::cout << "error en lectura con read()\n";
    }
    
    in.close();

    return 0;
}