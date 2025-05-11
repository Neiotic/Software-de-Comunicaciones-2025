#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <cstring> //para memcpy
#include <string_view>
#include <array>
#include <fstream>
#include <iostream>


int main(){

	//Abre el fichero de texto para escritura de los sucesos (logs)
    std::ofstream fich_log("logs.txt");
    if(fich_log.fail()){
        std::cout << "fallo al abrir el fichero de logs\n";
        return 1;
    }

    std::cout << "Intermediario: esperando escritores en las fifos\n";
    //Abre la primera fifo para leer
    int fd_fifo1 = open("/tmp/log_fifo1", O_RDONLY);
    if(fd_fifo1 < 0){
        perror("ruina abriendo /tmp/log_fifo1");
        return 1;
    }
    //Abre la segunda fifo para leer
    int fd_fifo2 = open("/tmp/log_fifo2", O_RDONLY);
    if(fd_fifo2 < 0){
        perror("ruina abriendo /tmp/log_fifo2");
        return 1;
    }

    std::cout << "Los escritores ya están listos. A trabajar!\n";

	fd_set conjunto;
	FD_ZERO(&conjunto);
    //Se añaden los descriptores de las fifos al conjunto
    FD_SET(fd_fifo1,&conjunto);
    FD_SET(fd_fifo2,&conjunto);
    //Este paso es crítico, tenemos que conocer el máximo descriptor
    int max_fd = std::max(fd_fifo1,fd_fifo2);
	
    int procesos_escritores = 2;
    //Comienza el bucle de atención a los procesos
	while(procesos_escritores > 0){
        fd_set modificado;
		//copiamos los descriptores originales
		std::memcpy(&modificado,&conjunto,sizeof(modificado));
		
		int result;
        //Select en bucle de reintentos, por si llega una señal
        do{
            result = select(max_fd + 1, &modificado, 0, 0 , 0);
        }while((result <0 ) && (errno == EINTR));
		if (result < 0){
			perror("ruina en select");
			return 1;
		}
        //Han llegado datos. Vemos por dónde y actuamos
        std::array<char, 2048> almacen;

		if(FD_ISSET(fd_fifo1,&modificado)){
			ssize_t leidos = read(fd_fifo1, almacen.data(), almacen.size());
			if(leidos < 0){
				perror("desastre en read");
				return 1;
			}else if(leidos == 0){
				//el proceso cerró la fifo
				FD_CLR(fd_fifo1,&conjunto);
				procesos_escritores--;
			}else{
                std::string_view sv(almacen.data(), leidos);
				fich_log << "Proceso 1: " << sv;
	
			}
		}
        if(FD_ISSET(fd_fifo2,&modificado)){
			ssize_t leidos = read(fd_fifo2, almacen.data(), almacen.size());
			if(leidos < 0){
				perror("desastre en read");
				return 1;
			}else if(leidos == 0){
				//el proceso cerró la fifo
				FD_CLR(fd_fifo2,&conjunto);
				procesos_escritores--;
			}else{
                std::string_view sv(almacen.data(), leidos);
				fich_log << "Proceso 2: " << sv;
	
			}
		}
	}

	fich_log.close();
	return 0;
}
