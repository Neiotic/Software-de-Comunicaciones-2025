/* Ejemplo que genera un fichero pcap que contiene pdus, compatible 
   con analizadores de protocolos (Wireshark, tcpdump...)

   Puedes comprobar si tu fichero generado (prueba.pcap, en el ejemplo) lo puede abrir
   (y lo entiende) la herramienta tcpdump, con este comando de consola:

   tcpdump -e -r prueba.pcap -v
*/
#include <cstdint> //para los tipos de enteros (uint8_t, uint32_t, etc.)
#include <array>
#include <fstream>

//Cabecera Global Header del paquete pcap (se utiliza una vez al principio del fichero)
struct global_hdr_t {
	uint32_t magic_number = 0xA1B2C3D4; /* magic number: valor fijo */
	uint16_t version_major = 2; /* número de versión mayor: valor fijo*/
	uint16_t version_minor = 4; /* número de versión menor: vaolr fijo */
	int32_t thiszone = 0; /* zona horaria sobre GMT (normalmente no se cambia) */
	uint32_t sigfigs = 0; /* precisión de los sellos de tiempo: valor fijo */
	uint32_t snaplen = 65535; /* tamaño máximo en bytes de una pdu capturada: valor fijo */
	uint32_t network = 1; /* tipo de enlace de datos: 1 es Ethernet (podría cambiarse)*/
};

//Cabecera Packer Header de cada (una por cada PDU a insertar en el fichero)
struct packet_hdr_t {
	uint32_t ts_sec; /* sello de tiempo en segundos */
	uint32_t ts_usec; /* sello de tiempo en microsegundos */
	uint32_t incl_len; /* numero de bytes del paquete presentes en este fichero */
	uint32_t orig_len; /* longitud total en bytes del paquete */
};

//Clase sencilla que crea un fichero pcap en el constructor y permite insertar PDUs
//con el método inserta_pdu()
class genera_pcap_file{
    public:
        //constructor
        genera_pcap_file(const std::string & nombre_fichero){
            fichero.open(nombre_fichero, std::ios::binary);
            if(fichero.fail()){
                throw; //error
            }
            //ESTA CABECERA GLOBAL DEL FICHERO TIENE VALORES POR DEFECTO 
            //QUE ESTÁN SACADOS DE LA DOCUMENTACION OFICIAL
            global_hdr_t cabecera; //sus campos están rellenos (mira la definición del tipo)
            
            //ATENCIÓN: CUIDADO AL ESCRIBIR UNA ESTRUCTURA EN UN FICHERO CON UN ÚNICO write()
            static_assert(sizeof(global_hdr_t)==24);
            //esta escritura solo es segura si sizeof(global_hdr_t) == 24 bytes
            fichero.write((char*)&cabecera, sizeof(cabecera));
            if(fichero.fail()){
                throw; //error
            }
	
        }
        //destructor
        ~genera_pcap_file(){
            if(fichero.is_open())
                fichero.close();
        }
        
        //escribe una PDU en el fichero pcap
        void inserta_pdu(void * pdu, size_t len_pdu){
            packet_hdr_t cabecera_pdu;
            //se rellena la cabecera
            cabecera_pdu.ts_sec = falso_contador_segundos++; //simulo que ha pasado 1 seg. más
            cabecera_pdu.ts_usec = 0; //no nos interesa el valor de este campo
            cabecera_pdu.incl_len = len_pdu;
            cabecera_pdu.orig_len = len_pdu;
            //se escribe la cabecera en el fichero:
            //ATENCIÓN: CUIDADO AL ESCRIBIR UNA ESTRUCTURA EN UN FICHERO CON UN ÚNICO write()
            static_assert(sizeof(packet_hdr_t)==16);      
            //esta escritura solo es segura si sizeof(packet_hdr_t) == 16 bytes 
            fichero.write((char *)&cabecera_pdu, sizeof(cabecera_pdu));
            if(fichero.fail()){
                throw; //error
            }
            //ahora por fin se escribe la pdu en el fichero:
            fichero.write((char *)pdu, len_pdu);
            if(fichero.fail()){
                throw; //error
            }
        }
        
        inline void close(){
            fichero.close();
        }

    private:
        std::ofstream fichero; //fichero que se abrirá/creará para escribir las pdus
        uint32_t falso_contador_segundos=0; //cada pdu insertada incrementa este valor
};


    

int main(){
    genera_pcap_file fich("prueba.pcap");
    
    std::array<uint8_t,1500> trama_Ethernet = {
        //Cabecera de trama Ethernet (14 bytes):
        0x00, 0x00, 0x0C, 0x11, 0x22, 0x33,  // MAC destino de la trama Ethernet
        0x00, 0x00, 0x0C, 0x44, 0x55, 0x66,  // MAC origen de la trama Ethernet
        0x08, 0x00,                          // Tipo de pdu que encapsula (un datagrama IPv4)
        //Payload de la trama Ethernet:
        // Incluye datos ficticios de una cabecera de datagrama IPv4 (20 bytes):                        
        0x45, 0x00, 0x00, 0x14, 
        0x00, 0x00, 0x00, 0x00, 
        0x40, 0x11, 0x7C, 0xD7, 
        0x7F, 0x00, 0x00, 0x01, //ip origen: 127.0.0.1
        0x7F, 0x00, 0x00, 0x01, //ip destino: 127.0.0.1
    };
    //OJO: Esta trama no está bien del todo! El payload minimo son 46 bytes (según el estándar)
    // y hemos metido solo 20 (datagrama IP)! Para que sea una trama válida real, 
    // el payload se tiene que rellenar con ceros hasta llegar a 46. Aquí no lo hacemos 
    // porque no es un paquete real que vaya a viajar por la red, solo tiene fines didácticos

    fich.inserta_pdu(trama_Ethernet.data(), 14 /*bytes cabecera Ehterhet*/ + 20 /*bytes datagrama IP*/);
    
    fich.close();

    return 0;
}