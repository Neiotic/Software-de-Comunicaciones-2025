#include <iostream>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <cstdlib>

int main (int argc, char *argv[]) {
    //Gestión de arquitectura
    bool soylittle = false;
    if (std::endian::native == std::endian::little) {
        soylittle = true;
    }

    ssize_t sd = socket(AF_INET,SOCK_DGRAM,0);

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 7000;
    if (soylittle)
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    int16_t numero = std::stoi(argv[1]);
    if(soylittle)
        numero = std::byteswap(numero);

    ssize_t enviados = sendto(sd, &numero, 2, 0, (sockaddr *)&vinculo, sizeof(vinculo));
    if (enviados < 2) {
        perror("ERROR en sendto");
        return 1;
    } else {
        int32_t respuesta;
        ssize_t leidos = recvfrom(sd,&respuesta,4,0,0,0);
        if (leidos < 4) {
            perror ("Recuperados menos datos de los esperados");
            return 1;
        } else {
            if (soylittle)
                respuesta = std::byteswap(respuesta);
            std::cout << "Recibido: " << respuesta << std::endl;
        }
    }
    close(sd);
    return 0;
}