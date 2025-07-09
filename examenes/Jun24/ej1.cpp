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


int main(int argc, char * argv[]){
    if (argc < 4) {
        perror("Error en numero de entrada de argumentos");
        return 1;
    }
    if (std::stoi(argv[3]) < 0) {
        perror("Error: Valor de identificador debe ser mayor a 0");
        return 1;
    }

    bool soylittle = false;
    if (std::endian::native == std::endian::little) {
        soylittle = true;
    }

    int sd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Error creando socket");
        return 1;
    }

    sockaddr_in vinculo;
    vinculo.sin_family = PF_INET;
    vinculo.sin_port = std::stoi(argv[2]);
    if (soylittle)
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    vinculo.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t long_vin = sizeof(vinculo);

    ssize_t resultado = bind(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    if(resultado < 0) {
        perror("Error en bind");
        return 1;
    }
    //Conexión realizada en este punto
    bool fin = false;
    fd_set conjunto;
    FD_ZERO(&conjunto);
    FD_SET(0, &conjunto); //Añado el teclado al set
    FD_SET(sd, &conjunto); //Añado la conexión al servidor al set
    int max_fd = 0;
    max_fd = std::max(sd, 0);
    while(!fin) {
        FD_ZERO(&conjunto);
        FD_SET(0, &conjunto); //Añado el teclado al set
        FD_SET(sd, &conjunto); //Añado la conexión al servidor al set

        resultado = select(max_fd + 1, &conjunto, 0, 0, 0);
        if(resultado < 0) {
            perror("Error en select");
            break;
        }
        //LLegados aqui hay datos en algún lado

        if(FD_ISSET(sd, &conjunto)) { //Hay datos del servidor
            uint16_t id_sensor, codigo;
            ssize_t leidos = recvfrom(sd, &id_sensor, 2, 0,(sockaddr *)&vinculo, &long_vin); //recibo los datos de una id
            if(leidos < 0) {
                perror("Error en recepcion");
                break;
            }
            leidos = recvfrom(sd, &codigo, 2, 0, (sockaddr *)&vinculo, &long_vin); //recibo los datos de un código de información
            if(leidos < 0) {
                perror("Error en recepcion");
                break;
            }
            if(soylittle) {
                id_sensor = std::byteswap(id_sensor);
                codigo = std::byteswap(codigo);
            }
            if (id_sensor == std::stoi(argv[3])) { //El datagrama es para este sensor
                if(codigo == 0xDEAD) { //Si es el código de apagado
                    fin = true;
                }
            }

        } else if (FD_ISSET(0, &conjunto)) { //Hay datos de teclado
            std::array<char,64> buffer;
            uint16_t id_sensor = static_cast<uint16_t>(std::stoi(argv[3])); //asigno la id del sensor, que viene de la terminal
            ssize_t leidos = read(0, buffer.data(), buffer.size()); //leo la temperatura de teclado
            int16_t temperatura = std::stoi(buffer.data());
            if (leidos < 0) {
                perror("Error en lectura");
                break;
            }
            if((temperatura > -1000) && (temperatura < 1000)) { //si la temperatura es válida
                if(soylittle) { //swappeo si necesario
                    id_sensor = std::byteswap(id_sensor);
                    temperatura = std::byteswap(temperatura);
                }
                ssize_t enviados = sendto(sd, &id_sensor, 2, 0, (sockaddr *)&vinculo, long_vin); //envío la id del sensor
                if(enviados < 0) {
                    perror("Error en envío");
                    return 1;
                }
                enviados = sendto(sd, &temperatura, 2, 0, (sockaddr *)&vinculo, long_vin); //envío la temperatura
                if(enviados < 0) {
                    perror("Error en envío");
                    return 1;
                }
            } else {
                std::cout << "Valor de temperatura no valido. Introduzca un valor entre -1000 y 1000\n";
            }
        }
    }
    close(sd);
    return 0;
}   
    