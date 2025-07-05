#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <bit>
#include <cstring> //para memcpy
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main (int argc, char * argv[]) {

    ssize_t sd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Error en socket");
        return 1;
    }
    sockaddr_in vinculo;
    vinculo.sin_family = PF_INET;
    vinculo.sin_port = 8000;
    if(std::endian::native == std::endian::little){
        vinculo.sin_port = std::byteswap(vinculo.sin_port); 
    }

    int resultado = bind(sd,(sockaddr *)&vinculo, sizeof(vinculo)); 
    if (resultado < 0) {
        perror ("Ruina bind");
        return 1;
    }

    sockaddr_in cliente;
    socklen_t long_cliente = sizeof(cliente);

    while (1) {
        std::array<uint8_t,2048> mensaje;

        ssize_t leidos = recvfrom(sd, mensaje.data(), mensaje.size(), 0, (sockaddr *)& cliente, &long_cliente);
        if(leidos < 0) {
            perror ("error en lectura");
            return 1;
        }

        uint8_t tipo_operacion, num_operandos;
        uint16_t operando;
        int i_operando;

        tipo_operacion = mensaje[0];
        num_operandos = mensaje[1];

        int16_t primer_operando;
        std::memcpy(&primer_operando, mensaje.data() + 2, 2);
        if(std::endian::native == std::endian::little){
            primer_operando = std::byteswap(primer_operando);
        }
        int32_t resultado = primer_operando;

        int offset = 4;
        for(uint8_t i = 1; i < num_operandos; i++){
            int16_t operando;
            std::memcpy(&operando, mensaje.data()+offset, 2);
            //convierto desde big endian:
            if(std::endian::native == std::endian::little){
                operando = std::byteswap(operando);
            }
            if(tipo_operacion == 0){ //suma
                resultado += operando;
            }else{ //resta
                resultado -= operando;
            }
            offset+=2;
        }
    }
    close(sd);
    return 0;
}