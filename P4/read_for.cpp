#include <iostream>
#include <array>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <csignal>

/* lectura de bytes (con tiempo de espera) 
    Argumentos:
        int fd: 
        void * datos: dirección de memoria donde colocar los bytes que se reciban
        size_t num_bytes: máximo numero de bytes que caben en la memoria (o que se quieren recibir)
        timeval timeout: indica los segundos-microsegundos de espera sin leer datos antes de retornar
        Retorna:
        ssize_t: (es un size_t con signo), porque puede devolver:
            -1: si hubo un error en la recepción
             0: si el extremo opuesto está cerrado o no hay datos y ha expirado el tiempo (con errno==ETIMEDOUT)
            >0: son los bytes que realmente se han recibido por el canal 
*/

ssize_t read_for(int fd, void * datos, size_t num_bytes, struct timeval timeout) {
    fd_set conjunto;
    int max_fd = 0;
    FD_ZERO(&conjunto);
    /*POR HACER: introduce el descriptor en el conjunto */
    FD_SET(fd, &conjunto);
    max_fd = std::max(fd, max_fd);
    int result;
    
    /*POR HACER: ponle los argumentos al select()*/
    do{
        result = select(max_fd + 1, &conjunto, 0, 0, &timeout);
    }while((result < 0) && (errno == EINTR));
    if (result == -1) {
        // Hubo un error
        return -1;
    }else if (result == 0) {
        // Ha pasado el tiempo
        errno = ETIMEDOUT; 
        return 0;
    } else{
        //Hay datos! A leer tocan
        /*POR HACER: completa los argumentos del read()*/
        return read(fd,datos,num_bytes);
    }
}

void manejadora (int num_sig){};

int main(){
    signal(SIGINT,manejadora);
    //prueba cutre 
    std::array<int, 2> fds_pipe;

    int result = pipe(fds_pipe.data());
    if(result < 0){
        perror("ruina en pipe()");
        return 1;
    }

    std::array<uint8_t, 2048> almacen;
    //si leo de la tubería, como no tiene datos, el proceso se bloquea para siempre
    //pero con un read_for... en este caso con un timeval de {3, 0}... se desbloquea a los tres segundos
    timeval timeout = {30,0};
    ssize_t leidos = read_for(fds_pipe[0], almacen.data(), almacen.size(), timeout);

    if(leidos < 0){
        perror("ruina en read_for()");
        return 1;
    }

    if((leidos == 0) && (errno == ETIMEDOUT)){
        std::cout << "No se han recibido datos durante el tiempo indicado\n";
    }

    return 0;
}