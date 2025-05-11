#include <iostream>
#include <array>
#include <vector>
#include <cstring> //para memcpy
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


//RELLENA LAS PARTES DE ESTA FUNCION QUE PERMITEN QUE Y RECIBA POR SU EXTREMO Y RESPONDA
void Y_recibe_y_envia_respuesta(extremo_canal & extremoY){
    std::array<uint8_t,1500> pdu_y;

    ssize_t leidos = extremoY.recibe(pdu_y.data(), /*relléname*/);
    if(leidos < 2){
        //esto es un error, la cabecera tiene siempre dos bytes 
        std::cout << "Y: recibido un mensaje con formato erróneo\n";
        std::exit(1); //esto es bastante fuerte, pero para este ejemplo nos vale
    }
    if(pdu_y[0]=='C'){ //solicitud de conexion
        //crea la respuesta reutilizando pdu_y:
        pdu_y[0] = /*relléname*/; pdu_y[1]+=1;
        extremoY.envia(pdu_y.data(),/*relléname*/);
    }else if(pdu_y[0]=='D'){ //vienen datos!
        if(leidos > 2){ //me aseguro que datos en el mensaje
            std::cout << std::string_view((char *)&pdu_y[/*relléname*/], /*relléname*/); //porque sé que tienen texto (en este ejemplo)
        }
        //crea la respuesta reutilizando pdu_y:
        pdu_y[0] = /*relléname*/; pdu_y[1]+=/*relléname*/;
        extremoY.envia(pdu_y.data(),/*relléname*/);
    }else if(pdu_y[0]=='F'){ //solicita cierre de conexion!
        //crea la respuesta reutilizando pdu_y:
        pdu_y[0] = /*relléname*/; pdu_y[1]+=/*relléname*/;
        extremoY.envia(pdu_y.data(),/*relléname*/);
    }else{
        std::cout << "Y: recibido un mensaje NO RECONOCIDO\n";
        std::exit(1); //otra vez a lo bruto...
    }
}


int main(){

    //NO SE PERMITE DECLARAR VARIABLES DE TIPO extremo_canal, porque el constructor es PRIVADO a posta
    //solo se pueden obtener dos objetos de este tipo a través del uso de la función crea_canal():
    auto [extremoX, extremoY] = crea_canal();

    std::array<uint8_t,1500> pdu_x; //array para montar mensajes de X a Y
    uint8_t seq_x = 0; //contador de numero de secuencia en los mensajes de X a Y
    //cada cadena en este vector debe ser enviada a Y en la fase de transferencia de datos:
    std::vector<std::string> datos = {"La cosa", " esta en", " la casa!"};

    
    //FASE 1: solicitud de conexión
    pdu_x = {/*relléname*/, /*relléname*/};
    seq_x = /*relléname*/; //incrementa la secuencia en 1
    extremoX.envia(/*relléname*/,/*relléname*/);
    Y_recibe_y_envia_respuesta(extremoY); 

    ssize_t leidos = extremoX.recibe(pdu_x.data(), /*relléname*/);
    if(leidos < 2 || pdu_x[0]!=/*relléname*/ || pdu_x[/*relléname*/] != seq_x){
        std::cout << "X: recibida respuesta NO esperada\n";
        return 0; //fin del programa
    }
    //FASE 2: transferencia de datos
    pdu_x[0] = /*relléname*/;
    for(auto & s : datos){
        pdu_x[/*relléname*/] = seq_x++;
        memcpy(/*relléname*/,/*relléname*/,/*relléname*/);
        extremoX.envia(pdu_x.data(),/*relléname*/);
        Y_recibe_y_envia_respuesta(extremoY); 
    }
    //FASE 3: fin de la conexion
    pdu_x[0] = /*relléname*/;
    pdu_x[/*relléname*/] = seq_x++;
    extremoX.envia(pdu_x.data(),/*relléname*/);
    Y_recibe_y_envia_respuesta(extremoY);

    return 0;
}