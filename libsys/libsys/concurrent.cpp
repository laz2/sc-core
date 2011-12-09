
/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "concurrent.h"

#ifdef WIN32

#include <Windows.h>

SimpleLock::SimpleLock(bool isOwner /* = false */)
{
    mutex = CreateMutex(NULL, isOwner, NULL);
}

SimpleLock::~SimpleLock() 
{
    ReleaseMutex(mutex);
    CloseHandle(mutex);
}

void SimpleLock::lock() 
{
    WaitForSingleObject(mutex, INFINITE);
}

void SimpleLock::unlock() 
{
    ReleaseMutex(mutex);
}

void ReentrantLock::initNewOwner()
{
    owner = GetCurrentFiber();
    holdCount = 0;
}

ReentrantLock::ReentrantLock(bool isOwner /* = false */) : SimpleLock(isOwner)
{
    initNewOwner();
}

void ReentrantLock::lock() 
{
    // Если текущий фибер уже владеет блокировкой, 
    // то увеличиваем счетчик, иначе ожидаем освобождения блокировки.
    if (owner == GetCurrentFiber()) {
        holdCount++;
    } else {
        SimpleLock::lock();
        initNewOwner();
    }
}

void ReentrantLock::unlock() 
{
    if (owner == GetCurrentFiber()) {
        if (holdCount != 0) {
            holdCount--;
        } else {
            SimpleLock::unlock();
        }
    }
}

#else 



#endif
