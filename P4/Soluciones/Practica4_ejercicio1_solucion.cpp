#include <iostream>
#include <array>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>


ssize_t read_for(int fd, void * datos, size_t num_bytes, struct timeval timeout) {
    fd_set conjunto;
    FD_ZERO(&conjunto);
    FD_SET(fd, &conjunto);
      
    int result;
    do { 
        result = select(fd + 1, &conjunto, 0, 0, &timeout);
    } while ((result < 0) && (errno == EINTR));
    if (result == -1) {
        // Hubo un error
        return -1;
    } else if (result == 0) {
        // Ha pasado el tiempo
        errno = ETIMEDOUT; 
        return 0;
    } else {
        //Hay datos! A leer tocan
        return read(fd, datos, num_bytes);
    }
}


int main(){
    //prueba cutre 
    std::array<int, 2> fds_pipe;

    int result = pipe(fds_pipe.data());
    if (result < 0){
        perror("ruina en pipe()");
        return 1;
    }

    std::array<uint8_t, 2048> almacen;
    //si leo de la tubería, como no tiene datos, el proceso se bloquea para siempre
    //pero con un read_for... en este caso con un timeval de {3, 0}... se desbloquea a los tres segundos
    timeval timeout = {3,0};
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