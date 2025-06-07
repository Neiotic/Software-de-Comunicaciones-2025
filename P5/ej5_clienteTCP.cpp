#include <iostream>
#include <array>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

int main(int argc, char *argv[]) {
    ssize_t sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror ("ERROR en socket");
        return 1;
    }
    
    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 7000;
    if(std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    ssize_t resultado = connect(sd,(sockaddr *)&vinculo,sizeof(vinculo));
    if (resultado < 0) {
        perror("Error en connect");
        return 1;
    }
    //Conexión realizada
    
    int16_t numero = std::stoi(argv[1]);
    if(std::endian::native == std::endian::little) {
        numero = std::byteswap(numero);
    }
    ssize_t escritos = writen(sd,&numero,2);
    if (escritos < 2) {
        perror("Error no se puede escribir en el canal");
        return 1;
    } else {
        int32_t resultado;
        ssize_t leidos = read(sd,&resultado,4);
        if(std::endian::native == std::endian::little) {
            resultado = std::byteswap(resultado);
        }
        if (leidos < 4) {
            perror ("Error en recepción");
            return 1;
        } else {
            std::cout << "El resultado es " << resultado << std::endl;
        }
    }
    close(sd);
    return 0;
}