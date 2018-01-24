# Запуск на Ubuntu 16.04

0. Установка зависимостей
```bash
sudo apt-get install libglew-dev libglfw3 libassimp-dev
```

1. Загрузка репозитория
```bash
git clone git@github.com:kravtsun/au-cg
cd au-cg
git fetch --all
git checkout hw02-object
```

2. Запуск сборки через CMake:
```bash
cd object
cmake . && make -j $(nproc)
```

3. Запуск приложения:
```bash
./object
```

