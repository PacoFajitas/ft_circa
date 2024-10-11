#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "server.hpp"

bool    is_valid_number(const char* str);	// Para el main
bool    is_valid_port(const char* argument);

// Problema con la terminal, no usa \r\n, tenemos que hacer una conversion
std::string convertToCRLF(const std::string& input);

// Declaración de la función splitString
std::vector<std::string> splitString(const std::string& input, const std::string& delimiter);

//bool isNicknameInUse(const std::string& nickname);
bool isUserOrChannelExists(const std::string& target);


#endif // UTILS_HPP

