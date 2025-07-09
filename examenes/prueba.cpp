#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <csignal>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <set>

int main() {
    std::array<char,2048> lista = {'h', 'o', 'l', 'a'};

    std::string_view sv(lista.data(), 4);

    std::cout << memcmp(sv.data(), "hola", 4);

    return 0;
}