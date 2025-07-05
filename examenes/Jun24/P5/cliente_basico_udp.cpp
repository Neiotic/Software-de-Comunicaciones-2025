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

int main(int argc, char * argv[]) {
    int sd = socket(PF_INET,SOCK_DGRAM,0);
    if (sd < 0) {
        perror("Error en socket");
        return 1;
    }

    sockaddr_in vinculo;
    vinculo.sin_family = PF_INET;
    vinculo.sin_port = 5050;
    if (std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    bool fin = false;

    while(!fin) {
        std::string cadena;
        std::getline(std::cin,cadena);
        if (cadena == "fin") {
            fin = true;
        } else {
            ssize_t escritos = sendto(sd, cadena.data(), cadena.size(), 0, (sockaddr *)&vinculo, sizeof(vinculo));
            if (escritos < 0) {
                perror ("Error en envío");
                fin = true;
            } else {
                std::array<char,2048> buffer;
                ssize_t leidos = recvfrom(sd, buffer.data(), buffer.size(), 0, 0, 0);
                if (leidos < 0) {
                    perror ("Error en lectura");
                    fin = true;
                } else {
                    std::string_view sv(buffer.data(),leidos);
                    std::cout << "Recibido: " << sv << std::endl;
                }
            }

        }


    }
    close (sd);
    return 0;
}