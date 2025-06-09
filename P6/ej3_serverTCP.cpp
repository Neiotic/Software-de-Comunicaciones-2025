#include <iostream>
#include <fstream>
#include <array>
#include <set>
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


int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Error en numero de argumentos. Valor esperado: 1");
        return 1;
    }
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror ("ERROR en socket");
        return 1;
    }
    
    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    vinculo.sin_port = std::stoi(argv[1]);
    if(std::endian::native == std::endian::little) {
        vinculo.sin_port = std::byteswap(vinculo.sin_port);
    }
    //vinculo.sin_addr.s_addr = inet_addr("127.0.0.1");

    ssize_t resultado = bind(sd,(sockaddr *)&vinculo,sizeof(vinculo));
    if (resultado < 0) {
        perror ("Error en bind");
        return 1;
    }

    resultado = listen(sd,10);
    if (resultado < 0) {
        perror ("ERROR en listen");
        return 1;
    }

    sockaddr_in dir_cliente = {};
    socklen_t longitud_dir = sizeof(dir_cliente);
    //Reactor
    fd_set conjunto;
    FD_ZERO(&conjunto);

    int max_fd;
    std::set<int> set_clientes = {};

    while(1) {
        FD_SET(sd,&conjunto);

        int num_clientes = set_clientes.size(); //Cantidad de clientes en el set
        auto it = set_clientes.begin(); //Coloca un iterador en la primera posicion
        for(int i = 0; i < num_clientes; i++) { //recorre el set con el it++
            int csd = *it; //csd es el descriptor al que apinta it
            FD_SET(csd,&conjunto); //lo añade al conjunto
            it++;
        }

        if(set_clientes.size() > 0) { //si hay clientes
            max_fd = std::max(sd, *set_clientes.rbegin()); //el maximo será el de sd o el ultimo cliente
        } else { //si no hay clientes
            max_fd = sd; //el maximo será sd
        }

        resultado = select(max_fd + 1, &conjunto, 0, 0, 0);//Select bloqueará hasta que haya datos en algún fd de su conjunto
        if (resultado < 0) {
            perror("ERROR en select");
            break;;
        }
        //A partir de aquí hay datos
        //Si hay datos en sd es que un cliente está esperando conexión
        if (FD_ISSET(sd,&conjunto)) {
            int csd = accept(sd, (sockaddr *)&dir_cliente, &longitud_dir);//Acepta la conexión
            if (csd < 0) {
                perror("ERROR en accept");
                break;
            }
            set_clientes.insert(csd); //Lo inserto en el set
        }
        std::set<int> paraborrar = {};
        for (auto &cliente_actual : set_clientes) { //recorre todo el set_clientes con un puntero iterador llamado cliente_actual
            if(FD_ISSET(cliente_actual,&conjunto)) {
                std::array<char,2048> buffer;
                ssize_t leidos = read(cliente_actual, buffer.data(), buffer.size());
                if (leidos > 0) {
                    for(auto & otro_cliente : set_clientes) {
                        if (otro_cliente != cliente_actual) {
                            ssize_t escritos = writen(otro_cliente,&buffer, leidos);
                            if(escritos != leidos) { //el otro cliente no lee. lo saco
                                paraborrar.insert(otro_cliente);
                                close(otro_cliente);
                            }
                        }
                    }
                } else if (leidos <= 0) { //Debo cerrar el cliente ya que no lee
                    paraborrar.insert(cliente_actual);
                    close(cliente_actual);
                }

            }
        }

        for(auto & csd : paraborrar){
            set_clientes.erase(csd);
        }
    }
    close(sd);
    return 0;
}