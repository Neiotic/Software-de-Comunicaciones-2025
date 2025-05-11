#include <iostream>
#include <sys/wait.h>
#include <cstdlib> //para exit()
#include <unistd.h> //para sleep()


int main(){
	
	for(int i=0; i < 10; i++){
        pid_t pid = fork();
		if(pid < 0){
			perror("Imposible clonar!");
			return 1;
		}
		else if (pid == 0){ /*zona exclusiva del hijo (aqui se da el servicio)*/
            for(int n=0; n < 2; n++){
                std::cout << "Soy el hijo número " << i << " y mi pid es: " << getpid() << std::endl;
                std::cout << "Soy el hijo número " << i << " y mi padre es: " << getppid() << std::endl;
            }
            exit(0);
		}
	}
	/*Siempre que se cree un número conocido de hijos,
    lo mejor es hacer ese mismo número de wait(0)
    antes de que el padre acabe, pero sin estropear "la concurrencia" */
	for(int i=0; i < 10; i++){
		wait(0);
	}

	return 0;
}