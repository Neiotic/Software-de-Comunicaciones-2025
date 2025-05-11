#include <iostream>
#include <string_view>
#include <string>
#include <signal.h>

int main(int argc, char * argv[]){
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <argumento>" << std::endl;
        return 1; // Código de error
    }

    signal(SIGINT, SIG_IGN);
    sleep(std::stoi(argv[1]));
    std::cout << "ya" << std::endl;
    signal(SIGINT, SIG_DFL);
    while (1) {
        pause();
    }
    
    return 0;
}