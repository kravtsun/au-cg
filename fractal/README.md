# Запуск на Ubuntu 16.04

0. Загрузка репозитория
```bash
git clone git@github.com:kravtsun/au-cg
cd au-cg
git fetch --all
git checkout hw01-fractal
```

1. Установка зависимостей
```bash
sudo apt-get install libglew-dev libglfw3
```

2. Загрузка подмодулей репозитория:
```bash
git submodule update --init --recursive --remote .
```

3. Запуск сборки через CMake:
```bash
cd fractal
cmake . && make $(nproc)
```

4. Запуск приложения:
```bash
./fractal
```
