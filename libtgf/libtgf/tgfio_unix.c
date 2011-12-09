/*
 *  SCPx processing module project.
 *
 *  tgfio_unix.c - this file contains __tgf_{read,write}_fn
 *     implemented for Unix.
 *
 *  Created at Mon 07 May 2001 01:21:07 AM EEST by ALK
 *
 */
#if defined (TGF_UNIX)
#include <unistd.h>
#endif

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <errno.h>

#include "tgfin.h"
#include "tgfout.h"

int	__tgf_read_fn(void *fd,char *buf,int size)
{
	int count;
	do {
		count = read((int)fd,buf,size);
	} while (count == -1 && (errno == EAGAIN || errno == EINTR));
	return count;
}

/* I think, that when no space left it sets ENOSPC, NOT RETURNS 0 */
/*  (MS-DOG returns 0 ) */
int	__tgf_write_fn(void *fd,char *buf,int size)
{
	int count,written=0;
	do {
		count = write((int)fd,buf,size);
		if (count<0 && errno != EAGAIN && errno != EINTR)
			return written;
		else
			written+=count;
	} while (size>written);
	return written;
}
