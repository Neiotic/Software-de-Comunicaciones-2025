/*
Ejercicio para que el padre le mande texto leido por pantalla a su hijo,
utilizando un canal de paquetes
*/
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

int main(){

    //se crea un canal con dos extremos
    auto [extremoX, extremoY] = crea_canal();

    //al hacer fork, el padre y el hijo comparten este canal abierto y sus descriptores
    pid_t pid = fork();
        std::string almacen;
    if(pid < 0){
        perror("imposible clonar!");
        return 1;
    }

    if(pid > 0){ //zona exclusiva del padre
        extremoY.cierra(); //siempre cierra el extremo que no uses

        bool fin = false;
        while(!fin){
            std::cout << "Introduzca un mensaje para enviar: \n";
            //1. POR HACER: lee un texto por teclado con std::getline()
            //si el texto no es "fin", envíalo por extremoX
            //si el texto es "fin", pon la variable fin a true
            getline(std::cin,almacen);
            if(almacen == "fin"){
                fin = true;
            }else{
                extremoX.envia(almacen.data(),almacen.size());
            }
        }

        extremoX.cierra(); //siempre cierra tu extremo antes de acabar 
        
        //2. POR HACER: cumple el contrato: espera a que acabe tu chaval
        wait(0);
    
    }else{ //zona exclusiva del hijo
        extremoX.cierra(); //siempre cierra el extremo que no uses

        std::array<char, 2048> paquete_recibido;
        ssize_t leidos;
        //3. POR HACER: en bucle, lee un paquete y muestra el texto por pantalla
        //(recuerda la condición de salida del bucle)
        do{
            leidos = extremoY.recibe(paquete_recibido.data(),paquete_recibido.size());
            std::string_view sv(paquete_recibido.data(),leidos);
            std::cout << "Paquete recibido: " << sv << std::endl;
        }while(leidos > 0);
        if (leidos < 0){
            perror("Error en lectura");
        }else{
            perror("Fin de la conexion con el emisor\n");
        }

        extremoY.cierra(); //siempre cierra tu extremo al acabar

        std::exit(0); //cumple el contrato
    }

    return 0; //aqui solo llega el padre (como debe ser)
}