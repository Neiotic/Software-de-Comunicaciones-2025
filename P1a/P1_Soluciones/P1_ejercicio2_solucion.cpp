#include <iostream>
#include <fstream>
#include <array>
#include <bit>

int main(){

    std::ofstream out;
    out.open("prueba2.bin", std::ios::binary);
    if(out.fail()){
        std::cout << "Imposible abrir/crear el fichero para escritura\n";
        return 1;
    }
    //cabecera del fichero con el formato nativo de representacion de bytes
    //en memoria del escritor
    uint16_t magic_number = 0xAABB;
    out.write((char *)&magic_number, 2);
    if(out.fail()){
        std::cout << "error en escritura\n";
    }
    //ahora se escriben los datos
    std::array<uint16_t,4> almacen = { 1, 2, 3, 4};

    out.write((char *)almacen.data(), 4 * sizeof(uint16_t)); //ojo, se escriben 8 bytes
    if(out.fail()){
        std::cout << "error en escritura\n";
    }

    out.close();

    
    std::ifstream in("prueba2.bin", std::ios::binary);
    if(in.fail()){
        std::cout << "Imposible abrir el fichero para lectura\n";
        return 1;
    }
    //leo la cabecera con el magic number
    uint16_t magic_leido;
    //utilizo read porque SÉ LOS BYTES QUE TENGO QUE LEER
    in.read((char *)&magic_leido,2);
    if(in.fail()){
        std::cout << "error en lectura del magic number()\n";
    }

    bool necesita_swap;
    if(magic_leido == 0xAABB){ //escritor y lector comparten la misma arquitectura
        necesita_swap = false;
    }else if(magic_leido == 0xBBAA){ //tienen diferentes arquitecturas
        necesita_swap = true;
    }else{
        std::cout <<"Formato de fichero inválido\n";
        return 1; //salida del programa
    }

    uint16_t valor;
    ssize_t leidos;
    bool fin = false;
    while(!fin){
        //utilizo readsome() para tener un ejemplo de cómo se hace con este método
        leidos = in.readsome((char *)&valor,2);
        if(leidos == 2){ //ha leido un nuevo valor correctamente
            if(necesita_swap){
                valor = std::byteswap(valor); //le doy la vuelta antes de utilizarlo!
            }
            std::cout << "Leido del fichero: " << valor << std::endl;
        }else if(leidos ==0){ //ha llegado al final del fichero, salimos del bucle
            fin = true; //break;
        }else{ //error, también salimos del bucle
            std::cout << "error en lectura de un uint16_t\n";
            fin = true;
        }
    }
    in.close();

    return 0;
}