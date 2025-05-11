#include <iostream>
#include <string>
#include <cstdlib> //para system()

int main(int argc, char *argv[]){

    if(argc < 2){
        std::cout << "Introduzca comandos para system\n";
        return 1;
    }

    std::string comandos;
    for(int i = 1; i < argc; i++){
        comandos += argv[i];
        comandos += ";";
    } 

    system(comandos.data());

    return 0;
}