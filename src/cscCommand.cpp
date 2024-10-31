#include "cscCommand.hpp"
#include "fileTransfer.hpp"
#include "responses.hpp"
#include <fstream>
#include <sstream>

void	sendFileRequest(Client& client, const std::string& fileName, Server& server) {
    std::string message = "FILE_SEND_REQUEST " + fileName;
    server.sendResponse(client.getSocketFD(), message);
}

void saveTransferredFile(const std::string& fileName, const std::vector<char>& data) {
    // Aquí se usa el nombre exacto del archivo sin añadir "_transf"
    std::ofstream outFile(fileName.c_str(), std::ios::binary);
    if (!outFile) {
        std::cout << "Error: Could not create file: " << fileName << std::endl;
        return;
    }
    outFile.write(data.data(), data.size());
    outFile.close();
    std::cout << "File saved as: " << fileName << std::endl;
}

// Función para enviar bloques de datos del archivo al destinatario
void sendFileData(Client& sender, Client& recipient, const std::string& originalFileName, const std::string& newFilePath, Server& server) {
    const size_t bufferSize = 1024;
    std::ifstream file(originalFileName.c_str(), std::ios::binary);
    if (!file) {
        server.sendResponse(sender.getSocketFD(), "Error: Could not open file: " + originalFileName);
        return;
    }
    char buffer[bufferSize];
    std::vector<char> fileData;
    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
        fileData.insert(fileData.end(), buffer, buffer + file.gcount());
    }
    file.close();
    saveTransferredFile(newFilePath, fileData);
    server.sendResponse(sender.getSocketFD(), ":File transfer completed.");
    server.sendResponse(recipient.getSocketFD(), ":File received successfully.");
}



