#include <iostream>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(){

    int sd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 7000;
    if(std::endian::native == std::endian::little){
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }//siempre asegúrate de que está en big endian!

    int resultado = bind(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    if(resultado < 0){
        perror("ruina bind");
        return 1;
    }

    sockaddr_in dir_cliente;
    socklen_t longitud_dir = sizeof(dir_cliente);
    while(1){  
        int16_t numero;
        ssize_t leidos = recvfrom(sd, &numero, 2, 0, (sockaddr *)&dir_cliente, &longitud_dir);    
        if(leidos < 0) {      
            perror("error en la recepcion");      
            return 1;    
        }
        //convierto desde big endian:
        if(std::endian::native == std::endian::little){
            numero = std::byteswap(numero);
        }

        int32_t resultado = numero;
        resultado += 1;
        //antes de mandar el resultado, lo paso a big endian:
        if(std::endian::native == std::endian::little){
            resultado = std::byteswap(resultado);
        }

        ssize_t escritos = sendto(sd, &resultado, 4, 0, (sockaddr *)&dir_cliente, sizeof(dir_cliente));
        if(escritos < 0){
            perror("error en sendto");
            return 1;
        }
    }
    //aqui no llega nunca
    close(sd);

    return 0;
}