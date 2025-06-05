#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <bit>
#include <string_view>

int main() {
	//Crea canal
	int sd = socket(PF_INET, SOCK_DGRAM, 0); //Devuelve un descriptor de fichero del canal bidireccional creado (socket descriptor)
	if (sd < 0) {
		perror("Error en socket");
		return 1;
	}
	uint16_t puerto = 6060;
	sockaddr_in vinculo = {}; //crea el socket e inicializa a 0 sus parámetros

	vinculo.sin_family = AF_INET;
	if(std::endian::native == std::endian::little) { //Si este sistema está en little endian
		vinculo.sin_port = std::byteswap(puerto); //Paso la dirección del puerto a big endian
	} else {
		vinculo.sin_port = puerto; //Ya está en big endian, así que lo asigno
	}

	int resultado = bind(sd, (sockaddr*)&vinculo, sizeof(vinculo)); //crea el enlace
	if (resultado < 0) {
		perror("Error en bind");
		return 1;
	}
	//envío de un eco por el loopback
	uint16_t puerto_destino = 5050;
	sockaddr_in dir_destino = {};

	dir_destino.sin_family = PF_INET;
	if (std::endian::native == std::endian::little) {
		dir_destino.sin_port = std::byteswap(puerto_destino);
	} else {
		dir_destino.sin_port = puerto_destino;
	}
	dir_destino.sin_addr.s_addr = inet_addr("127.0.0.1");

	std::string_view sv = "Hola eco!";
	ssize_t escritos = sendto(sd, sv.data(), sv.size(), 0, (struct sockaddr*) &dir_destino, sizeof(dir_destino));
	if(escritos < 0) {
		perror("error de sendto");
		return 1;
	}

	//recepción

	std::array<char,512> almacen;
	sockaddr_in dir_origen;
	socklen_t longitud_dir = sizeof(dir_origen);

	ssize_t leidos = recvfrom(sd, almacen.data(), almacen.size(), 0, (sockaddr*) &dir_origen, &longitud_dir);
	if(leidos < 0) {
		perror("Error recvfrom");
		return 1;
	}

	close(sd);
	std::cout << "Respuesta: " << std::string_view(almacen.data(), leidos) << "\n";
	
	return 0;
}
