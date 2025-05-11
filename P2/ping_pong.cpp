#include <iostream>
#include <array>
#include <string_view>
#include <unistd.h>
#include <sys/socket.h>
#include <csignal>
#include <string_view>

class extremo_canal;
using par_extremos = std::pair<extremo_canal, extremo_canal>; //nuestro moderno typedef
par_extremos crea_canal();

class extremo_canal{
public:
    /* envio de bytes por el canal. 
    Argumentos:
        const void * donde : dirección de memoria donde empiezan los bytes a enviar
        size_t cuantos_bytes: numero de bytes consecutivos a mandar
    Retorna:
        ssize_t: (es un size_t con signo), porque puede devolver:
            -1: si hubo un error en el envío
            >0: son los bytes que realmente ha conseguido mandar por el canal 
    */
    ssize_t envia(const void * donde, size_t cuantos_bytes){
        return write(canal_id, donde, cuantos_bytes);
    }
    /* recepcion de bytes por el canal. 
    Argumentos:
        void * donde : dirección de memoria donde colocar los bytes que se reciban
        size_t maximo_de_bytes: máximo numero de bytes que caben en la memoria (o que se quieren recibir)
    Retorna:
        ssize_t: (es un size_t con signo), porque puede devolver:
            -1: si hubo un error en la recepción
			 0: si el extremo opuesto está cerrado
            >0: son los bytes que realmente se han recibido por el canal 
    */
    ssize_t recibe(void * donde, size_t maximo_de_bytes){
        return read(canal_id, donde, maximo_de_bytes);
    }
    /*cierra este extremo del canal*/
    void cierra(){
        close(canal_id);
    }

    //friend NO ENTRA EN EVALUACION DE SOFTWARE DE COMUNICACIONES, puedes ignorar esto
    friend par_extremos crea_canal();

private:
    int canal_id; //identificador del extremo de canal abierto (de cara al sistema operativo)
    //constructor privado para que solo se pueda instanciar desde la funcion crea_canal(), que es amiguilla
    extremo_canal(int id){
        canal_id = id;
    } 

};

//Función que permite crear un canal bidireccional gracias al sistema operativo
//NO TIENES POR QUÉ ENTENDER SU CONTENIDO AÚN, SOLO UTILÍZALA EN TU PROGRAMA
par_extremos crea_canal(){
    int ids_canal[2];
	//socketpair permite obtener dos sockets abiertos, validos para entrada y salida
	// y de uso local dentro de la máquina (PF_LOCAL como primer argumento)
	// SOCK_SEQPACKET solicita al sistema un servicio fiable, conectado y de intercambio completo de paquetes 
    int result = socketpair(PF_LOCAL, SOCK_SEQPACKET, 0, ids_canal);
    if(result < 0){ //error y ruina gorda
        throw std::system_error(errno, std::generic_category(), "error al crear el canal");
    }
    //si he llegado hasta aquí, en cada casilla del array ids_canal hay un identificador valido
    return std::make_pair(extremo_canal(ids_canal[0]),extremo_canal(ids_canal[1]));
}

//prototipos de funciones (adelantadas para que el compilador no se queje):
void transmite_ping(int);
void transmite_pong(int);


//ahora los extremos se han creado como variables globales!
auto [extremoX, extremoY] = crea_canal();


void transmite_ping(int num_sig){
    std::string sv = "ping"; //no hay copia
    ssize_t escritos = extremoX.envia(sv.data(), sv.size());
    if(escritos < 0){
        perror("extremoX.envia");
        std::exit(1);
    }
    signal(SIGUSR1, transmite_pong); //cambio a transmite_pong
}

void transmite_pong(int num_sig){
    std::string sv = "pong"; //no hay copia
    ssize_t escritos = extremoX.envia(sv.data(), sv.size());
    if(escritos < 0){
        perror("extremoX.envia");
        std::exit(1);
    }
    signal(SIGUSR1, transmite_ping); //cambio a transmite_ping
}

void cierra_extremoX(int num_sig){
    extremoX.cierra();
}

int main(){    
    
    signal(SIGUSR1, transmite_ping);
    signal(SIGTERM, cierra_extremoX);

    std::cout << "Este es mi PID: " << getpid() << std::endl;

    std::array<char, 2048> almacen;
    ssize_t leidos;
    do{
        leidos = extremoY.recibe(almacen.data(),2048);
        if(leidos > 0){
            std::string sv(almacen.data(), static_cast<size_t>(leidos));
            //lo que recibo NO es una cadena, pero std::cout "la ve" como si lo fuera
            std::cout << sv << std::endl;
        }
    }while(leidos > 0);

    return 0;
}