#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <openssl/sha.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

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

// Основная функция приемника
int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        std::cerr << "Ошибка при создании сокета\n";
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к IP/порту
    if (bind(server_sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Ошибка привязки\n";
        close(server_sock);
        return 1;
    }

    listen(server_sock, 1);
    std::cout << "Ожидание соединения...\n";

    int client_sock = accept(server_sock, nullptr, nullptr);
    if (client_sock < 0) {
        std::cerr << "Ошибка подключения клиента\n";
        close(server_sock);
        return 1;
    }

    // Получение данных
    char buffer[4096];
    int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        std::cerr << "Ошибка приема данных\n";
        close(client_sock);
        close(server_sock);
        return 1;
    }

    buffer[bytes_received] = '\0';  // Завершение строки для правильного отображения
    std::string receivedData(buffer);

    std::cout << "Полученные данные: " << receivedData << "\n";

    std::string calculatedHash = sha256(receivedData);
    std::cout << "Вычисленный хэш: " << calculatedHash << "\n";

    close(client_sock);
    close(server_sock);

    return 0;
}
