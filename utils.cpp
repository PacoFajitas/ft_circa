#include "utils.hpp"
#include "server.hpp"
#include "responses.hpp"
#include <iomanip> // Añade esta línea para usar std::setw y std::hex
#include <cstdlib>  // Para usar atoi


// Declaración de la lista de comandos válidos
std::set<std::string> initializeValidCommands() {
    std::set<std::string> commands;
    commands.insert("PASS");
    commands.insert("NICK");
    commands.insert("USER");
    commands.insert("JOIN");
    commands.insert("PART");
    commands.insert("QUIT");
    commands.insert("PRIVMSG");
    return commands;
}

const std::set<std::string> validCommands = initializeValidCommands();


bool	is_valid_number(const char* str) {
	while (*str) {
		if (!isdigit(*str)) {
			return (false);
		}
		str++;
	}
	return (true);
}

bool	is_valid_port(const char* argument) {
	int	port = atoi(argument);
	if (port < 1 || port > 65535)
		return (false);
	return (true);
}


// Funcion para convertir los \n a \r\n
std::string convertToCRLF(const std::string& input) {
    std::string result;
    result.reserve(input.size() + 2);  // Reservar espacio para evitar múltiples realocaciones
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\n') {
            if (i == 0 || input[i - 1] != '\r') {
                result += '\r';  // Añadir \r antes de \n si no está presente
            }
        }
        result += input[i];  // Añadir el carácter actual
    }
	// Verificar si el resultado termina con \r\n, si no, añadirlo
	if (result.empty() || result[result.size() - 1] != '\n' || (result.size() > 1 && result[result.size() - 2] != '\r')) {
		result += "\r\n";
    }
    return result;
}


// Función para dividir una cadena en comandos usando un delimitador
std::vector<std::string> splitString(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!token.empty()) {  // Ignora los tokens vacíos
            tokens.push_back(token);
        }
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    // Agregar la última parte si no está vacía
    std::string token = str.substr(start);
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}



/************************
*****************/

bool isUserOrChannelExists(const std::string& target) {
	(void)target; // Para evitar la advertencia de parámetro no usado
    // Lógica para verificar si el usuario o canal existe
    return true;
}

