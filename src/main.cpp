// main.cpp

#include <iostream>
#include <cstdlib> // Para std::atoi
#include "utils.hpp"
#include "server.hpp"
#include "client.hpp"
#include "modeCommand.hpp"


int main(int argc, char *argv[]) {
    // Comprobar los argumentos de línea de comandos
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    // Verificar que el puerto es un número válido y está en el rango correcto
    if (!is_valid_number(argv[1]) || !is_valid_port(argv[1])) {
        std::cout << "Error: Invalid port number." << std::endl;
        return 1;
    }

    // Convertir el puerto a entero
    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    // Crear la instancia del servidor usando el constructor por defecto
    Server server;
    server.configureServer(port, password); // Configurar el servidor
    server.run(); // Ejecutar el servidor

    return 0;
}

// Main chorra que normalmente estara comentado para probar funciones pequenas

// int main(int a, char **b)
// {
//     (void)a;
//     (void)b;
//     Client test(1);
//     std::vector<std::string> test2;
//     std::vector<std::string> test3;
//     test2.push_back("MODE");
//     test2.push_back("#a");
//     test2.push_back("+l");
//     test2.push_back("MODE");
//     test2.push_back("MODE");
//     test2.push_back("MODE");
//     test2.push_back("MODE");
//     test2.push_back("MODE");
//     test2.push_back("MODE");
//     test2.push_back("MODE");

//     test3 = getArgs(test2);
//     for (size_t i = 0; i < test3.size(); i++)
//     {
//         std::cout << test3[i] << std::endl;
//     }
// }
