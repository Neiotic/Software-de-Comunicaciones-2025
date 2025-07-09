#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <csignal>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <set>

int main(int argc, char * argv[]) {
    bool soylittle = false;
    if (std::endian::native == std::endian::little) {
        soylittle = true;
    }
    ssize_t sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd < 0) {
        perror("error socket");
        return -1;
    }
    sockaddr_in vinculo;
    vinculo.sin_family = PF_INET;
    vinculo.sin_port = 5052;
    if(soylittle)
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    ssize_t resultado = connect(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    if(resultado < 0) {
        perror("error connect");
        return -1;
    }

    uint32_t cabecera = 4;
    std::array<char,2048> mensaje = {'H', 'o', 'l', 'a'};
    uint16_t codigo = 5;
    uint8_t respuesta;


    if(soylittle)
        cabecera = std::byteswap(cabecera);
    ssize_t enviados = write(sd, &cabecera, 4);
    if(enviados < 0) {
        perror("error write 1");
        return -1;
    }
    std::cout << "enviado\n";
    enviados = write(sd, mensaje.data(), std::byteswap(cabecera));
    if(enviados < 0) {
        perror("error write 2");
        return -1;
    }
    if(soylittle)
        codigo = std::byteswap(codigo);
    enviados = write(sd, &codigo, 2);
    if(enviados < 0) {
        perror("error write 3");
        return -1;
    }

    ssize_t leidos = read(sd, &respuesta, 1);
    if(leidos < 0) {
        perror("error read");
        return -1;
    }

    std::cout << "Recibido: " << respuesta;


}