#include <iostream>
#include <fcntl.h>
#include <array>
#include <string_view>
#include <unistd.h>

int main(){
	std::cout << "Esperando escritor en la fifo sc_mensajes...\n";
	int fd_fifo = open("sc_mensajes",O_RDONLY);
	if(fd_fifo < 0){
		perror("Error en open\n");
		return 1;
	}
	std::cout << "Conectado con el otro usuario con exito!\n";
	fd_set conjunto;
	FD_ZERO(&conjunto);

	bool fin = false;
	while (!fin)
	{
		FD_SET(0,&conjunto);
		FD_SET(fd_fifo,&conjunto);
		timeval timeout = {3,0};
		int result;
		do {
			result = select(fd_fifo + 1, &conjunto, 0, 0, &timeout);
		} while ((result < 0) && (errno == EINTR));
		if (result < 0){
			perror("Error en select\n");
			return 1;
		} else if (result == 0){
			std::cout << ".\n";
		}
		if (FD_ISSET(0, &conjunto)){
			std::string mensaje;
			std::getline(std::cin,mensaje);
			std::cout << "Mensaje enviado: " << mensaje << std::endl;
		}
	
		if (FD_ISSET(fd_fifo, &conjunto))
		{
			std::array<char,2048> buffer;
			ssize_t leidos = read(fd_fifo,buffer.data(),buffer.size());
			if (leidos < 0){
				perror("Error en read\n");
				return 1;
			}else if (leidos > 0){
				std::string_view sv(buffer.data(),leidos);
				std::cout << "Recibido: " << sv;
			}else{
				close(fd_fifo);
				std::cout << "Cerrando enalce...\nEnlace cerrado!";
				fin = true;
			}
		}
	}

	return 0;
}