#include <iostream>
#include <fstream>
#include <array>

int main(int argc, char * argv[]){

    if (argc != 3){
        std::cout << "ERROR! USO: nombre_ejecutable nombre_fichero_entrada nombre_fichero_salida\n";
        return 1;
    }
    std::ifstream entrada(argv[1], std::ios::binary);
    if (entrada.fail()){
        std::cout << "ERROR: Fallo en la apertura del archivo de entrada\n";
        return 1;
    }
    std::ofstream salida(argv[2], std::ios::binary);
    if (salida.fail()){
        std::cout << "ERROR: Fallo en la apertura/creacion del archivo de salida\n";
        return 1;
    }

    std::array <char,2048> almacen;
    ssize_t leidos;
    do{
        leidos = entrada.readsome(almacen.data(),2048);
        if (entrada.fail())
            std::cout << "ERROR EN LA LECTURA\n";
        salida.write(almacen.data(),2048);
        if (salida.fail())
            std::cout << "ERROR EN LA ESCRITURA\n";
    }while(leidos == 2048);
    
    entrada.close();
    salida.close();

    return 0;
}