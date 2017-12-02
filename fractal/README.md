# Запуск на Ubuntu 16.04

1. Установка зависимостей
```bash
sudo apt-get install libglew-dev libglfw3
```

2. Загрузка подмодулей репозитория:
```bash
git submodule update --recursive --remote .
```

3. Запуск сборки через CMake:
```bash
cmake . && make
```
