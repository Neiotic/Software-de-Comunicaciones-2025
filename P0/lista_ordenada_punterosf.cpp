#include <iostream>
#include <array>
#include <list>
#include <string>

// Funciones de comparación que podemos usar con nuestra función de inserción ordenada
bool ascendente(int a, int b) {
    return a < b;  // Orden ascendente (menor a mayor)
}

bool descendente(int a, int b) {
    return a > b;  // Orden descendente (mayor a menor)
}

bool par_impar(int a, int b) {
    // Ordenamos primero los pares y luego los impares
    if (a % 2 == 0 && b % 2 != 0) return true;  // a es par, b es impar
    if (a % 2 != 0 && b % 2 == 0) return false; // a es impar, b es par
    return a < b; // Si ambos son pares o ambos impares, ordenamos ascendente
}

/* Función para insertar ordenadamente en una lista usando un puntero a función como criterio de ordenación
Argumentos:
    std::list<int> & lista : referencia a la lista donde se quiere insertar un nuevo valor 
    int valor: el valor a insertar
    comparador : variable de tipo puntero a una funcion de ordenacion como las definidas arriba   
    
*/
void inserta_ordenado(std::list<int>& lista, int valor, /*de que tipo es?*/comparador ) {
    // Caso más fácil: si la lista está vacía o el valor va detrás del elemento final (tras compararlo con él)
    if (lista.empty() || comparador(lista.back(), valor)) {
        lista.push_back(valor); //lo insertas al final
        return; //salgo de la función
    }
    // Busco la posición correcta iterando por la lista
    for (auto it = lista.begin(); it != lista.end(); ++it) {
        if (comparador(valor, *it)) {
            lista.insert(it, valor);
            return;
        }
    }
}

// Función para mostrar la lista por pantalla
void imprime_lista(const std::list<int> & lista){
    for (int valor : lista) {
        std::cout << valor << " ";
    }
    std::cout << std::endl;
}

int main() {
    // Creo tres listas para probar cada tipo de ordenación
    std::list<int> lista_ascendente;
    std::list<int> lista_descendente;
    std::list<int> lista_par_impar;
    
    // Ejemplo de valores a insertar en las listas
    std::array valores = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    
    // Inserto los valores en cada lista de forma ordenada usando diferentes criterios de comparacion
    for (int valor : valores) {
        inserta_ordenado(lista_ascendente, valor, ascendente);
        inserta_ordenado(lista_descendente, valor, descendente);
        inserta_ordenado(lista_par_impar, valor, par_impar);
    }
    
    // Muestra los resultados
    std::cout <<  "Lista ordenada de menor a mayor: ";
    imprime_lista(lista_ascendente); 
    std::cout <<  "Lista ordenada de mayor a menor: ";
    imprime_lista(lista_descendente);
    std::cout <<  "Lista ordenada (los pares primero): ";
    imprime_lista(lista_par_impar);
    
    return 0;
}