#include <iostream>
#include <array>
#include <string_view>
#include <netinet/in.h>
#include <unistd.h>

int main() {
	int sd = socket(PF_INET, SOCK_STREAM, 0); //Devuelve un descriptor de fichero del canal bidireccional creado (socket descriptor)
	if (sd < 0) {
		perror("Error en socket");
		return 1;
	}
	sockaddr_in vinculo = {}; //crea el socket e inicializa a 0 sus parámetros

	vinculo.sin_family = AF_INET;
	if(std::endian::native == std::endian::little) {
		vinculo.sin_port = std::byteswap((uint16_t)5050);
	} else {
		vinculo.sin_port = 5050;
	}
	int resultadob = bind(sd,(sockaddr *)&vinculo,sizeof(vinculo));
	if (resultadob < 0) {
		perror("Error en bind");
		return 1;
	}

	int resultado = listen(sd,10); //TCP pasivo, escuchará a que los clientes le manden solicitudes
	if (resultado < 0) {
		perror("Error en listen");
		return 1;
	}

	sockaddr_in dir_cliente;
	socklen_t longitud_dir = sizeof(dir_cliente);
	int csd = accept(sd,(sockaddr*)&dir_cliente,&longitud_dir); //si no revuelve -1, está conectado con un cliente, con su id y puerto
	if(csd < 0) {
		perror("error en accept");
		return 1;
	}
	//servicio de eco
	ssize_t leidos;
	std::string_view sv = "Respuesta: ";
	std::array<char,2048> buffer;
	do
	{
		leidos = read(csd,buffer.data(),buffer.size());
		if(leidos > 0) {
			write(csd,sv.data(),sv.size());
			ssize_t escritosw = write(csd,buffer.data(),leidos);
			if (escritosw < 0) {
				perror("error en write");
				return 1;
			}
		} else if (leidos < 0) {
			perror("error en read");
			return 1;
		}
	} while (leidos > 0);
	
	close(csd); //cierra la conexion con el cliente
	close(sd); //cierra su propio socket
	return 0;
}