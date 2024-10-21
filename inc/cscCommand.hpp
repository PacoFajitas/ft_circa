#ifndef CSCCOMMAND_HPP
#define CSCCOMMAND_HPP

#include "client.hpp"
#include "server.hpp"
#include "fileTransfer.hpp"
#include <vector>
#include <string>
#include <iostream>


void    sendFileRequest(Client& client, const std::string& fileName, Server& server);

void	saveTransferredFile(const std::string& fileName, const std::vector<char>& data);

void	sendFileData(Client& sender, Client& recipient, const std::string& originalFileName, const std::string& newFilePath, Server& server);








#endif
