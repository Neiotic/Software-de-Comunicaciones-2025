#include <iostream>
#include <fstream>
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

int main(int argc, char *argv[]){

    if(argc < 2){
        std::cout << "Introduzca como argumento al programa el nombre del archivo a enviar\n";
        return 1;
    }
    //aquí está el nombre del fichero a transmitir
    std::string nombre_fich = argv[1]; 
    //vamos a intentar abrir el fichero antes de seguir y así no metemos la pata
    std::ifstream in(nombre_fich, std::ios::binary);    
    if(in.fail()){
        std::cout << "No se puede abrir el archivo " << nombre_fich << ". Ni molesto al servidor\n";
        return 1;
    }

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
    //a partir de aquí ya estás conectado con el servidor
    
    //envio primero una cabecera con el tamaño del nombre del fichero
    uint8_t tam = nombre_fich.size(); //esto no hace falta pasarlo a big endian (¡es un byte!)
    ssize_t escritos = writen(sd, &tam, 1);
    if(escritos != 1){
        std::cout << "error al escribir en el socket la cabecera\n";
        return 1;
    }
    //ahora envio el nombre del fichero
    escritos = writen(sd, nombre_fich.data(), nombre_fich.size());
    if(escritos != (ssize_t)nombre_fich.size()){
        std::cout << "error al escribir en el socket el nombre del fichero\n";
        return 1;
    }
    //bucle de la asignatura: leo un bloque de fichero y lo envío por el socket en cada iteración
    std::array<char, 2048> buffer;
    ssize_t leidos;
    do{
        leidos = in.readsome(buffer.data(),2048);
        if(in.fail()){
            std::cout << "error en lectura del fichero\n";
            break; //para salir del bucle
        }
        escritos = writen(sd, buffer.data(), leidos);
        if(escritos != leidos){
            std::cout << "error al escribir en el socket un bloque de fichero\n";
            break; //para salir del bucle
        }
    }while(leidos > 0);

    in.close();
    close(sd);

    return 0;
}