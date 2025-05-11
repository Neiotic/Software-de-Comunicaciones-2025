#include <iostream>
#include <fstream>
#include <array>
#include <bit>


int main(){

    std::ofstream out;
    out.open("prueba3.bin", std::ios::binary);
    if(out.fail()){
        std::cout << "Imposible abrir/crear el fichero para escritura\n";
        return 1;
    }
    //cabecera del fichero con el formato nativo de representacion de bytes
    //en memoria del escritor
    uint16_t magic_number = 0xAABB;
    out.write((char *)&magic_number, 2);
    if(out.fail()){
        std::cout << "error en escritura\n";
    }

    std::array<uint16_t, 4> almacen1 = {1, 2, 3, 4};
    std::array<int8_t, 3> almacen2 = {5, -6, 7};
    std::array<uint32_t, 2> almacen3 = {8, 9};

    //ahora se escriben las secuencias de datos, con cabecera
    uint8_t cabecera = almacen1.size();
    out.write((char *)&cabecera, 1);
    if(out.fail()){
        std::cout << "error en escritura\n";
    }    
    out.write((char *)almacen1.data(), cabecera * sizeof(uint16_t)); //ojo
    if(out.fail()){
        std::cout << "error en escritura\n";
    }

    cabecera = almacen2.size();
    out.write((char *)&cabecera, 1);
    if(out.fail()){
        std::cout << "error en escritura\n";
    }    
    out.write((char *)almacen2.data(), cabecera * sizeof(uint8_t));
    if(out.fail()){
        std::cout << "error en escritura\n";
    }

    cabecera = almacen3.size();
    out.write((char *)&cabecera, 1);
    if(out.fail()){
        std::cout << "error en escritura\n";
    }    
    out.write((char *)almacen3.data(), cabecera * sizeof(uint32_t));
    if(out.fail()){
        std::cout << "error en escritura\n";
    }

    out.close();

    
    std::ifstream in("prueba3.bin", std::ios::binary);
    if(in.fail()){
        std::cout << "Imposible abrir el fichero para lectura\n";
        return 1;
    }
    //leo la cabecera con el magic number
    uint16_t magic_leido;
    //utilizo read porque SÉ LOS BYTES QUE TENGO QUE LEER
    in.read((char *)&magic_leido,2);
    if(in.fail()){
        std::cout << "error en lectura del magic number()\n";
    }

    bool necesita_swap;
    if(magic_leido == 0xAABB){ //escritor y lector comparten la misma arquitectura
        necesita_swap = false;
    }else if(magic_leido == 0xBBAA){ //tienen diferentes arquitecturas
        necesita_swap = true;
    }else{
        std::cout <<"Formato de fichero inválido\n";
        return 1; //salida del programa
    }

    //almacenes de recepcion
    std::array<uint16_t, 255> almacen1_r;
    std::array<int8_t, 255> almacen2_r;
    std::array<uint32_t, 255> almacen3_r;

    //utilizo read() porque sé el tamaño de bytes a leer
    //primero la cabecera
    uint8_t cabecera_r;
    in.read((char *)&cabecera_r,1);
    if(in.fail()){ std::cout << "Error de lectura"; return 1; }
    //luego la secuencia de valores
    in.read((char *)almacen1_r.data(),cabecera_r * sizeof(uint16_t)); //ojo
    if(in.fail()){ std::cout << "Error de lectura"; return 1; }
    std::cout << "Recibida secuencia de valores uint16_t:\n";
    for(size_t i=0; i < cabecera_r; i++){
        if(necesita_swap){
            almacen1_r[i] = std::byteswap(almacen1_r[i]); //le doy la vuelta antes de utilizarlo!
        }
        std::cout << almacen1_r[i] << " ";
    }

    //otra cabecera
    in.read((char *)&cabecera_r,1);
    if(in.fail()){ std::cout << "Error de lectura"; return 1; }
    //luego la secuencia de valores
    in.read((char *)almacen2_r.data(),cabecera_r * sizeof(int8_t)); //ojo
    if(in.fail()){ std::cout << "Error de lectura"; return 1; }
    std::cout << "\nRecibida secuencia de valores int8_t:\n";
    for(size_t i=0; i < cabecera_r; i++){
        std::cout << (int)almacen2_r[i] << " "; //A un byte NO se le da la vuelta!
        //el casting es porque cout ve el byte y lo intenta imprimir como char en ASCII (horror)
        //casi que mejor:  printf("%d ", almacen2_r[i]);
    }

    //otra cabecera
    in.read((char *)&cabecera_r,1);
    if(in.fail()){ std::cout << "Error de lectura"; return 1; }
    //luego la secuencia de valores
    in.read((char *)almacen3_r.data(),cabecera_r * sizeof(uint32_t)); //ojo
    if(in.fail()){ std::cout << "Error de lectura"; return 1; }
    std::cout << "\nRecibida secuencia de valores uint32_t:\n";
    for(size_t i=0; i < cabecera_r; i++){
        if(necesita_swap){
            almacen3_r[i] = std::byteswap(almacen3_r[i]); //le doy la vuelta antes de utilizarlo!
        }
        std::cout << almacen3_r[i] << " ";
    }


    in.close();

    return 0;
}