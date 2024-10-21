#include "fileTransfer.hpp"

FileTransfer::FileTransfer(Client* sender, Client* recipient, const std::string& fileName, std::streamsize fileSize)
    : sender(sender), recipient(recipient), fileName(fileName), fileSize(fileSize), fileStream(fileName.c_str(), std::ios::binary) {}

bool	FileTransfer::isFileOpen() const {
	return (fileStream.is_open());
}

Client* FileTransfer::getSender() const {
	return (sender);
}

Client* FileTransfer::getRecipient() const {
	return (recipient);
}

const std::string& FileTransfer::getFileName() const {
	return (fileName);
}

std::streamsize FileTransfer::getFileSize() const {
	return (fileSize);
}

std::ifstream& FileTransfer::getFileStream() {
	return (fileStream);
}
