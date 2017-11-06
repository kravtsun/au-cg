# Запуск на Ubuntu 16.04

1. Установка зависимостей
```bash
sudo apt-get install libglew-dev libglfw3
```

2. Загрузка подмодулей репозитория:
```bash
git submodule update --recursive --remote .
```

3. Распаковка архива с lgm в `3rdparty`:
cd 3rdparty
unzip 

3. Запуск сборки через CMake:
```bash
cmake . && make
```


# Запуск на Windows 10

1. Установка зависимостей:

    * GLEW - согласно [инструкциям на сайте](http://glew.sourceforge.net/install.html)

    * GLFW - распаковать архив `3rdparty/glfw-3.2.1.bin.WIN64.zip` в `3rdparty`

    * GLM - распаковать архив `3rdparty/glm-0.9.8.5.zip` в `3rdparty`.

    * AntTweakBar - распаковать архив `3rdparty/AntTweakBar_116.zip` в `3rdparty`.

2. Построить проект Visual Studio 2015.

