#include <iostream>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <cstdlib>

int main() {
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
    ssize_t resultado = bind(sd,(sockaddr *)&vinculo,sizeof(vinculo));
    if (resultado < 0) {
        perror("Error en bind");
        return 1;
    }
    sockaddr_in dircliente = {};
    socklen_t long_dir = sizeof(dircliente);

    while (1) {
        int16_t recibido;
        //recepción
        ssize_t leidos = recvfrom(sd,&recibido,2,0,(sockaddr *)&dircliente,&long_dir);
        if (leidos < 2) {
            perror("Error en recepcion");
            break;
        } else {
            if(soylittle)
                recibido = std::byteswap(recibido);
            //servicio
            int32_t resultado = recibido + 1;
            //envío
            if (soylittle)
                resultado = std::byteswap(resultado);
            ssize_t enviados = sendto(sd, &resultado, 4, 0, (sockaddr *)&dircliente, sizeof(dircliente));
            if (enviados < 4) {
                perror("Error en envio");
                break;
            }
        }
    }
    close(sd);
    return 0;
}