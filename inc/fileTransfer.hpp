#ifndef FILETRANSFER_HPP
#define FILETRANSFER_HPP

#include <string>
#include <fstream>
#include "client.hpp"

class FileTransfer {
private:
    Client* sender;
    Client* recipient;
    std::string fileName;
    std::streamsize fileSize;
    std::ifstream fileStream;

    // Para evitar copias no deseadas (C++98)
    FileTransfer(const FileTransfer&); // Constructor de copia declarado pero no implementado
    FileTransfer& operator=(const FileTransfer&); // Operador de asignación declarado pero no implementado

public:
    FileTransfer(Client* sender, Client* recipient, const std::string& fileName, std::streamsize fileSize);

    bool	isFileOpen() const;
    Client* getSender() const;
    Client* getRecipient() const;
    const std::string& getFileName() const;
    std::streamsize getFileSize() const;
    std::ifstream& getFileStream();
};

#endif

