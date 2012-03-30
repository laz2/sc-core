
==================
 Проект "SC-ядро"
==================

.. contents::

Чтение этого файла в формате HTML
=================================

Этот файл в приемлимом формате можно читать прямо с `github'a
<https://github.com/laz2/sc-core>`_. Однако, если вы хотите читать его
в HTML c локального компьютера, то этот раздел для вас.

Файл написан в формате ``reStructured Text``, и для его компиляции в
HTML необходимо будет установить пакет Python'a ``docutils``. Будем
рассматривать установку этого добра под ОС Windows и Ubuntu Linux.

Windows
-------

#. Установите Python_ (наверное, версия 2.7 вполне подойдет)

#. Скачайте и установите при помощи exe-инсталлятора
   `setuptools <http://pypi.python.org/pypi/setuptools>`_

#. Запустите cmd в директории ``c:\Python\Scripts`` и установите пакет
   ``docutils``:

   easy_install.exe docutils

#. Выполните следующую команду в ``c:\Python\Scripts`` и читайте файл
   README.html::

    rst2html.py /путь/к/этому/файлу

Ubuntu Linux
------------

#. Установите Python и docutils при помощи ``apt-get``::

    sudo apt-get install python python-docutils

#. Выполните следующую команду и читайте файл README.html::

    rst2html /путь/к/этому/файлу

Описание
========

Модуль sc-core – это набор библиотек и программ, которые составляют
ядро обработки sc-текстов. В него входят следующие динамические
библиотеки:

- libsys : библиотека, обеспечивающая независимость от операционной системы;
- libtgf : библиотека обработки формата TGF (Transfer Graph Format);
- libsc : библиотека моделирования sc-памяти;
- libpm : библиотека процессорного модуля для обработки sc-текстов
  (например, она включает scp-интерпретатор и навигационно-поисковую
  машину);
- librgp : библиотека удаленного подключения к sc-памяти по протоколу
  RGP (Remote Graph Protocol).

Также модуль sc-core включает следующие программы:

- tools/pmc : средство запуска процессорного модуля из консоли;
- tools/dumptgf : утилита для просмотра TGF-файлов в человеко-читаемой форме;
- tools/scs2tgf : транслятор sc.s-текстов в формат TGF.


Сборка
======

Необходимое программное обеспечение
-----------------------------------

- Компилятор C/C++
- CMake_ (>= 2.8.0)
- Flex_ (>= 2.5.4a) и Bison_ (>= 2.4.1)
- StrawberryPerl_ (>= 5.10.1.4) или другую реализацию Perl'а
- Boost_ (>= 1.46.1)

Компилятор С/С++
~~~~~~~~~~~~~~~~

Под Windows можно использовать Visual Studio (>= 7). Если вы хотите
использовать MinGW, то тогда придется собрать STL (STLport)
вручную. Это необходимо, потому что стандартная STL MinGW не содержит
реализацию строк с поддержки юникода, а для сборки Boost'а это
является важным.

Если вы собираете проект под Ubuntu Linux, то проблем с компилятором у
вас возникнуть не должно.

СMake
~~~~~

Под Ubuntu Linux установка очень проста::

 sudo apt-get install cmake

Под Windows придется скачать с сайта CMake_ инсталлятор. Когда будете
ставить, то укажите чтобы инсталлятор добавил в переменную среды
окружения ``PATH`` путь к директории исполняемых файлов (по умолчанию
инсталлятор этого не делает).

.. warning:: Если у вас руссифицированная Windows, то возможно вам
   придется скопировать некоторые файлы вручную. CMake по какой-то
   причине не сможет их скопировать в директорию ``Мои Документы``. О
   том, что именно cmake не может скопировать, вы увидите сообщение
   при первом запуске.

Flex & Bison
~~~~~~~~~~~~

Под Ubuntu Linux установка очень проста::

 sudo apt-get install flex bison

Под Windows скачайте и запустите инсталляторы Flex_ и Bison_.

.. warning:: Ни в коем случае не ставьте эти программы в директорию, в
   пути к которой есть пробелы. По умолчанию они ставятся в ``Program
   Files``, что вас совсем не будет устраивать.

Программы ставятся по умолчанию в одну директорию с именем
``GnuWin32``. В дальнейшем я буду считать, что вы установили их в
директорию ``c:\GnuWin32``.

Perl
~~~~

Под Ubuntu Linux установка очень проста::

 sudo apt-get install perl

Под Windows скачайте и запустите инсталлятор StrawberryPerl_.

Boost
~~~~~

В Ubuntu Linux вы можете ставить Boost из пакетов::

 sudo apt-get install libboost-dev

Однако, чтобы получить самую свежую версию, необходимо собирать Boost
самостоятельно. Дальше будет объяснено, как собрать Boost только под
Windows, а походу будут указаны особенности сборки под Linux. Перейдем
к сборке Boost'а.

Скачайте zip-архив исходных текстов набора библиотек с сайт Boost_ и
разархивируйте его в ``c:\boost``. Перейдите в эту директорию,
запустите командную строку, а в ней::

 bootstrap.bat
 bjam

.. warning:: Для своей сборки Boost требует порядка 5-ти гигабайт
   дискового пространства и много времени.

.. tip::

   По умолчанию для сборки будет использована наиболее новая версия
   Visual Studio. Если вы хотите собрать Boost с использованием
   конкретной версии Visual Studio, то откройте в текстовом редакторе
   файл ``c:\boost\tools\build\v2\user-config.jam`` и в секции ``MSVC
   Configuration`` явно укажите версию компилятора.

Установка RMagick gem под Windows
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. Установить `Ruby <http://rubyinstaller.org/downloads/>`_::

    c:\Ruby

#. Установить `DevKit <http://rubyinstaller.org/downloads/>`_::

    c:\DevKit

#. Установить `ImageMagick <http://www.imagemagick.org/>`_ вместе с
   заголовочными файлами C++::

    c:\ImageMagick

#. Запустить cmd и установить переменные окружения::

    set PATH=c:\Ruby\bin;c:\ImageMagick
    set CPATH=c:\ImageMagick\include
    set LIBRARY_PATH=c:\ImageMagick\lib

#. Настроить DevKit::

    cd c:\DevKit
    ruby dk.rb init
    ruby dk.rb install
    devkitvars.bat

#. Установить RMagick::

    gem install rmagick

Сборка проекта
--------------

Переходите в директорию с исходными текстами и запускайте консоль:

#. Создадим директорию для сборки::

    mkdir build
    cd build

#. Сгенерируем файлы сборки при помощи ``cmake``::

    cmake -DSC_CORE_BUILD_EXAMPLES=1 -DSC_CORE_BUILD_TOOLS=1 -DSC_CORE_BUILD_DOCS=1 -DCMAKE_INSTALL_PREFIX="./sc-core" ..

.. Tip:: Под Linux по умолчанию создаются makefile'ы для gcc, и это
   нас устраивает. А вот Windows cmake по умолчанию будет создавать
   проектные файлы для наиболее новой версии Visual Studio. Если вы
   хотите указать явно версию Visual Studio, то это можно сделать явно
   указав генератор через опцию ``-G``. Например::

    cmake -G "Visual Studio 9 2008" ...

   Полный список генераторов можно узнать, выполнив команду ``cmake --help``.

.. Tip:: Переменная ``CMAKE_INSTALL_PREFIX`` задает директорию, в
   которую будет производиться установка собранного проекта.

#. Теперь осуществим сборку и установку:

   - Makefile'ы::

      make
      make install

   - Visual Studio 9.0::

      call "c:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat"

      devenv "sc-core.sln" /build "Debug" /project "ALL_BUILD"
      devenv "sc-core.sln" /build "RelWithDebInfo" /project "ALL_BUILD"

      devenv "sc-core.sln" /build "Debug" /project "INSTALL"
      devenv "sc-core.sln" /build "RelWithDebInfo" /project "INSTALL"

   - Visual Studio 10.0::

      call "c:\Program Files\Microsoft Visual Studio 10.0\Common7\Tools\vsvars32.bat"

      devenv "sc-core.sln" /build "Debug" /project "ALL_BUILD"
      devenv "sc-core.sln" /build "RelWithDebInfo" /project "ALL_BUILD"

      devenv "sc-core.sln" /build "Debug" /project "INSTALL"
      devenv "sc-core.sln" /build "RelWithDebInfo" /project "INSTALL"

.. _Python: http://www.python.org/
.. _CMake:  http://www.cmake.org/
.. _Flex:   http://gnuwin32.sourceforge.net/packages/flex.htm>
.. _Bison:  http://gnuwin32.sourceforge.net/packages/bison.htm>
.. _StrawberryPerl: http://strawberryperl.com/
.. _RubyInstaller: http://rubyinstaller.org/downloads/
.. _Boost:  http://www.boost.org/
