#include <iostream>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(){

    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 5050;
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
    int csd = accept(sd, (sockaddr *)&dir_cliente, &longitud_dir);    
    if(csd < 0) {      
        perror("error en accept");      
        return 1;    
    }

    //listo para dar servicio con writen/readn/read por el descriptor csd
    
    close(csd);

    close(sd);

    return 0;
}