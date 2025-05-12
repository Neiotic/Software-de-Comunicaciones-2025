#include <iostream>
#include <fcntl.h>

int main(){
	std::cout << "Esperando escritor en la fifo sc_mensajes...\n";
	int fd_fifo = open("sc_fifo",O_RDONLY);
	if(fd_fifo < 0){
		perror("Error en open\n");
		return 1;
	}
	std::cout << "Conectado con el otro usuario con exito!\n";
	fd_set conjunto;
	FD_ZERO(&conjunto);
	timeval timeout = {3,0};
	int result;
	do{
		result = select(fd_fifo + 1, &conjunto, 0, 0, &timeout);
	}while((result < 0) && (errno == EINTR));
	if (result < 0){
		perror("Error en select\n");
		return 1;
	}else if(result == 0){
		std::cout << ".\n";
	}

	return 0;
}