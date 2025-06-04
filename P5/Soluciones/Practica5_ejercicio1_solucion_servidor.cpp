#include <iostream>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

ssize_t writen(int fd, const void *data, size_t N){
    size_t por_escribir = N;
    size_t total_escritos = 0;
    char * p = (char *)data;
    ssize_t escritos;
    do{
        escritos = write(fd, p + total_escritos, por_escribir);
        if(escritos > 0){
            total_escritos +=escritos;
            por_escribir -=escritos;
        }
    }while((escritos > 0) && (total_escritos < N));
    if(escritos < 0){
        return -1;
    }else{
        return total_escritos;
    }
}

int main(){

    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 6000;
    if(std::endian::native == std::endian::little){
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }//siempre asegúrate de que está en big endian!

    int resultado = bind(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    if(resultado < 0){
        perror("ruina bind");
        return 1;
    }

    resultado = listen(sd,10);
    if(resultado < 0){
        perror("ruina listen");
        return 1;
    }

    sockaddr_in dir_cliente;
    socklen_t longitud_dir = sizeof(dir_cliente);
    while(1){  
        int csd = accept(sd, (sockaddr *)&dir_cliente, &longitud_dir);    
        if(csd < 0) {      
            perror("error en accept");      
            return 1;    
        }
        //listo para el servicio de eco
        ssize_t leidos;
        std::array<char,2048> almacen;
        do{
            leidos = read(csd,almacen.data(),almacen.size());
            if(leidos > 0){
                ssize_t escritos = writen(csd, almacen.data(), leidos);
                if(escritos < 0){
                    perror("error en write");
                    break; //en esta ocasión, decido salir del bucle de servicio, no del programa
                }
            }
        }while(leidos > 0);     
        close(csd);
    }
    //aqui no llega nunca
    close(sd);

    return 0;
}