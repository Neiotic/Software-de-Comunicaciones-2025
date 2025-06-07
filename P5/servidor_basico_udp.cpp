#include <iostream>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(){

    int sd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("ruina socket udp");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 6061;
    if(std::endian::native == std::endian::little){
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }//siempre asegúrate de que está en big endian!

    int resultado = bind(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    if(resultado < 0){
        perror("ruina bind");
        return 1;
    }
    while(1) {
        std::array<char, 2048> mensaje;
        sockaddr_in dir_cliente;
        socklen_t longitud_dir = sizeof(dir_cliente);  
        ssize_t leidos = recvfrom(sd, mensaje.data(), mensaje.size(), 0, (sockaddr *)&dir_cliente, &longitud_dir);    
        if(leidos < 0) {      
            perror("error en recvfrom");      
            return 1;    
        }
        ssize_t escritos = sendto(sd,mensaje.data(),leidos,0,(sockaddr *)&dir_cliente,sizeof(dir_cliente));
        std::string_view sv(mensaje.data(),escritos);
        std::cout << "Enviado de vuelta: " << sv << std::endl;
        if(escritos < 0) {
            perror ("ERROR en sendto");
            return 1;
        }
    }


    //si vas a responder, utiliza sendto(). Tienes la dirección del cliente en dir_cliente
    
    close(sd);

    return 0;
}