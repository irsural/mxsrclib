# mxsrclib

В проекте [mxsrclib](https://okr.irsural.ru/git/okr/mxsrclib) находится cmake
файл для сборки.

## Переменные

### Обязательные

- **CMAKE_CXX_COMPILER**

  C++ компилятор.

- **IRSCONFIG_DIR**

  Путь до директории, в которой находится файл **irsconfig.h**.

- **MXSRCLIB_ARCH**

  Имя директории с архитектурно зависимым кодом.

  Может принимать значение имени директории, находящейся в
  [этой директории](https://okr.irsural.ru/git/okr/mxsrclib/src/branch/master/arch).

- **MXSRCLIB_STM32CUBE_TARGET**

  Имя таргета с подключенными библиотеками
  [STM32CUBE](https://okr.irsural.ru/git/third_party_mirrors/STM32CubeF7)
  (проект для stm32f7).

  Эта переменная является обязательной только на сборке с архитектурой
  **arm_st_hal**.

### Необязательные

- **VERBOSE**

  Режим подробного вывода дополнительной информации.

  Значение по умолчанию: **True**

- **MXSRCLIB_UTF8**

  Конвертировать cp1251 заголовки mxsrclib в utf-8 (про конвертацию написано далее).

  Может быть **True** или **False**.

  Значение по умолчанию: **False**


## Подключение

Для использования mxsrclib проекта необходимо:

- Скопировать файл **irsconfig0.h** в клиентский проект, переименовать в **irsconfig.h**.

- Подключить поддиректорию с mxsrclib:

  ```add_subdirectory(${Путь до директории mxsrclib})```.

- Подключить библиотеку **mxsrclib** к клиентскому проекту с помощью
```target_link_libraries(${Имя клиентского таргета} mxsrclib)```.

- Добавить зависимость клиентского проекта от **mxsrclib**, чтобы убедиться, что
  **mxsrclib** соберется раньше, чем основной проект:

  ```add_dependencies(${Имя клиентского таргета} mxsrclib)```.

- Установить необходимые переменные для cmake.


### Пример подключения

CMake файл клиентского проекта:

```cmake
  # Подключение таргета cube для архитектурно зависимого кода mxsrclib
  add_subdirectory(stm32cubef7)
  # Подключение таргета mxsrclib
  add_subdirectory(mxsrclib)

  # Создание клиентского таргета
  add_executable(project_name ${SOURCES})
  # Подключение таргета mxsrclib к клиентскому таргету
  target_link_libraries(project_name PUBLIC mxsrclib)
  # Добавление зависимости клиентского таргета от таргета mxsrclib
  add_dependencies(project_name mxsrclib)
```

Команда сборки cmake:

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_MAKE_PROGRAM=ninja \
  -DCMAKE_C_COMPILER=arm-none-eabi-gcc \
  -DCMAKE_CXX_COMPILER=arm-none-eabi-g++ \
  -DMXSRCLIB_UTF8=True \
  -DMXSRCLIB_ARCH=arm_st_hal \
  -DMXSRCLIB_STM32CUBE_TARGET=stm32cubef7 \
  -G Ninja \
  -S ./project_folder \
  -B ./project_folder/cmake-build-release-cortex-m7-gcc
```


## Подключение к проектам с исходным кодом в кодировке utf-8

Все файлы **mxsrclib** имеют кодировку cp1251, поэтому **mxsrclib** нельзя подключать к
проектам в utf-8 без дополнительных действий.

Подключение к проектам в utf-8 выполняется так:

- перед сборкой исходники **mxsrclib** конвертируются в utf-8 с помощью iconv
- все сконвертированные header-ы помещаются в папку **utf8_headers**. Этим занимается
  **utf8_headers/CmakeLists.txt**.
- header-ы **utf8_headers** пробрасываются в клиентский проект с помощью ``target_include_directories(INTERFACE)``

Конвертирование выполняется автоматически при установке опции **MXSRCLIB_UTF8** в ``True``.
