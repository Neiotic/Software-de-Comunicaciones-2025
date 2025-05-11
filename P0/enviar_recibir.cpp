#include <iostream>
#include <string_view>
#include <string>
#include <array>
#include <unistd.h>
#include <sys/socket.h>

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
            >0: son los bytes que realmente se han recibido por el canal 
    */
    ssize_t recibe(void * donde, size_t maximo_de_bytes){
        return read(canal_id, donde, maximo_de_bytes);
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

void imprime_almacen(std::array<uint8_t,1500> almacen, size_t size){
    if(size < almacen.size()){
        for(size_t i = 0; i < size ;i++){
            //std::cout << almacen[i];
            printf("%x",almacen[i]);
        }
    }
}

int main(){

    //NO SE PERMITE DECLARAR VARIABLES DE TIPO extremo_canal, porque el constructor es PRIVADO a posta
    //solo se pueden obtener dos objetos de este tipo a través del uso de la función crea_canal():
    auto [extremoX, extremoY] = crea_canal();


    std::array<uint8_t,1500> almacen;
    ssize_t confirmacion;

    std::string cadena = "Ni que fuera yo Dulcinea de Toloso";
    std::array <uint8_t, 5> pdu = {0x01,0x02,0x03,0xAF};

    confirmacion = extremoX.envia(pdu.data(),pdu.size()); //Envía la cadena
        if(confirmacion > 0){
            std::cout << "Cadena enviada correctamente\n";
        }else{
            std::cout << "Error de transmision\n";
        }

    confirmacion = extremoY.recibe(almacen.data(),almacen.size()); //La recibe y guarda en almacen
        if(confirmacion > 0){
            std::cout << "Recibida cadena.\n";
            std::cout << "Valor: \n";
            imprime_almacen(almacen, (size_t)confirmacion);
            std::cout << "\n";
        }else{
            std::cout << "Error en recepcion\n";
        }
/*
    //ejemplo de envío de un byte
    if(extremoX.envia("h",1)!=1){
        std::cout << "No se ha podido enviar!\n";
        return 1; //sale del programa
    }
    //ejemplo de recepción de un byte que se guarda en la pila
    char almacen;
	ssize_t leidos = extremoY.recibe(&almacen,1);
    if(leidos < 0){
        std::cout << "Hubo error en recepcion\n";
    }else{
        std::cout << "Recibido: " << almacen << std::endl;
    }
*/
    return 0;
}