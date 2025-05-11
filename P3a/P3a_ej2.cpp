#include <iostream>
#include <string>

int main(int argc, char * argv[]){

    std::string comandos = "";
    std::string espacio = " ; ";
    int size;

    for(int i = 1; i < argc; i++){
        if(i == argc - 1){//es la ultima entrada
            comandos.append(argv[i]);
        }else{
                comandos.append(argv[i]);
                comandos.append(espacio);
        }
    }
    system(comandos.data());
    return 0;
}