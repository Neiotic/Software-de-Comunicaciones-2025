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

int main(){

    //NO SE PERMITE DECLARAR VARIABLES DE TIPO extremo_canal, porque el constructor es PRIVADO a posta
    //solo se pueden obtener dos objetos de este tipo a través del uso de la función crea_canal():
    auto [extremoX, extremoY] = crea_canal();

    //Aqui se van a recibir todos los mensajes (reutilizando el almacen)
    std::array<uint8_t, 1500> almacen;
    //variable reutilizable para obtener los bytes recibidos
    ssize_t leidos;

    //Envio el texto de una cadena
    std::string cadena = "Hola amigos";
    extremoX.envia(cadena.data(),cadena.size());

    //Recibe los bytes que contienen el texto (ojo, no se envió el '\0')
    leidos = extremoY.recibe(almacen.data(), almacen.size());
    if(leidos > 0){
        //no he recibido una cadena, pero sí texto imprimible por pantalla
        std::cout << std::string_view((const char *)almacen.data(),(size_t)leidos) << std::endl;

    }

    //Envio de cuatro bytes de una pdu
    std::array<uint8_t, 5> pdu = {0x01,0x02, 0x03, 0xAF};
    extremoX.envia(pdu.data(), 4); //cuatro, no cinco, para evitar mandar basura

    //Recibe los bytes, en este caso se muestran en formato hexadecimal
    leidos = extremoY.recibe(almacen.data(), almacen.size());
    if(leidos > 0){
        //he recibido bytes, pero no es texto
        for(int i = 0; i < leidos; i++){
            printf("%x ", almacen[i]);
        }
        std::cout << std::endl;
    }

        //Envio de dos bytes de un array de C
        uint8_t pdu_c[1500] = {0xDE,0xAD};
        extremoX.envia(pdu_c, 2); //dos, no 1500, para evitar mandar basura
    
        //Recibe los bytes, en este caso se muestran en formato hexadecimal
        leidos = extremoY.recibe(almacen.data(), almacen.size());
        if(leidos > 0){
            //he recibido bytes, pero no es texto
            for(int i = 0; i < leidos; i++){
                printf("%x ", almacen[i]); //es que cout para los hexadecimales es una ruina!
            }
            std::cout << std::endl;
        }

        //Dos envios de texto consecutivos
        extremoX.envia(cadena.data(),cadena.size());
        std::string cadena2 = " de lo ajeno";
        extremoX.envia(cadena2.data(), cadena2.size());

        //Recibe ambos mensajes en el almacen
        leidos = extremoY.recibe(almacen.data(), almacen.size());
        if(leidos <= 0){
            std::cout << "Error: no se ha recibido nada\n";
            return 1;
        }
        size_t utiles = (size_t)leidos;
        //Ojo a la segunda lectura
        leidos = extremoY.recibe(almacen.data()+ leidos, almacen.size()-leidos);
        if(leidos <= 0){
            std::cout << "Error: no se ha recibido nada\n";
            return 1;
        }
        utiles +=leidos;
        //dentro del almacen no hay una cadena, pero sí texto imprimible por pantalla
        std::cout << std::string_view((const char *)almacen.data(),utiles) << std::endl;


        //Envío este simpático carácter
        char c = 'X';
        extremoX.envia(&c,1);

        //Lo guardo en un array de C
        uint8_t almacen_c[50];
        leidos = extremoY.recibe(almacen_c,50);
        if(leidos > 0){
            std::cout << almacen_c[0] << std::endl;
        }

    return 0;
}