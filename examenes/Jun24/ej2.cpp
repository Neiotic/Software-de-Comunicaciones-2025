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

void manejadora_alarma(int sig) {

}

int main(int argc, char * argv[]) {
    signal(SIGALRM, manejadora_alarma);
    bool soylittle = false;
    if(std::endian::native == std::endian::little) {
        soylittle = true;
    }

    ssize_t sd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Error en socket");
        return 1;
    }
    sockaddr_in vinculo = {};
    vinculo.sin_family = PF_INET;
    vinculo.sin_port = 12345;
    if(soylittle)
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    ssize_t resultado = bind(sd,(sockaddr *)&vinculo, sizeof(vinculo));
    if(resultado < 0) {
        perror("Error en bind");
        return 1;
    }

    sockaddr_in dir_cliente;
    socklen_t tam_dir = sizeof(dir_cliente);
    uint16_t id_sensor;
    int16_t temperatura;
    int16_t max_temperatura = 0;

    while (1) {
        
        ssize_t leidos = recvfrom(sd, &id_sensor, 2, 0, (sockaddr *)&dir_cliente, &tam_dir);
        if (leidos < 0) {
            perror("Error en lectura");
            break;
        }
        if(soylittle)
            id_sensor = std::byteswap(id_sensor);

        leidos = recvfrom(sd, &temperatura, 2, 0, (sockaddr *)&dir_cliente, &tam_dir);
        if (leidos < 0) {
            perror("Error en lectura");
            break;
        }
        if(soylittle)
            temperatura = std::byteswap(temperatura);

        
        

        







    }

    close(sd);
    return 0;
}