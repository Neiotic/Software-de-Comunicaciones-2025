#include <iostream>
#include <string>
#include <sys/wait.h>
#include <string_view>
#include <unistd.h>
#include <array>

void manejadora(int num_sig){};

int main(int argc, char * argv[]){
    signal(SIGUSR1,manejadora);

    pid_t pidHijo1 = fork();
    if(pidHijo1 < 0){
        perror("ERROR en fork 1");
        return 1;
    }
    if(pidHijo1 > 0){ //zona exclusiva del padre
        pid_t pidHijo2 = fork();
        if(pidHijo2 < 0){
            perror("ERROR en fork 2");
            return 1;
        }
        if(pidHijo2 > 0){//sigue siendo zona padre
            pause();
            std::cout << "Padre: Recibida señal!\n";
            wait(0);
            std::cout << "Padre: Uno de mis hijos ha acabado\n";
            std::cout << "Padre: Envio señal a mi Hijo 1...\n";
            kill(pidHijo1,SIGUSR1);
            wait(0);
            std::cout << "Padre: Otro de mis hijos ha acabado. FIN\n";
        }else{//zona hijo2
            std::cout << "Hijo 2: Envio señal a mi padre...\n";
            kill(getppid(),SIGUSR1);
            std::cout << "Hijo 2: FIN\n";
        }
    }else{//zona del hijo 1
        std::cout << "Hijo 1: Espero la señal de mi padre\n";
        pause();
        std::cout << "Hijo 1: Recibida señal. FIN\n";
        exit(0);
    }
    return 0; 
}