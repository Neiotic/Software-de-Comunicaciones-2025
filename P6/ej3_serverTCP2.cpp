#include <iostream>
#include <bit>
#include <set>
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


int main(int argc, char * argv[]){
    if(argc < 2) {
        perror("ERROR, falta de argumentos");
        return 1;
    }

    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        perror("ruina socket tcp");
        return 1;
    }

    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = std::stoi(argv[1]);
    if(std::endian::native == std::endian::little){
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    } //siempre asegúrate de que está en big endian!

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


    fd_set conjunto;
    FD_ZERO(&conjunto);
    std::set<int> set_clientes = {};
    int max_fd;

    while (1) {
        FD_SET(sd,&conjunto);
        for(auto &csd : set_clientes) {
            FD_SET(csd,&conjunto);
        }

        if(set_clientes.size() > 0) { //Si hay clientes
            max_fd = std::max(sd, *set_clientes.rbegin());
        } else {
            max_fd = sd;
        }

        int resultado = select(max_fd + 1,&conjunto,0,0,0);
        if (resultado < 0) {
            perror ("ERROR en select");
            break;
        }

        //Compruebo conexión nueva
        if (FD_ISSET(sd, &conjunto)) {
            int csd = accept(sd, 0, 0);
            if (csd < 0) {
                perror("ERROR en accept");
                break;
            }
            set_clientes.insert(csd);
        }
        //Si llega por otro fd, es que hay datos nuevos
        std::set<int> para_borrar = {};

        for (auto &cliente_actual : set_clientes) {
            if(FD_ISSET(cliente_actual, &conjunto)) {// Hay datos de ese cliente
                std::array<char, 2048> buffer;
                int leidos = read(cliente_actual,&buffer,buffer.size());
                if(leidos > 0){
                    for(auto & otro_cliente : set_clientes){
                        if(otro_cliente != cliente_actual){
                            ssize_t escritos = writen(otro_cliente, buffer.data(),leidos);
                            if(escritos != leidos){
                                //mal asunto con otro_cliente
                                close(otro_cliente);
                                para_borrar.insert(otro_cliente); //luego lo borraré del conjunto de clientes
                            }
                        }
                    }
                } else if(leidos <= 0){
                    close(cliente_actual);
                    //está claro que tengo que quitar cliente_actual del set_clientes
                    //pero no puedo hacerlo dentro del bucle for que está recorriendo set_clientes! 
                    //por eso, guardo el descriptor en un conjunto o vector para que, ya fuera
                    //del bucle for que recorre set_clientes, pueda borrarlo
                    para_borrar.insert(cliente_actual); //luego lo borraré del conjunto de clientes
                }
            }
        }
        for (auto &cliente : para_borrar) {
            set_clientes.erase(cliente);
        }
    }
    close(sd);
    return 0;
}