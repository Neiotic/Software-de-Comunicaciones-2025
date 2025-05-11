#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    for (int i = 0; i < 10; i++){
        pid_t pid1 = fork();

        if (pid1 < 0){
            perror("Falla Fork\n");
            exit(1);
        }
        if (pid1 == 0){
            /*zona exclusiva del hijo (aquí se da el servicio)*/
            for (int n = 0; n < 2; n++){
                std::cout << "Soy el hijo número " << i << " y mi pid es: "
                          << getpid() << std::endl;
                std::cout << "Soy el hijo número " << i << " y mi padre es: "
                          << getppid() << std::endl;
            }
            exit(0);
        }else{
            wait(0);
        }
    }

    return 0;
}