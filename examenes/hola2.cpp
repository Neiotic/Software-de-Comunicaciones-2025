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


int main() {
    ssize_t sd = socket(PF_INET,SOCK_STREAM,0);
    if (sd < 0) {
        perror ("Error en socket");
        return 1;
    }
    sockaddr_in vinculo;
    vinculo.sin_family = PF_INET;
    vinculo.sin_port = 5052;
    if (std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    ssize_t resultado = bind(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    //compruebo error -1

    resultado = listen(sd, 5);
    //compruebo error -1

    sockaddr_in dir_cliente;
    socklen_t tam_dir = sizeof(dir_cliente);

    while (1) {
        ssize_t csd = accept(sd, (sockaddr *)&dir_cliente, &tam_dir);
        //compruebo error -1
        uint32_t cabecera;
        std::array<char,2048> buffer;
        uint16_t codigo;
        ssize_t leidos = read(csd, &cabecera, 4);
        //compruebo error -1
        if (std::endian::native == std::endian::little) {
            cabecera = std::byteswap(cabecera);
        }

        leidos = read(csd, buffer.data(), cabecera);
        //compruebo error -1
        
        leidos = read(csd, &codigo, 2);
        //compruebo error -1
        if (std::endian::native == std::endian::little) {
            codigo = std::byteswap(codigo);
        }

        uint8_t retorno;
        if (memcmp(buffer.data(), "Hola", 4) == 0 && codigo == 5) { //Mensaje y código válidos
            retorno = 1;
            ssize_t enviados = write(csd, &retorno, 1);
            //compruebo error -1
        } else {
            retorno = 0;
            ssize_t enviados = write(csd, &retorno, 1);
        }
        close(csd);
    }

    close(sd);
    return 0;
}