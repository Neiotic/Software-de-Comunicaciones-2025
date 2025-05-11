#include <string_view>
#include <iostream>
#include <csignal>
#include <sys/time.h>
#include <unistd.h>

void manejadora_timeout(int n_sig){
    signal(SIGINT, SIG_DFL);
}

void manejadora_interrupcion(int n_sig){
    std::string_view sv = "jaja, no me cierras!";
    write(1, sv.data(), sv.size());
}

int main(int argc, char * argv[]){
    //zona de signals (siempre al principio del todo)
    signal(SIGINT,manejadora_interrupcion);
    signal(SIGALRM,manejadora_timeout); 

    if(argc < 2){
        std::cout << "Introduzca un valor de tiempo\n";
        return 1;
    }

    int tiempo = std::stoi(argv[1]);
    alarm(tiempo); //activo la cuenta atrás
  
  //bucle infinito para probar la llegada de señales
  while(1){
    pause(); //eco-friendly, my friends!
  } 
  
  return 0;
}
