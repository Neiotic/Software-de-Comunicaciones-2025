#include <iostream>
#include <csignal>
#include <sys/time.h>
#include <unistd.h>

void escribepunto(int numsig){
    write(1, ".", 1);
}
void interrupcion(int numsig){
    itimerval temporizador0;
    temporizador0.it_value = {0,0};
    temporizador0.it_interval = {0,0};
    setitimer(ITIMER_VIRTUAL, &temporizador0, 0);
    std::cout << "REESTABLECIDO POR DEFECTO" << std::endl;
    signal(SIGINT, SIG_DFL);
}

int main(int argc, char *argv[]){
    signal(SIGVTALRM,escribepunto);
    signal(SIGINT,interrupcion);

    itimerval temporizador;
    temporizador.it_value = {0, 500000};    // {segundos, microsegundos}
    temporizador.it_interval = {0, 500000}; // {segundos, microsegundos}
    setitimer(ITIMER_VIRTUAL, &temporizador, 0);
    while(1){
        
    };
    return 0;
}