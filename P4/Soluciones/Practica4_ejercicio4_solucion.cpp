
#include <iostream>
#include <array>
#include <string>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

int main(){

    std::cout << "Esperando escritor en la fifo /tmp/sc_mensajes...\n";
    int fd_fifo = open("/tmp/sc_mensajes", O_RDONLY);
    if(fd_fifo < 0){
        perror("error al abrir la fifo");
        return 1;
    }
    std::cout << "Conectado con el otro usuario! Ya podéis intercambiar mensajes\n";
	fd_set conjunto;
    FD_ZERO(&conjunto);

    bool fin = false;
	//Bucle de servicio 
	while(!fin){ 
        FD_SET(0, &conjunto);
        FD_SET(fd_fifo, &conjunto);
        timeval timeout = {3,0}; //cada iteración la cuenta atrás comienza de nuevo
        int result;
		do{ 
			result = select(fd_fifo+1, &conjunto, 0, 0, &timeout);
		}while((result < 0) && (errno == EINTR));
	    if(result < 0){
			perror("error en select");
		    return 1;
		}else if(result==0){ //pinta un punto por pantalla
		    std::cout << ".\n";
		}
		  
        if( FD_ISSET(0, &conjunto) ) {
            std::string mensaje;
            std::getline(std::cin,mensaje);
            std::cout << "MENSAJE ENVIADO: " << mensaje << std::endl;
            /*no manda nada a ningun sitio, es una simulacion*/
		}
		  
		if( FD_ISSET(fd_fifo, &conjunto) ) {
            std::array<char,2048> almacen; 
			ssize_t leidos = read(fd_fifo, almacen.data(), almacen.size());
			if(leidos < 0){
				perror("error al leer la fifo");
				return 1; //salida del programa (fallo en pipe)
			}else if(leidos > 0){
				std::string_view sv(almacen.data(),leidos);
                //lo que se ha leido NO es una cadena, pero "la tratamos" como si lo fuera
                std::cout << "RECIBIDO: " << sv;
			}else{
				//el otro extemo ha cerrado la conexión
				close(fd_fifo);
				fin = true;
            }
		}
	}//fin del while

	return 0;
}