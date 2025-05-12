#include <iostream>
#include <fstream>
#include <array>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

int main(){
	std::cout << "Intermediario: Esperando apertura de fichero\n";
	int fd_fifo1 = open("log_fifo1",O_RDONLY);
	int fd_fifo2 = open("log_fifo2",O_RDONLY);
	if (fd_fifo1 < 0){
		perror("Error fd_fifo1\n");
		return 1;
	}
	if (fd_fifo2 < 0){
		perror("Error fd_fifo2\n");
		return 1;
	}
	std::cout << "Intermediario: Apertura realizada con exito!\n";
	std::ofstream fich_log("logs.txt");
	if (fich_log.fail()){
		perror("Fallo al abrir el fichero de logs\n");
		return 1;
	}
	fd_set conjunto;
	FD_ZERO(&conjunto);
	FD_SET(fd_fifo1,&conjunto);
	FD_SET(fd_fifo2,&conjunto);
	int max_fd = std::max(fd_fifo1,fd_fifo2);

	std::cout << "Escritores listos\n";
	int procesos_escritores = 2;

	while (procesos_escritores > 0){
		fd_set modificado;
		std::memcpy(&modificado,&conjunto,sizeof(modificado));

		int resultado = 0;
		do {
			resultado = select(max_fd + 1, &modificado, 0, 0, 0);
		} while ((resultado < 0) && (errno == EINTR));
		if (resultado < 0){
			perror("Error en select\n");
			return 1;
		}

		std::array <char,2048> buffer;
		if (FD_ISSET(fd_fifo1,&modificado)){
			ssize_t leidos = read(fd_fifo1,buffer.data(),buffer.size());
			if (leidos < 0){
				perror("Error en read fifo1");
				return 1;
			} else if (leidos == 0){
				FD_CLR(fd_fifo1,&conjunto);
				procesos_escritores--;
			} else {
				std::string_view sv(buffer.data(),leidos);
				fich_log << "Proceso 1: " << sv;
			}
		}
		if (FD_ISSET(fd_fifo2,&modificado)){
			ssize_t leidos = read(fd_fifo2,buffer.data(),buffer.size());
			if (leidos < 0){
				perror("Error en read fifo2");
				return 1;
			} else if (leidos == 0){
				FD_CLR(fd_fifo2,&conjunto);
				procesos_escritores--;
			} else {
				std::string_view sv(buffer.data(),leidos);
				fich_log << "Proceso 2: " << sv;
			}
		}
	}
	
	fich_log.close();
	return 0;
}