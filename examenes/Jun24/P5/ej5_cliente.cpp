#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <bit>
#include <cstring> //para memcpy
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char * argv[]) {

    ssize_t sd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Error en creacion de socket");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = PF_INET;
    vinculo.sin_port = 8000;
    if(std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Conexión realizada
    int entrada, operando;
    int8_t t_operacion, n_operandos;
    int16_t operando16;
    int32_t resultado;
    std::array<int8_t,2048> respuesta;
    std::array<int8_t,2048> mensaje;
    std::cout << "Introduzca tipo de operacion (0:+ 1:-): \n";
    std::cin >> entrada;
    t_operacion = static_cast<int8_t>(entrada);
    mensaje[0] = t_operacion;
    std::cout << "Introduzca numero de operandos: \n";
    std::cin >> entrada;
    n_operandos = static_cast<int8_t>(entrada);
    mensaje[1] = n_operandos;

    for (int i = 0; i < entrada; i++) {
        std::cout << "Introduzca operando " << i << " : ";
        std::cin >> operando;
        operando16 = static_cast<int16_t>(operando);
        if(std::endian::native == std::endian::little) 
            operando16 = std::byteswap(operando16);
        std::memcpy(&mensaje[2 + i],&operando16, 2);
    } //tras esto, el mensaje está formado

    ssize_t enviados = sendto(sd, mensaje.data(), mensaje.size(), 0, (sockaddr *)&vinculo, sizeof(vinculo));
    if (enviados == 0) {
        perror("Error en envio");
        return 1;
    } else {
        ssize_t recibidos = recvfrom(sd, &respuesta, 5, 0, 0, 0);
        if (recibidos < 0) {
            perror("Error en recepcion ");
            return 1;
        } else {
            if(respuesta[0] == 1) { //operacion realizada
                std::cout << "operacion realizada con exito: \n" << "Resultado: ";
                std::memcpy(&resultado,respuesta.data() + 1, 4);
                if (std::endian::native == std::endian::little) {
                    resultado = std::byteswap(resultado);
                }
                std::cout << resultado;
            } else {
                std::cout << "No se pudo realizar la operacion \n";
            }
        }
    }
    close(sd);

    return 0;
}