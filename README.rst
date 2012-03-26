
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

   ``easy_install.exe docutils``

#. Выполните следующую команду в ``c:\Python\Scripts`` и читайте файл
   README.html:

   ``rst2html.py /путь/к/этому/файлу``

Ubuntu Linux
------------

#. Установите Python и docutils при помощи ``apt-get``:

   ``sudo apt-get install python python-docutils``

#. Выполните следующую команду и читайте файл README.html:

   ``rst2html /путь/к/этому/файлу``

Сборка
======

Необходимое программное обеспечение
-----------------------------------

- `CMake <http://www.cmake.org/>`_
- `Flex <http://gnuwin32.sourceforge.net/packages/flex.htm>`_
- `Bison <http://gnuwin32.sourceforge.net/packages/bison.htm>`_
- `Perl <http://strawberryperl.com/>`_
- `Boost <http://www.boost.org/users/download/>`_

Установка RMagick gem под Windows
---------------------------------

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


.. _Python: http://www.python.org/
