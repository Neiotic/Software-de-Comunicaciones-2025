#include <iostream>
#include <fstream>
#include <array>

int main(int argc, char * argv[]){
    if(argc != 3){
        std::cout << "Uso del programa: " << argv[0] << " fich_origen fich_destino\n";
        return 1; 
    }

    std::ifstream in(argv[1], std::ios::binary);
    if(in.fail()){
        std::cout << "Imposible abrir el fichero " << argv[1] << " para lectura\n";
        return 1;
    }
    std::ofstream out(argv[2], std::ios::binary);
    if(out.fail()){
        std::cout << "Imposible abrir/crear el fichero " << argv[2] << " para escritura\n";
        return 1;
    }

    std::array<char, 2048> buffer;
    ssize_t leidos;
    do{
        leidos = in.readsome(buffer.data(),2048);
        if(in.fail()){
            std::cout << "error en lectura\n";
        }
        out.write(buffer.data(), leidos);
        if(out.fail()){
            std::cout << "error en escritura\n";
        }
    }while(leidos==2048);

    in.close();
    out.close();

    return 0;
}