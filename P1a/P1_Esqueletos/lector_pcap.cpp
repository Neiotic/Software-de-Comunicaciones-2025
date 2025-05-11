/**
 * Ejemplo con partes a completar. Abre un fichero pcap y muestra su contenido
 * (Busca y completa las partes etiquetadas como "relléname")
 */

#include <iostream>
#include <cstdint> 
#include <array>
#include <vector>
#include <fstream>
#include <iomanip> //para el horroroso formateo de hexadecimales con cout

struct global_hdr_t {
	uint32_t magic_number = 0xA1B2C3D4; /* magic number: valor fijo */
	uint16_t version_major = 2; /* número de versión mayor: valor fijo*/
	uint16_t version_minor = 4; /* número de versión menor: vaolr fijo */
	int32_t thiszone = 0; /* zona horaria sobre GMT (normalmente no se cambia) */
	uint32_t sigfigs = 0; /* precisión de los sellos de tiempo: valor fijo */
	uint32_t snaplen = 65535; /* tamaño máximo en bytes de una pdu capturada: valor fijo */
	uint32_t network = 1; /* tipo de enlace de datos: 1 es Ethernet (podría cambiarse)*/
};

struct packet_hdr_t {
	uint32_t ts_sec; /* sello de tiempo en segundos */
	uint32_t ts_usec; /* sello de tiempo en microsegundos */
	uint32_t incl_len; /* numero de bytes del paquete presentes en este fichero */
	uint32_t orig_len; /* longitud total en bytes del paquete */
};


class analiza_pcap_file{
    public:
        //constructor
        analiza_pcap_file(const std::string & nombre_fichero){
            fichero.open(nombre_fichero, std::ios::binary);
            if(fichero.fail()){
                throw; //error
            }
            global_hdr_t cabecera;
            
            
            //ATENCIÓN: CUIDADO CON LEER DATOS Y GUARDARLOS EN UNA ESTRUCTURA
            static_assert(sizeof(global_hdr_t)==24);
            //esta lectura solo es segura si sizeof(global_hdr_t) == 24 bytes
            fichero.read(/*relléname*/, /*relléname*/);
            //la lectura es completa (todos los bytes pedidos) o se considera fallo
            if(fichero.fail()){
                throw; //error
            }
            //PASO 1: comprobar el formato en el que estan los datos

            if(/*relléname*/ == 0xA1B2C3D4){
                //el ordenador que creó el fichero tiene mi misma arquitectura, asi que
                //todos los enteros de las cabeceras pcap se interpretan correctamente
                //sin necesidad de invertir sus valores
                necesita_swap = false;
                tipo_de_pdus = cabecera.network; //normalmente vale 1 (Ethernet)
            }else if(/*relléname*/ == 0xD4C3B2A1){
                //el ordenador que creó el fichero tiene una arquitectura diferente, así que
                //hay que darle la vuelta a los campos de las cabeceras antes de usarlos
                necesita_swap = true;
                tipo_de_pdus = std::byteswap(cabecera.network);
            }else{
                std::cout << "Formato de PCAP incorrecto\n";
                throw; //error
            }
        }

        bool lee_siguiente_pdu(){
            packet_hdr_t cabecera_pdu;

            //se lee la cabecera desde el fichero:
            //ATENCIÓN: CUIDADO CON LEER UNA ESTRUCTURA DESDE UN FICHERO
            static_assert(sizeof(packet_hdr_t)==16);      
            //esta lectura solo es segura si sizeof(packet_hdr_t) == 16 bytes 
            fichero.read(/*relléname*/, /*relléname*/);
            if(fichero.fail()){
                return false;
            }

            //Asigna a esta variable el campo de cabecera_pdu donde se indique
            //el tamaño de la pdu a leer a continuación
            size_t tam_pdu = /*relléname*/;
            
            std::vector<uint8_t> pdu(tam_pdu); //reservo tam_pdu bytes en el heap

            //ahora se lee la pdu del fichero:
            fichero.read/*relléname*/, /*relléname*/);
            if(fichero.fail()){
                return false;
            }

            //Ya tengo la PDU. Ahora tengo que decodificarla
            if(tipo_de_pdus == 1){ //las PDUs en este fichero son de tipo Ethernet
               decodifica_trama_ethernet(/*relléname*/);
            }else{
                std::cout << "No tengo un decodificador para esta PDU \n";
                throw;
            }

            return true;     
        }

        void decodifica_trama_ethernet(const std::vector<uint8_t> & pdu){
            if(pdu.size()< 14){
                std::cout << "Error: la cabecera Ethernet no tiene 14 bytes";
                throw; //error
            }
            std::cout << "\nTrama Ethernet:\n";
            //ejemplo de cómo mostrar un hexadecimal (byte) con printf:
            printf("MAC destino: %02X:%02X:%02X:%02X:%02X:%02X\n", pdu[0], pdu[1], pdu[2],
                                                       pdu[3], pdu[4], pdu[5]);
            printf("MAC origen: %02X:%02X:%02X:%02X:%02X:%02X\n", pdu[6], pdu[7], pdu[8],
                                                      pdu[9], pdu[10], pdu[11]);
            //tipo de paquete que encapsula
            if((pdu[12] == 0x08) && (pdu[13] == 0x00)){
                std::cout << "Tiene dentro un datagrama IP: \n";
            } //hay otros, pero lo dejamos ahí por ahora

            //ejemplo de cómo mostrar un hexadecimal (byte) con cout (qué horror)
            for(size_t i = 14; i < pdu.size(); i++){
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                          << (int)pdu[i] << " ";
            }
            //restaura la salida a formato decimal (ojú, qué paciencia hay que tener...)
            std::cout << std::dec;
        }

    private:
        std::ifstream fichero; //fichero que se abrirá para leer las pdus
        bool necesita_swap; //indica si los campos de las cabeceras pcap se tienen que invertir o no
        uint32_t tipo_de_pdus; //que contiene el fichero: Ethernet, IP, etc.
};


int main(){
    //Este main está completo, aunque estaría mejor si el nombre de fichero se pasara como argumento
    //al programa, porque depende de que exista el fichero "prueba.pcap"
    analiza_pcap_file fich("prueba.pcap");

    bool sigue;
    do{
        sigue = fich.lee_siguiente_pdu(); 
    }while(sigue);
}