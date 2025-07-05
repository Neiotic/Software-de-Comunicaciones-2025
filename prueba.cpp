#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <csignal>
#include <sys/time.h>

void manejadora(int num) {
    std::cout << "BOOM\n";
}
void manejadora2(int num) {} 

//Fifos y timers
int main(){
    signal(SIGALRM, manejadora);
    
    itimerval timer;
    timer.it_value = {1,500000};
    timer.it_interval = {0,500000};

    setitimer(ITIMER_REAL, &timer, 0);

    int cont = 0;
    bool fin = false;

    while(!fin) {
        pause();
        if(cont < 10) {
            cont++;
            std::cout << "cont: " << cont << std::endl;
        } else {
            fin = true;
        }
    }
    std::cout << "FIN BUCLE\n";
    fin = false;

    timer.it_value = {0,0};
    setitimer(ITIMER_REAL, &timer, 0);
    std::cout << "Timer detenido\n";

    timer.it_value = {1,500000};
    timer.it_interval = {0,100000};

    setitimer(ITIMER_REAL, &timer, 0);
    signal(SIGALRM, &manejadora2);
    system("clear");

    while(!fin) {
        pause();
        if(cont < 100) {
            cont++;
            std::cout << cont << " ";
        } else {
            fin = true;
        }
    }
    return 0;
}