#include <iostream>
#include <array>
#include <string_view>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

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
    /* recepcion de bytes por el canal (con tiempo de espera. 
    Argumentos:
        void * donde : dirección de memoria donde colocar los bytes que se reciban
        size_t maximo_de_bytes: máximo numero de bytes que caben en la memoria (o que se quieren recibir)
        timeval timeout: indica los segundos-microsegundos de espera sin leer datos antes de retornar
        Retorna:
        ssize_t: (es un size_t con signo), porque puede devolver:
            -1: si hubo un error en la recepción
             0: si el extremo opuesto está cerrado o no hay datos y ha expirado el tiempo (con errno==ETIMEDOUT)
            >0: son los bytes que realmente se han recibido por el canal 
    */
    ssize_t recibe(void * donde, size_t maximo_de_bytes, timeval timeout){
        fd_set conjunto;
        FD_ZERO(&conjunto);
        FD_SET(canal_id, &conjunto);
      
        int result;
        do{ 
            result = select(canal_id + 1, &conjunto, 0, 0, &timeout); 
        }while((result < 0) && (errno == EINTR));
        if (result == -1) {
            // Hubo un error
            return -1;
        }else if (result == 0) {
            // Ha pasado el tiempo
            errno = ETIMEDOUT; 
            return 0;
        } else{
            //Hay datos! A leer tocan
            return read(canal_id, donde, maximo_de_bytes);
        }
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

/**
 * Función que envia un mensaje y espera un ack del otro extremo. 
 * Retorna:
 *      true: si todo fue bien
 *      false: hubo error (si errno == ETIMEDOUT, implica que no llegó el ack) 
*/
bool envio_parada_y_espera(extremo_canal extremo, const std::string & mensaje){
    std::cout << "Quieres simular que este mensaje se pierde? [S/n]: ";
    std::string opcion;
    std::getline(std::cin,opcion);
    if(opcion.starts_with("S")){
        std::cout << "oops! mensaje perdido por el canal!\n";
    }else{
        ssize_t escritos = extremo.envia(mensaje.data(), mensaje.size());
        if(escritos != (ssize_t)mensaje.size()){
            perror("error en el envío del paquete");
            return false;
        }
    }
    //espera confirmacion del otro extremo (parada y espera)
    char ack;
    timeval timeout = {2,0};
    ssize_t leidos = extremo.recibe(&ack,1, timeout); //temporizador! espera 2 segundos
    if(leidos < 1){
        return false;
    }else{
        return true;
    }
}


int main(){

    auto [extremoX, extremoY] = crea_canal();

    //si hago fork, el padre y el hijo comparten este canal abierto y sus descriptores
    pid_t pid = fork();
    if(pid < 0){
        perror("imposible clonar!");
        return 1;
    }

    if(pid > 0){ //zona exclusiva del padre
        extremoY.cierra(); //siempre cierra el extremo que no uses

        bool fin = false;
        while(!fin){
            std::cout << "Introduzca un mensaje para enviar: \n";
            std::string mensaje;
            std::getline(std::cin,mensaje);
            if(mensaje!="fin"){
                bool resultado_envio;
                do{
                    resultado_envio = envio_parada_y_espera(extremoX,mensaje);
                }while((resultado_envio == false) && (errno == ETIMEDOUT));   
            }else{
                fin = true;
            }
        }

        extremoX.cierra(); //siempre cierra tu extremo antes de acabar 
        
        wait(0); //cumple el contrato: espera a que acabe su chaval
    
    }else{ //zona exclusiva del hijo
        extremoX.cierra(); //siempre cierra el extremo que no uses

        std::array<char, 2048> paquete_recibido;
        ssize_t leidos;
        do{
            //espera recibir un mensaje
            leidos = extremoY.recibe(paquete_recibido.data(), paquete_recibido.size());
            if(leidos > 0){
                //el paquete leido NO es una cadena, pero puedo hacer que lo parezca para std::cout
                std::string_view sv(paquete_recibido.data(),leidos); //no hay copia de datos
                std::cout << "Paquete recibido: " << sv << std::endl;
            }
            //confirma su recepción al otro extremo con un byte de ACK
            uint8_t ack = 1;
            extremoY.envia(&ack,1);

        }while(leidos > 0);
        if(leidos < 0){
            perror("error en read");
        }else{
            std::cout << "Fin de recepción (detectado cierre del otro extremo)\n";
        }

        extremoY.cierra(); //siempre cierra tu extremo al acabar

        std::exit(0); //cumple el contrato
    }

    return 0; //aqui solo llega el padre (como debe ser)
}