#include <iostream>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string>

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

int main() {
    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in dir_servidor = {};
    dir_servidor.sin_family = AF_INET;
    dir_servidor.sin_port = 52345;
    if(std::endian::native == std::endian::little) {
        dir_servidor.sin_port = std::byteswap(dir_servidor.sin_port);
    }
    dir_servidor.sin_addr.s_addr = inet_addr("79.112.36.225");

    int resultado = connect(sd, (sockaddr *)&dir_servidor, sizeof(dir_servidor));
    if (resultado < 0) {
        perror ("error connect");
        return 1;
    }
    //Conexión realizada
    bool fin = false;
    while(!fin) {
        std::array<char,2048> buffer;
        std::string mensaje;
        std::string_view entrada = "Respuesta del servidor: ";

        getline(std::cin,mensaje);
        if (mensaje == "fin") {
            fin = true;
            std::string_view fin = "FINAL_MESSAGE\n";
            ssize_t escrito_final = writen(sd, fin.data(), fin.size());
            if (escrito_final < 0) {
                perror("Error en write(cierre)");
                return 1;
            }
        } else {
            mensaje += "\n";
            ssize_t escritos = writen(sd, mensaje.data(), mensaje.size());
            if (escritos < 0)
            {
                perror("Error en write");
                return 1;
            } else {
                ssize_t leidos = read(sd,buffer.data(),buffer.size());
                if (leidos < 0) {
                    perror("Error en read");
                    return 1;
                } else if (leidos == 0) {
                    std::cout << "Servidor Desconectado!!!!\n";
                } else {
                    std::cout << entrada;
                    std::string_view sv(buffer.data(),leidos);
                    std::cout << sv << std::endl;
                }
            }
        }
    }
    close(sd);
    return 0;
}