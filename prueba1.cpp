#include <iostream>
#include <unistd.h>
#include <fcntl.h>

int main(){
    int fd = open("../../mififo", O_WRONLY);
    if(fd < 0){
    perror("error en open");
    exit(1);
    }
    std::cout << "Fifo abierta con éxito!\n";
   
    std::string_view sv = "prueba total";
   
    ssize_t escritos = write(fd, sv.data(), sv.size());
    if(escritos < long(sv.size())){
        perror("error en write");
        return 1;
    }
    close(fd);
    return 0;
}