#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
 
void manejadora(int s){
    //no hago nada: mi misión es evitar que el proceso muera (bastante es)
} 

int main(){
    signal(SIGUSR1,manejadora);

    pid_t pidHijo1 = fork();
    if(pidHijo1 < 0){
        perror("Error fork() 1");
        exit(1);
    }else if(pidHijo1 > 0){//zona exclusiva padre
        pid_t pidHijo2 = fork();
        if(pidHijo2 < 0){
            perror("Error fork() 2");
            exit(1);
        }else if(pidHijo2 > 0){//zona exclusiva padre
            pause(); //espera la señal del hijo 2
            std::cout << "Padre: recibida señal\n";
            wait(0);//Espero a que acabe un hijo
            std::cout << "Padre: uno de mis hijos ha acabado\n";
            std::cout << "Padre: envio la señal a mi Hijo 1\n";
            kill(pidHijo1,SIGUSR1);
            wait(0);//Espero a que acabe otro hijo
            std::cout << "Padre: otro de mis hijos ha acabado. FIN\n";
       
        }else{// zona exclusiva del Hijo 2
            std::cout << "Hijo 2: envío la señal a mi padre\n";
            kill(getppid(),SIGUSR1); /*ver nota al final*/
            std::cout << "Hijo 2: FIN\n";
            exit(0);
        }
    }else{//zona exclusiva del Hijo 1	
        std::cout << "Hijo 1: espero la señal de mi padre\n";
        pause(); //espera la señal del padre
        std::cout << "Hijo 1: recibida señal. FIN\n";
        exit(0);
    }
    return 0;
}

