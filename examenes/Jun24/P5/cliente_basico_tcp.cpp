#include <iostream>
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

int main(int argc, char * argv[]){
    int sd = socket(AF_INET,SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Error en socket");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 6000;
    if (std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    int resultado = connect(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    if (resultado < 0) {
        perror("Error en connect");
        return 1;
    }
    //Ya estan conectados
    bool fin = false;
    while (!fin)
    {
        std::string mensaje;
        std::getline(std::cin,mensaje);
        if (mensaje == "fin") {
            fin = true;
        } else {
            ssize_t escritos = writen(sd,mensaje.data(),mensaje.size());
            if (escritos < 0) {
                perror("error en escritura");
                return 1;
            } else {

                std::array<char,2048> buffer;
                ssize_t leidos = read(sd, buffer.data(), buffer.size());
                if (leidos < 0) {
                    perror("Error en lectura");
                    return 1;
                } else if (leidos == 0) {
                    std::cout << "Servidor cierra conexion!\n";
                    fin = true;
                } else {
                    std::string_view sv(buffer.data(),leidos);
                    std::cout << sv << std::endl;
                }
            }
        }

    }
    close(sd);


    return 0;
}