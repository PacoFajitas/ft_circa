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

template <typename T>
std::vector<T>concatenateVectors(std::vector<T>v1, std::vector<T>v2)
{
    if (v1.size() > 0 && v2.size() > 0)
        v1.insert(v1.end(), v2.begin(), v2.end());
    else if (v2.size() > 0)
        return (v2);
    return (v1);
}

//bool isNicknameInUse(const std::string& nickname);
bool isUserOrChannelExists(const std::string& target);

void printClientVector(std::vector<Client *> arg);

bool isStringNum(std::string str);

std::string joinTokens(const std::vector<std::string>& tokens, size_t start);

#endif // UTILS_HPP

