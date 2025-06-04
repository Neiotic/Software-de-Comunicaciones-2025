#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(){

    int sd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in dir_serv = {};
    dir_serv.sin_family = AF_INET;
    dir_serv.sin_port = 6000;
    if(std::endian::native == std::endian::little){
        dir_serv.sin_port = std::byteswap(dir_serv.sin_port);
    }//siempre asegúrate de que está en big endian!
    dir_serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Vamos a intentar hacer el eco mandando datagramas al servidor y esperando que nos responda(ojalá!)
    bool fin = false;
    while(!fin){  
        std::string cadena;
        std::getline(std::cin, cadena);
        if(cadena == "fin"){
            fin = true;
        }else{
            ssize_t escritos = sendto(sd, cadena.data(), cadena.size(), 0, (sockaddr *)&dir_serv, sizeof(dir_serv));
            if(escritos < (ssize_t)cadena.size()){
                perror("no se pudo escribir correctamente en el socket");
                fin = true;
            }else{
                std::array<char,2048> almacen;
                ssize_t leidos = recvfrom(sd, almacen.data(),almacen.size(),0,0,0);
                //si pongo los ultimos argumentos de recvfrom a cero, me fio de que me ha respondido el servidor
                //(pero podría haber recibido el mensaje de algún otro ¿"malvado"?)
                if(leidos < 0){
                    perror("fallo en la lectura");
                    fin = true;
                }else{
                    //el servidor NO manda una cadena terminada en '\0', pero la tratas como tal
                    std::string_view sv(almacen.data(),leidos);
                    std::cout << sv << std::endl;
                }
            }
        }

    }
    close(sd);

    return 0;
}