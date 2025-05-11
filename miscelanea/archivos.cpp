#include <iostream>
#include <fstream>
#include <array>
#include <string>

using namespace std;


int main(int argc, char * argv[]){
    ifstream entrada;
    ofstream salida;
    string almacen1;
    string almacen2;

    entrada.open("prueba_entrada.txt");
    getline(entrada,almacen1,';');
    getline(entrada,almacen2,'\n');
    while(!entrada.eof()) {
        cout << "x:" <<almacen1 << endl;
        cout << "y:" << almacen2 << endl << endl;
        getline(entrada,almacen1,';');
        getline(entrada,almacen2,'\n');
    }
    cout << "x:" <<almacen1 << endl;
    cout << "y:" << almacen2 << endl << endl;

    entrada.close();
    return 0;
}