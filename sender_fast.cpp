#include <iostream>
#include <fstream>
#include <cstring>
#include <openssl/sha.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <array>

#define MAX_DATA_SIZE 127 // Максимальный размер данных в байтах (1023 бита / 8)
#define MAX_REFS 4

struct Cell {
    std::array<uint8_t, MAX_DATA_SIZE> data; // Данные ячейки
    size_t data_size; // Размер данных
    std::array<Cell*, MAX_REFS> refs; // Ссылки на дочерние ячейки
    uint8_t refs_count; // Количество ссылок

    Cell() : data_size(0), refs_count(0) {
        data.fill(0);
        refs.fill(nullptr);
    }
};

// Сериализация ячейки и её поддерева
size_t serializeCell(const Cell* cell, uint8_t* buffer, size_t offset) {
    buffer[offset++] = static_cast<uint8_t>(cell->data_size); // Размер данных
    std::memcpy(buffer + offset, cell->data.data(), cell->data_size); // Сериализация данных
    offset += cell->data_size;
    buffer[offset++] = cell->refs_count; // Количество ссылок

    for (uint8_t i = 0; i < cell->refs_count; ++i) {
        offset = serializeCell(cell->refs[i], buffer, offset); // Рекурсивная сериализация
    }
    return offset;
}

// Чтение входных данных и создание дерева ячеек
Cell* readAndBuildCellTree(const char* filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return nullptr;
    }

    auto* root = new Cell();
    size_t count = 0;
    infile >> count; // Чтение количества ячеек

    for (size_t i = 0; i < count; ++i) {
        auto* child = new Cell();
        infile >> child->data_size; // Чтение размера данных
        infile.read(reinterpret_cast<char*>(child->data.data()), child->data_size); // Чтение данных
        root->refs[root->refs_count++] = child; // Добавление дочерней ячейки
    }

    infile.close();
    return root;
}

// Вычисление SHA-256 хэша
void sha256(const uint8_t* data, size_t len, uint8_t* output) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, len);
    SHA256_Final(output, &sha256);
}

// Основная функция отправителя
int main() {
    const char* filename = "input.txt";
    Cell* root = readAndBuildCellTree(filename);

    if (!root) return -1;

    uint8_t buffer[2048]; // Увеличен размер буфера для данных и хэша
    size_t serialized_len = serializeCell(root, buffer, 0);

    uint8_t hash[SHA256_DIGEST_LENGTH];
    sha256(buffer, serialized_len, hash);

    // Отправка данных по сокету
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Не удалось создать сокет." << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Не удалось подключиться к серверу." << std::endl;
        close(sock);
        return -1;
    }

    send(sock, buffer, serialized_len, 0);
    send(sock, hash, SHA256_DIGEST_LENGTH, 0);

    close(sock);
    delete root;
    return 0;
}
