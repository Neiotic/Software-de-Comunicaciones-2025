#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <bit>
#include <csignal>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

ssize_t readn(int fd, void *data, size_t N){
    size_t por_leer = N;
    size_t total_leidos = 0;
    char * p = (char *)data;
    ssize_t leidos;
    do{
        leidos = read(fd, p + total_leidos, por_leer);
        if(leidos > 0){
            total_leidos +=leidos;
            por_leer -=leidos;
        }
    }while((leidos > 0) && (total_leidos < N));
    if(leidos < 0){
        return -1;
    }else{
        return total_leidos;
    }
}

int main(){

    signal(SIGCHLD, SIG_IGN); //ya no hará falta hacer wait(0) en el padre

    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 6000;
    if(std::endian::native == std::endian::little){
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }//siempre asegúrate de que está en big endian!

    int resultado = bind(sd, (sockaddr *)&vinculo, sizeof(vinculo));
    if(resultado < 0){
        perror("ruina bind");
        return 1;
    }

    resultado = listen(sd,10);
    if(resultado < 0){
        perror("ruina listen");
        return 1;
    }

    sockaddr_in dir_cliente;
    socklen_t longitud_dir = sizeof(dir_cliente);
    while(1){  
        int csd = accept(sd, (sockaddr *)&dir_cliente, &longitud_dir);    
        if(csd < 0) {      
            perror("error en accept");      
            return 1;    
        }

        pid_t pid = fork();
        if(pid < 0){
            perror("error en fork");
            return 1;
        }
        if(pid == 0){ //zona exclusiva del hijo
            //listo para el servicio de transferencia de fichero desde el cliente
            
            //primero leo la cabecera con el tamaño del nombre del fichero
            uint8_t tam; 
            ssize_t leidos = readn(csd, &tam, 1);
            if(leidos != 1){
                close(csd);
                exit(1); //esto no tiene buena pinta, mejor terminar el hijo
            }
            //ahora leo el texto con el nombre del fichero (exactamente los bytes que necesito)
            std::array<char,2048> almacen;
            leidos = readn(csd, almacen.data(), tam);
            if(leidos != tam){
                close(csd);
                exit(1); //esto no tiene buena pinta, mejor terminar el hijo
            }
            //en almacen no hay una cadena, así que la creo porque la necesito para abrir el fichero en modo escritura
            std::string nombre_fich(almacen.data(),tam);

            std::ofstream out(nombre_fich, std::ios::binary);
            if(!out.fail()){
                //bucle de la asignatura: leo un bloque del socket y lo guardo en el fichero
                ssize_t leidos;
                do{
                    leidos = read(csd, almacen.data(), almacen.size());
                    if(leidos > 0){
                        out.write(almacen.data(),leidos);
                        if(out.fail()){
                            std::cout << "Fallo en escritura de bloque de fichero\n";
                            break; //salgo del bucle de la asignatura
                        }
                    }
                }while(leidos > 0);

                out.close();
            }
            close(csd);
            //cumple contrato
            std::exit(0);
        }//fin zona exclusiva del hijo

        //el padre simplemente vuelve al bucle a esperar de nuevo en accept()
        close(csd); //no lo necesita
    }
    //aqui no llega nunca
    close(sd);

    return 0;
}