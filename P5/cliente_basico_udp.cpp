#include <iostream>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

int main() {
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("ERROR en socket");
        return 1;
    }
    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 6061;
    if(std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Conexión realizada
    std::array<char,2048> buffer;
    std::string mensaje;
    bool fin = false;
    while(!fin) {
        std::getline(std::cin,mensaje);
        if (mensaje == "fin") {
            fin = true;
        } else {
            ssize_t escritos = sendto(sd,mensaje.data(),mensaje.size(),0,(sockaddr *)&vinculo,(socklen_t)sizeof(vinculo));
            if (escritos < (ssize_t)mensaje.size()) {
                perror ("ERROR en envío");
                fin = true;
            } else {
                ssize_t leidos = recvfrom(sd,buffer.data(),buffer.size(),0,0,0);
                if(leidos < 0){
                    perror("fallo en la lectura");
                    fin = true;
                } else {
                    std::string_view sv(buffer.data(),leidos);
                    std::cout << "Recibido: " << sv << std::endl;
                }
            }
        }
    }
    close(sd);
    return 0;
}