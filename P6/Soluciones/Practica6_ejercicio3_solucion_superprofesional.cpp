#include <iostream>
#include <array>
#include <set>
#include <bit>
#include <unistd.h>
#include <sys/socket.h>
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



int main(int argc, char *argv[]) {
    //chequeo antes de seguir
    if(argc < 2){
        std::cout << "Introduzca el puerto como argumento al programa\n";
        return 1;
    }
   
    uint16_t puerto = std::stoi(argv[1]);
 
    //creamos el socket tcp
    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("error en socket");
        return 1;
    }
   
    sockaddr_in vinculo = {};
    vinculo.sin_family = AF_INET;
    if(std::endian::native == std::endian::little){
        vinculo.sin_port = std::byteswap(puerto);
    }
   
   //vinculamos el socket con el puerto
   int resultado = bind(sd, (sockaddr *) &vinculo, sizeof(vinculo));
   if(resultado < 0){
        perror("error en bind");
        return 1;
   }
   
   //le decimos al sistema que este socket será servidor tcp
   resultado = listen(sd, 5); 
   if ( resultado < 0) {
        perror("error en listen");
        return 1;
   }
   
   fd_set cjto_descriptores;
   FD_ZERO(&cjto_descriptores);
   
   //necesito el maximo descriptor para el select
   int max_fd;
   //este conjunto C++ me servirá para ir guardando los descriptores de los clientes conectados al chat
   std::set<int> set_clientes = {};
   
   //bucle infinito con un select
   while(1) {
        //necesito un cjto_descriptores con valores actualizados cada vez que llame a select
        //primero incluyo el socket de conexión en el conjunto
        FD_SET(sd, &cjto_descriptores);
        //luego incluyo todos los descriptores de clientes conectados
        for(auto & csd : set_clientes){
            FD_SET(csd, &cjto_descriptores);
        }
        //por último, actualizo la variable max_fd
        if(set_clientes.size() > 0){ 
			/*en un std::set de C++, el método .rbegin() devuelve un puntero al último elemento (rbegin significa "reverse begin"), y como este tipo de contenedor está ordenado de menor a mayor, pues ahí está el máximo */
            max_fd = std::max(sd, *set_clientes.rbegin());
        }else{ //si no hay clientes, el máximo descriptor siempre es sd
            max_fd = sd;
        }
        //espera hasta que algún descriptor esté listo
        resultado = select(max_fd+1, &cjto_descriptores, nullptr, nullptr, nullptr);
        if (resultado < 0) {
            perror("error en select");
            break; //ruina gorda. Fuera del bucle
        }

        //si he llegado aquí es porque hay datos listos! a ver por qué descriptor...
        
        //hay datos por el socket de conexiones? eso es que hay un cliente esperando a que lo aceptemos
        if (FD_ISSET(sd, &cjto_descriptores)){
            //se acepta la nueva conexión
            int csd = accept(sd, 0, 0);
            if(csd < 0){
                perror("error en accept");
                break; //error grave, fuera del bucle inmediatamente
            }
            //inserto el nuevo cliente en el conjunto de clientes
            set_clientes.insert(csd);           
        }

        //habrá sido algún cliente el que tenga datos?
		
		/*declaro un set auxiliar donde ir almacenando los clientes que ya no estén disponibles
        (verás que se utiliza más adelante) */
        std::set<int> set_para_borrar = {}; 

        for(auto & cliente_actual : set_clientes){
            if(FD_ISSET(cliente_actual, &cjto_descriptores)){
				std::array<char, 2048> almacen;
                ssize_t leidos = read(cliente_actual, almacen.data(), almacen.size());
                if(leidos > 0){
                    for(auto & otro_cliente : set_clientes){
                        if(otro_cliente != cliente_actual){
                            ssize_t escritos = writen(otro_cliente, almacen.data(),leidos);
                            if(escritos != leidos){
                                //mal asunto con otro_cliente
                                close(otro_cliente);
                                set_para_borrar.insert(otro_cliente); //luego lo borraré del conjunto de clientes
                            }
                        }
                    }
                }else if(leidos <= 0){
                    close(cliente_actual);
                    //está claro que tengo que quitar cliente_actual del set_clientes
                    //pero no puedo hacerlo dentro del bucle for que está recorriendo set_clientes! 
                    //por eso, guardo el descriptor en un conjunto o vector para que, ya fuera
                    //del bucle for que recorre set_clientes, pueda borrarlo
                    set_para_borrar.insert(cliente_actual); //luego lo borraré del conjunto de clientes
                }
            }
        }
        //operación limpieza:
        // antes de volver al select dejo en set_cliente solo los clientes válidos
        for(auto & csd : set_para_borrar){
            set_clientes.erase(csd);
        }

   }
   
   close(sd);

   return 0;
 }
