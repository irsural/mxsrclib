# mxsrclib

В проекте [mxsrclib](https://okr.irsural.ru/git/okr/mxsrclib) находится cmake
файл для сборки.

## Подключение

Для использования mxsrclib проекта необходимо:

- Скопировать файл **irsconfig0.h**
  в клиентский проект, переименовать в **irsconfig.h**, при необходимости редактировать.

- Подключить поддиректорию с mxsrclib.
  Сделать это можно с помощью
  ```add_subdirectory(${Путь до директории mxsrclib})```.

- Подключить библиотеку **mxsrclib** к клиентскому проекту с помощью
```target_link_libraries(${Имя клиентского таргета} mxsrclib)```.

- Добавить зависимость клиентского проекта от **mxsrclib**, чтобы убедиться, что
  **mxsrclib** соберется раньше, чем основной проект.

  Сделать это можно с помощью
  ```add_dependencies(${Имя клиентского таргета} mxsrclib)```.

- При сборке для микроконтроллеров добавить необходимые define'ы с помощью
  ```add_definitions```.

- Установить необходимые переменные для cmake.

### Пример подключения

CMake файл клиентского проекта:

```cmake
  # Подключение таргета cube для архитектурно зависимого кода mxsrclib
  add_subdirectory(stm32cubef7)
  # Подключение таргета mxsrclib
  add_subdirectory(mxsrclib)

  # Создание клиентского таргета
  add_executable(noise_generator ${SOURCES})
  # Подключение таргета mxsrclib к клиентскому таргету
  target_link_libraries(noise_generator PUBLIC mxsrclib)
  # Добавление зависимости клиентского таргета от таргета mxsrclib
  add_dependencies(noise_generator mxsrclib)
```

Команда сборки cmake:

```console
  /opt/clion-2023.3.4/bin/cmake/linux/x64/bin/cmake -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_MAKE_PROGRAM=/opt/clion-2023.3.4/bin/ninja/linux/x64/ninja \
  -DCMAKE_C_COMPILER=arm-none-eabi-gcc -DCMAKE_CXX_COMPILER=arm-none-eabi-g++ \
  -G Ninja -DMXSRCLIB_UTF8=True -DMXSRCLIB_ARCH=arm_st_hal \
  -DMXSRCLIB_STM32CUBE_TARGET=stm32cubef7 -S /home/u516/repos/noise_generator \
  -B /home/u516/repos/noise_generator/cmake-build-release-cortex-m7-gcc
```

## Переменные

### Обязательные

- **CMAKE_CXX_COMPILER**

  C++ компилятор.

- **IRSCONFIG_DIR**

  Путь до директории, в которой находится файл **irsconfig.h**.

- **MXSRCLIB_ARCH**

  Имя директории с архитектурно зависимым кодом.

  Может принимать значение имени директории, находящейся в [этой директории
  ](https://okr.irsural.ru/git/okr/mxsrclib/src/branch/master/arch).

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

- **CONVERT_HEADERS**

  Конвертировать cp1251 заголовки mxsrclib в utf-8 (про конвертацию написано далее).

  Может быть **True** или **False**.

  Значение по умолчанию: **False**

## Конвертация

В директории **utf8_headers** находится cmake файл с конвертацией заголовков
проекта [mxsrclib](https://okr.irsural.ru/git/okr/mxsrclib).

Изначально **mxsrclib** собирается в кодировке cp1251.

Если клиентский проект собирается в utf-8, то необходимо сконвертировать библиотеку
**mxsrclib** в utf-8.

Сделать это можно задав переменной **CONVERT_HEADERS** значение **True**.

Сконвертированные заголовки сохраняются в директории **utf8_headers**.

При конвертации архитектурно зависимого кода, сконвертированные заголовки
сохранятся в директории **utf8_headers**, где
будет создана папка **arch** с выбранной директорией внутри.
