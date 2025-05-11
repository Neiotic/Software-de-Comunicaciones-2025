#include <iostream>
#include <fstream>
#include <array>

struct A{   
    uint16_t x; 
    uint8_t y; 
    uint16_t z; 
};

struct B{   
    uint16_t x; 
    uint16_t y; 
    uint16_t z; 
};

int main(){

    std::ofstream out;
    out.open("prueba4.bin", std::ios::binary);
    if(out.fail()){
        std::cout << "Imposible abrir/crear el fichero para escritura\n";
        return 1;
    }
    A a = {1, 2, 3 };
    //out.write( (char *) &a, sizeof(A)); //NO ES CORRECTO PORQUE:
    //static_assert(sizeof(A) == 5); //NO COMPILA!
    //hay que hacerlo campo a campo
    out.write( (char *) &a.x, sizeof(a.x)); //CORRECTO!
    if(out.fail()){
        std::cout << "Error al escribir el contenido del campo A.x\n";
    }
    out.write( (char *) &a.y, sizeof(a.y)); //CORRECTO!
    if(out.fail()){
        std::cout << "Error al escribir el contenido del campo A.y\n";
    }
    out.write( (char *) &a.z, sizeof(a.z)); //CORRECTO!
    if(out.fail()){
        std::cout << "Error al escribir el contenido del campo A.z\n";
    }
    
    
    B b = { 1 ,2, 3 };
    static_assert(sizeof(B) == 6);
    out.write( (char *) &b, sizeof(B)); //CORRECTO!
    if(out.fail()){
        std::cout << "Error al escribir el contenido de b\n";
    }

    out.close();

    //AQUI FALTA LA LECTURA, PERO ENTIENDO QUE NO TIENE DIFICULTAD
    //Para leer del fichero a un objeto A a_r, hay que hacerlo campo a campo
    //Para leer del fichero a un objeto B b_r, se puede leer en un solo write() si al programa
    //lector le ha compilado: static_assert(sizeof(B), 6); 

    return 0;
}