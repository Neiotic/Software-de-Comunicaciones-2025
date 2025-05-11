#include <stdio.h>
#include <unistd.h>
#include <cstring>

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("ERROR. NUMERO DE PARAMETROS DE ENTRADA DEBE SER DOS (valor actual; %d)\n",argv);
        return 1;
    }
    printf("El valor de entrada es: %s\n",argv[1]);
    return 0;
}