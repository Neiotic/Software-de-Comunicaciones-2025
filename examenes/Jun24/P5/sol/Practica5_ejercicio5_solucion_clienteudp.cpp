#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(int argc, char *argv[]){

    if(argc < 2){
        std::cout << "Intruduzca un número para enviarlo al servidor\n";
        return 1;
    }

    int sd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in dir_serv = {};
    dir_serv.sin_family = AF_INET;
    dir_serv.sin_port = 7000;
    if(std::endian::native == std::endian::little){
        dir_serv.sin_port = std::byteswap(dir_serv.sin_port);
    }//siempre asegúrate de que está en big endian!
    dir_serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Vamos a intentar mandarle tarea al servidor y a esperar que nos responda(ojalá!)
    int16_t numero = std::stoi(argv[1]); //el número me lo pasaron como argumento al programa
    //paso a big endian:
    if(std::endian::native == std::endian::little){
        numero = std::byteswap(numero);
    }

    ssize_t escritos = sendto(sd, &numero, 2, 0, (sockaddr *)&dir_serv, sizeof(dir_serv));
    if(escritos < 2){
        perror("no se pudo escribir correctamente en el socket");
        return 1;
    }else{
        int32_t resultado;
        ssize_t leidos = recvfrom(sd, &resultado,4,0,0,0);
        //si pongo los ultimos argumentos de recvfrom a cero, me fio de que me ha respondido el servidor
        //(pero podría haber recibido el mensaje de algún otro ¿"malvado"?)
         if(leidos < 4){
            perror("fallo en la lectura");
            return 1;
        }else{
            if(std::endian::native == std::endian::little){
                resultado = std::byteswap(resultado);
            }
            std::cout << "El resultado es: " << resultado << std::endl;
        }
    }
    close(sd);

    return 0;
}