#include <iostream>
#include <fstream>
#include <array>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

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

void manejadora(int numero_senal){
wait(0); //cumple contrato, y sin bloquearse
}

int main() {
    signal(SIGCHLD,manejadora);
    ssize_t sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror ("ERROR en socket");
        return 1;
    }
    
    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = 6000;
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
    uint8_t tam = 0;
    std::array<char,2048> nom_fich;
    while(1){
        int csd = accept(sd, (sockaddr *)&dir_cliente, &longitud_dir);    
        if(csd < 0) {      
            perror("error en accept");      
            return 1;  
        }

        pid_t pid = fork(); //FORK
        if (pid < 0) {
            perror("ERROR en fork");
            return 1;
        }

        if (pid == 0) {
            //SOY EL HIJO
            int res = readn(csd, &tam, 1); //Leo el tamaño
            if (res < 1) {
                perror("ERROR leyendo tamaño del archivo");
                continue;
            }
            res = readn(csd, nom_fich.data() ,tam); //Leo el nombre del fichero
            if(res != tam) {
                perror("ERROR leyendo nombre de fichero");
                continue;
            }
            std::string fichero(nom_fich.data(),tam);
            std::ofstream out(fichero, std::ios::binary); //crea el archivo de salida
            if (out.fail()) {
                perror("Error en creacion de fichero");
                return 1;
            }
            //Ahora leo los datos y los escribo en el archivo

            std::array<char, 2048> buffer;
            ssize_t leidos;
            do{
                leidos = readn(csd,&buffer,buffer.size());
                if(leidos < 0){
                    std::cout << "error en recepción\n";
                    continue; //para salir del bucle
                }
                std::string salida(buffer.data(),leidos);
                out << salida;
            }while(leidos > 0);
            out.close();
            std::exit(0);
        } 
        
    }
    close(sd);
    return 0;
}