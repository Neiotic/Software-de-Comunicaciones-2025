#include <iostream>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(){

    int sd  = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror ("Error en socket");
        return 1;
    }
    sockaddr_in vinculo;
    vinculo.sin_family = PF_INET;
    vinculo.sin_port = 5050;
    if(std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    vinculo.sin_addr.s_addr = inet_addr("127.0.01");
    //vinculo creado
    bool fin = false;
    while(!fin) {
        std::string mensaje;
        std::getline(std::cin,mensaje);

        ssize_t enviados = sendto(sd, mensaje.data(), mensaje.size(), 0, (sockaddr *)&vinculo, sizeof(vinculo));
        if (enviados < 0) {
            perror ("Error en envio");
            fin = true;
        } else {
            std::array<char,2048> buffer;
            ssize_t recibidos = recvfrom(sd, buffer.data(), buffer.size(), 0, 0, 0);
            if (recibidos < 0) {
                perror("Error en recepcion");
                fin = true;
            } else {
                std::string_view sv(buffer.data(),recibidos);
                std::cout << "Recibido: " << sv << std::endl;
            }
        }
    }
   close(sd);
    return 0;
}