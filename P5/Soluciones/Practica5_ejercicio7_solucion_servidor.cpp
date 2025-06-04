#include <iostream>
#include <array>
#include <bit>
#include <cstring> //para memcpy
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(){

    int sd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 8000;
    if(std::endian::native == std::endian::little){
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }//siempre asegúrate de que está en big endian!

    int resultado = bind(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    if(resultado < 0){
        perror("ruina bind");
        return 1;
    }

    sockaddr_in dir_cliente;
    socklen_t longitud_dir = sizeof(dir_cliente);
    while(1){  
        std::array<uint8_t, 1500> mensaje;

        ssize_t leidos = recvfrom(sd, mensaje.data(), mensaje.size(), 0, (sockaddr *)&dir_cliente, &longitud_dir);    
        if(leidos < 0) {      
            perror("error en la recepcion");      
            return 1;    
        }
        //un pequeño chequeo para ver si el datagrama recibido es correcto antes de procesarlo
        if((leidos >= 4) && (mensaje[1] == ((leidos - 2) / 2))){
            uint8_t tipo_operacion = mensaje[0];
            uint8_t num_operandos = mensaje[1];

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

            //se envia la respuesta al cliente:
            std::array<uint8_t, 5> respuesta = {};
            if((tipo_operacion == 0) || (tipo_operacion == 1)){
                respuesta[0] = 1; //la operación se pudo realizar
                if(std::endian::native == std::endian::little){
                    resultado = std::byteswap(resultado);
                }//paso el resultado a big endian y lo copio en la respuesta
                std::memcpy(respuesta.data()+1, &resultado, 4);
            }
            ssize_t escritos = sendto(sd, respuesta.data(), 5, 0, (sockaddr *)&dir_cliente, sizeof(dir_cliente));
            if(escritos < 0){
                perror("error en sendto");
                return 1;
            }
        }
    }
    //aqui no llega nunca
    close(sd);

    return 0;
}