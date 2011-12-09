
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



#ifndef __LOCK_H__
#define __LOCK_H__

#include "libsys.h"

class LIBSYS_API Lock 
{
public:
	virtual ~Lock() {};
	virtual void lock() = 0;
	virtual void unlock() = 0;
};

class LIBSYS_API SimpleLock : public Lock
{
protected:
	void *mutex;

public:
	SimpleLock(bool isOwner = false);
	~SimpleLock();

	void lock();
	void unlock();
};

class LIBSYS_API ReentrantLock : public SimpleLock
{
protected:
	void *owner;
	int holdCount;

public:
	ReentrantLock(bool isOwner = false);

	void lock();
	void unlock();

protected:
	void initNewOwner();
};

class LIBSYS_API ScopeLock : Lock
{
private:
	Lock *realLock;

public:
	bool flag;

	ScopeLock(Lock *_lock) : realLock(_lock), flag(true)
	{
		lock();
	}

	~ScopeLock() 
	{
		realLock->unlock();
	}

	void lock() 
	{
		realLock->lock();
	}

	void unlock() 
	{
		realLock->unlock();
	}

};

#define LOCK_SECTION(realLock) for (ScopeLock __internalLock(&realLock); __internalLock.flag; __internalLock.flag = false)
#define LOCAL_LOCK(realLock) ScopeLock __internalLock(&realLock);

#endif // __LOCK_H__
