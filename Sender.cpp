#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <openssl/sha.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Функция чтения данных из файла
std::vector<std::string> readDataFromFile(const std::string& filename) {
    std::ifstream infile(filename);
    std::vector<std::string> data;
    std::string line;

    while (std::getline(infile, line)) {
        data.push_back(line);
    }

    return data;
}

// Функция вычисления SHA-256 хэша
std::string sha256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Основная функция отправителя
int main() {
    // Чтение данных из файла
    std::vector<std::string> data = readDataFromFile("input.txt");

    // Сериализация данных в одну строку
    std::string serializedData;
    for (const auto& line : data) {
        serializedData += line;
    }

    // Вычисление SHA-256 хэша
    std::string hash = sha256(serializedData);

    // Создание сокета
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Ошибка при создании сокета\n";
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Соединение с сервером
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Ошибка подключения\n";
        close(sock);
        return 1;
    }

    // Отправка данных
    send(sock, serializedData.c_str(), serializedData.size(), 0);
    send(sock, hash.c_str(), hash.size(), 0);

    close(sock);
    std::cout << "Данные и хэш отправлены.\n";
    return 0;
}
