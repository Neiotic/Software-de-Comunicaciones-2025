#include <iostream>
#include <netinet/in.h>
#include <bit>


int main(){
	u_int16_t puerto = 6060; //Declaro mi puerto
	sockaddr_in vinculo = {}; //Declaro el socket

	if (std::endian::native == std::endian::little) { //Si soy little endian, swapeo la direccion del puerto
		vinculo.sin_port = std::byteswap(puerto);
	} else {
		vinculo.sin_port = puerto;
	}

	vinculo.sin_addr.s_addr = INADDR_ANY; //Es la IP 0.0.0.0

	int resultado = bind(sd,(sockaddr *)&vinculo, sizeof(vinculo));



	return 0;
}