#include <iostream>
#include <string>
#include <array>
#include <bit>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

ssize_t writen(int fd, const void *data, size_t N){
    size_t por_escribir = N;
    size_t total_escritos = 0;
    char * p = (char *)data;
    ssize_t escritos;
    do{
        escritos = write(fd, p + total_escritos, por_escribir);
        if(escritos > 0){
            total_escritos +=escritos;
            por_escribir -=escritos;
        }
    }while((escritos > 0) && (total_escritos < N));
    if(escritos < 0){
        return -1;
    }else{
        return total_escritos;
    }
}

int main(){

    int sd = socket(PF_INET, SOCK_STREAM, 0);
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

    int resultado = connect(sd, (sockaddr *)&dir_serv, sizeof(dir_serv));
    if(resultado < 0){
        perror("imposible conectarse al servidor");
        return 1;
    }
    //a partir de aquí ya estás conectado con el servidor. Pídele que haga cosas
    bool fin = false;
    while(!fin){  
        std::string cadena;
        std::getline(std::cin, cadena);
        if(cadena == "fin"){
            fin = true;
        }else{
            ssize_t escritos = writen(sd, cadena.data(), cadena.size());
            if(escritos < (ssize_t)cadena.size()){
                perror("no se pudo escribir correctamente en el socket");
                fin = true;
            }else{
                std::array<char,2048> almacen;
                ssize_t leidos = read(sd, almacen.data(),almacen.size());
                if(leidos < 0){
                    perror("fallo en la lectura");
                    fin = true;
                }else if(leidos == 0){
                    std::cout << "El servidor ha desconectado!\n";
                    fin = true;
                }else{
                    //el servidor NO manda una cadena terminada en '\0', pero la tratas como tal
                    std::string_view sv(almacen.data(),leidos);
                    std::cout << sv << std::endl;
                }
            }
        }
*
    }
    close(sd);

    return 0;
}