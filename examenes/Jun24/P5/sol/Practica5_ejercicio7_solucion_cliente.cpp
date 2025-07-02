#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <bit>
#include <cstring> //para memcpy
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(int argc, char *argv[]){

    if(argc < 3){
        std::cout << "Intruduzca tipo de operacion y (al menos) un operando\n";
        return 1;
    }

    int sd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in dir_serv = {};
    dir_serv.sin_family = AF_INET;
    dir_serv.sin_port = 8000;
    if(std::endian::native == std::endian::little){
        dir_serv.sin_port = std::byteswap(dir_serv.sin_port);
    }//siempre asegúrate de que está en big endian!
    dir_serv.sin_addr.s_addr = inet_addr("127.0.0.1");


    //Vamos a intentar mandarle tarea al servidor y a esperar que nos responda(ojalá!)
    //aquí se va a construir el mensaje para mandarlo al servidor
    std::array<uint8_t, 1500> mensaje;
    mensaje[0] = std::stoi(argv[1]); //el tipo de operación: 0 es suma, 1 es resta
    mensaje[1] = argc - 2 ; //el número de operandos
    
    int offset = 2;
    for(int i = 2; i < argc; i++){
        int16_t operando = std::stoi(argv[i]);
        if(std::endian::native == std::endian::little){
            operando = std::byteswap(operando);
        }//siempre asegúrate de que está en big endian!
        std::memcpy(mensaje.data()+ offset, &operando, 2);
        offset +=2;
    }
    //puedes llevar la cuenta de bytes del mensaje como quieras, en esta ocasión
    //he aprovechado que offset tiene el valor correcto
    ssize_t escritos = sendto(sd, mensaje.data(), offset, 0, (sockaddr *)&dir_serv, sizeof(dir_serv));
    if(escritos < offset){
        perror("no se pudo escribir correctamente en el socket");
        return 1;
    }else{
        std::array<uint8_t, 5> respuesta;
        ssize_t leidos = recvfrom(sd, &respuesta,5,0,0,0);
        //si pongo los ultimos argumentos de recvfrom a cero, me fío de que me ha respondido el servidor
        //(pero podría haber recibido el mensaje de algún otro ¿"malvado"?)
         if(leidos < 5){
            perror("fallo en la lectura");
            return 1;
        }else{
            if(respuesta[0] == 1){ //resultado de la operación ok
                int32_t resultado;
                std::memcpy(&resultado, respuesta.data()+1, 4);
                if(std::endian::native == std::endian::little){
                    resultado = std::byteswap(resultado);
                }
                std::cout << "El resultado es: " << resultado << std::endl;
            }else{
                std::cout << "La operación no se pudo realizar\n";
            }
        }
    }
    close(sd);

    return 0;
}