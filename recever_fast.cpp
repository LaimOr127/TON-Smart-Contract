#include <iostream>
#include <cstring>
#include <openssl/sha.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <array>

#define MAX_DATA_SIZE 127
#define MAX_REFS 4

struct Cell {
    std::array<uint8_t, MAX_DATA_SIZE> data;
    size_t data_size;
    std::array<Cell*, MAX_REFS> refs;
    uint8_t refs_count;

    Cell() : data_size(0), refs_count(0) {
        data.fill(0);
        refs.fill(nullptr);
    }
};

// Десериализация ячейки и её поддерева
size_t deserializeCell(uint8_t* buffer, size_t offset, Cell* cell) {
    cell->data_size = buffer[offset++];
    std::memcpy(cell->data.data(), buffer + offset, cell->data_size);
    offset += cell->data_size;
    cell->refs_count = buffer[offset++];

    for (uint8_t i = 0; i < cell->refs_count; ++i) {
        cell->refs[i] = new Cell();
        offset = deserializeCell(buffer, offset, cell->refs[i]);
    }

    return offset;
}

// Вычисление SHA-256 хэша
void sha256(const uint8_t* data, size_t len, uint8_t* output) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, len);
    SHA256_Final(output, &sha256);
}

// Основная функция приемника
int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Не удалось создать сокет." << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr{}, client_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Не удалось привязать сокет." << std::endl;
        close(server_fd);
        return -1;
    }

    listen(server_fd, 1);

    socklen_t addr_len = sizeof(client_addr);
    int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket < 0) {
        std::cerr << "Не удалось принять подключение." << std::endl;
        close(server_fd);
        return -1;
    }

    uint8_t buffer[2048], received_hash[SHA256_DIGEST_LENGTH];
    size_t received_len = recv(client_socket, buffer, sizeof(buffer), 0);
    recv(client_socket, received_hash, SHA256_DIGEST_LENGTH, 0);

    auto* root = new Cell();
    deserializeCell(buffer, 0, root);

    uint8_t calculated_hash[SHA256_DIGEST_LENGTH];
    sha256(buffer, received_len, calculated_hash);

    bool is_valid = memcmp(received_hash, calculated_hash, SHA256_DIGEST_LENGTH) == 0;
    std::cout << "Целостность данных: " << (is_valid ? "Проверена" : "Не проверена") << std::endl;

    delete root;
    close(client_socket);
    close(server_fd);
    return 0;
}
