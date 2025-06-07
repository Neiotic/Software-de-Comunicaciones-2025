#include <iostream>
#include <array>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

int main() {
    ssize_t sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror ("ERROR en socket");
        return 1;
    }
    
    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 7000;
    if(std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    ssize_t resultado = bind(sd,(sockaddr *)&vinculo,sizeof(vinculo));
    if (resultado < 0) {
        perror ("Error en bind");
        return 1;
    }

    resultado = listen(sd,10);
    if (resultado < 0) {
        perror ("ERROR en litsen");
        return 1;
    }

    sockaddr_in dir_cliente = {};
    socklen_t longitud_dir = sizeof(dir_cliente);

    //CLIENTE
    while(1){  
        int csd = accept(sd, (sockaddr *)&dir_cliente, &longitud_dir);    
        if(csd < 0) {      
            perror("error en accept");      
            return 1;  
        }
        int16_t recibido;
        ssize_t leidos = read(csd,&recibido,2);
        if(std::endian::native == std::endian::little) {
        recibido = std::byteswap(recibido);
        }
        if (leidos < 2) {
            perror ("Error en lectura");
            close(sd);
            return 1;
        } else {
            int32_t resultado = recibido + 1;
            int32_t aux = resultado;
            if(std::endian::native == std::endian::little) {
                resultado = std::byteswap(resultado);
            }
            ssize_t enviados = writen(csd,&resultado,4);
            if (enviados < 4) {
                perror("Error en Writen");
                close(sd);
                return 1;
            } else {
                std::cout << "Enviado " << aux << " con exito!\n";
            }
        }
    }
    
    close(sd);
    return 0;
}