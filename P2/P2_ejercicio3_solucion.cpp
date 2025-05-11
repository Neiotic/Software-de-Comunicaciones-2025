#include <csignal>
#include <sys/time.h>
#include <unistd.h>

void manejadora_timeout(int n_sig){
  write(1,".",1);
}

void manejadora_interrupcion(int n_sig){
    itimerval cero_timer;
    cero_timer.it_value = {0, 0};
    cero_timer.it_interval = {0, 0};
    setitimer(ITIMER_VIRTUAL,&cero_timer, 0); //desactivo el temporizador
    signal(n_sig, SIG_DFL); //restauro su comportamiento por defecto para la siguiente llegada
}

int main(){
  //zona de signals (siempre al principio del todo)
  signal(SIGINT,manejadora_interrupcion);
  signal(SIGVTALRM,manejadora_timeout); //la cuenta atrás de ITIMER_VIRTUAL lanzará la señal SIGVTALRM! 

  //preparo los valores del temporizador para llamar a setitimer()
  itimerval temporizador;
  temporizador.it_value = {3, 300000}; // {segundos, microsegundos}
  temporizador.it_interval = {3, 300000}; // {segundos, microsegundos}
  //esta vez no mido tiempo real, sino tiempo del proceso en la CPU (ITIMER_VIRTUAL)
  setitimer(ITIMER_VIRTUAL, &temporizador, 0);
  
  //bucle infinito para probar la llegada de señales
  while(1){} //no utilizo pause() porque quiero contar el tiempo que está el proceso en la CPU
  
  return 0;
}
