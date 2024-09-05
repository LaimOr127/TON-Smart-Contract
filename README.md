# TON Smart Contract Communication Simulation

## Описание

Этот проект представляет собой модель обмена сообщениями между двумя смарт-контрактами в сети **TON** (The Open Network). Он включает две программы на языке C++, которые взаимодействуют между собой по сокету:

1. **Sender**: читает входные данные из файла, сериализует их, вычисляет хэш SHA-256 и отправляет данные и хэш по сокету.
2. **Receiver**: принимает данные по сокету, десериализует их, вычисляет хэш SHA-256 и проверяет целостность данных.

### Используемые технологии

- **C++**: основной язык программирования проекта.
- **Сокеты**: для организации сетевого взаимодействия между программами.
- **OpenSSL**: для вычисления хэш-функции SHA-256.

## Структура проекта
```
├── sender.cpp # Код программы отправителя (Sender) 
├── receiver.cpp # Код программы приемника (Receiver) 
├── input.txt # Файл с входными данными 
└── README.md # Описание проекта
```

## Требования

Для сборки и запуска проекта вам потребуется:

- Компилятор C++ (например, `g++`)
- Библиотека OpenSSL для поддержки криптографических функций


А также создать файл **input.txt** с вводными данными

```
1
[0x0000000000000000000000000000000000000000000000000000000000000001,
0x0000000000000000000000000000000000000000000000000000000000000003,
0x0000000000000000000000000000000000000000000000000000000000000060,
0x0000000000000000000000000000000000000000000000000000000000000003,
0x0000000000000000000000004ad3c3699488e4bf20154061ae58c4cf2cb46033,
0x000000000000000000000000188eb3274800a1cd4d7d225fdea841cb0087b9da,
0x000000000000000000000000308f4fea6ac39d45f5e3fea083fc72eb80cd92a2]
[0x0000000000000000000000000000000000000000000000000000000000000001,
0x0000000000000000000000000000000000000000000000000000000000000060,
0x00000000000000000000000000000000000000000000000000000000000000c0,
0x0000000000000000000000000000000000000000000000000000000000000002,
0x00000000000000000000000098bde60231da0807bcb2cf153dc97f1d104bf517,
0x00000000000000000000000089049ac1497e4a79d9d062be086549396172455b,
0x0000000000000000000000000000000000000000000000000000000000000001,
0x0000000000000000000000004ad3c3699488e4bf20154061ae58c4cf2cb46033]
```
# TON-Smart-Contract
