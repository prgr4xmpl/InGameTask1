## InGameTask1
Решение тестовой задачи №1 от InGame

![](https://github.com/prgr4xmpl/InGameTask1/blob/master/resources/demonstration.gif)

### Использование
Релизную версию можно скачать по [ccылке](https://github.com/prgr4xmpl/InGameTask1/releases/latest) или найти в **Releases**.

После скачивания архива, его можно распаковать в любую папку и запустить **MoonAndEarth.exe**

Используйте WASD для управления камерой, ползунки для изменения параметров орбиты Луны.

### О решении
В репозитории находится исходный код для сборки проекта, а также необходимые текстуры и иконка.
Для решения был использован OpenGL для отрисовки графики и QT6 для создания пользовательского интерфейса.

### Сборка
```
qmake.exe EarthMoon.pro -spec win32-g++ "CONFIG+=qtquickcompiler" && mingw32-make.exe qmake_all
```

