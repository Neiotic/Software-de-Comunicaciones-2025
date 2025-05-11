#include <iostream>
#include <sched.h>
#include <signal.h>


int main(){
    signal(SIGINT, SIG_IGN); //es inmune a Ctrl-C (la ignora)
    sleep(5); //el proceso duerme y se despierta tras cinco segundos
    signal(SIGINT, SIG_DFL); //vuelve poder recibir Ctrl-C
    while(1){
    pause(); //no hace falta chequear su valor de retorno
    }
    }
    