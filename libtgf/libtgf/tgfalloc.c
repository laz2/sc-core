#include "tgf.h"
#include "tgfalloc.h"
#include <stdlib.h>

#if defined(TGF_UNIX)
#include <unistd.h>
#endif

#define MIN_CHUNK_SIZE	16384

struct _tgf_page {
	size_t size;
	tgf_page *prev;
};

struct _tgf_allocator {
	tgf_uint32 chunk_size;
	tgf_page *current_page;
	tgf_uint32 free_offs;
};

static
void	*iso_alloc_a4k(int count)
{
	return malloc(count);
}

static
void	iso_free_a4k(void *p)
{
	free(p);
}

#if	defined(_POSIX_MAPPED_FILES)
#include <sys/mman.h>
#include <fcntl.h>

#if	defined(_SC_PAGESIZE) /* It's not POSIX (SUS v2) */
static	size_t page_size = MIN_CHUNK_SIZE;
#undef MIN_CHUNK_SIZE
#define MIN_CHUNK_SIZE page_size
#endif

static	int use_iso = 1;
static	int init_done = 0;

#if defined(MAP_ANONYMOUS) /* As far as I know it's Linux and BSD feature */
#define ANONYM MAP_ANONYMOUS
#define zero_fd 0
#define mmap_offset 0
#else
#define ANONYM 0
static	int zero_fd=0; /* fd of /dev/zero */
static	off_t mmap_offset = 0;
#endif

static
void	posix_alloc_init()
{
#if	defined(_SC_PAGESIZE)
	size_t ps;
	ps = sysconf(_SC_PAGESIZE);
	if (ps>0)
		page_size = MAX(ps,page_size);
#endif
#if !defined(MAP_ANONYMOUS)
	zero_fd = open("/dev/zero",O_RDWR);
#endif
	init_done = 1;
}
static
void *	posix_alloc_a4k(int count)
{
	void *res;
	if (!init_done)
		posix_alloc_init();
	if (use_iso)
		return iso_alloc_a4k(count);
	res = mmap(0,count,PROT_READ|PROT_WRITE,MAP_PRIVATE|ANONYM,zero_fd,mmap_offset);
	if (res == MAP_FAILED) {
		use_iso = 1;
		return iso_alloc_a4k(count);
	}
	/* 'couse I'm not sure about MAP_PRIVATE inside one process */
#	ifndef MAP_ANONYMOUS
		mmap_offset += count;
#	endif
	return res;
}
static
void	posix_free_a4k(void *ptr,int size)
{
	if (use_iso)
		iso_free_a4k(ptr);
	else
		munmap(ptr,size);
}

#define alloc_a4k(cnt) posix_alloc_a4k(cnt)
#define free_a4k(ptr,size) posix_free_a4k(ptr,size)
#else /* POSIX */
#define alloc_a4k(cnt) iso_alloc_a4k(cnt)
#define free_a4k(ptr,size) iso_free_a4k(ptr)
#endif /* POSIX */


tgf_allocator *	tgf_allocator_new(int chunk_size)
{
	tgf_allocator *p = malloc(sizeof(tgf_allocator));
	if (!p)
		return 0;
	chunk_size = MAX(MIN_CHUNK_SIZE,chunk_size);
	p->chunk_size = chunk_size;
	p->current_page = 0;
	return p;
}

void	tgf_allocator_free_all(tgf_allocator *a)
{
	tgf_page *ptr = a->current_page;
	if (!ptr)
		return;
	while (1) {
		tgf_page *prev = ptr->prev;
		if (!prev) {
			a->current_page = ptr;
			a->free_offs = sizeof(tgf_page);
			break;
		}
		free_a4k(ptr,ptr->size);
		ptr = prev;
	}
}

void	tgf_allocator_delete(tgf_allocator *a)
{
	tgf_allocator_free_all(a);
	if (a->current_page)
		free_a4k(a->current_page,a->current_page->size);
	free(a);
}

void	tgf_allocator_set_chunk_size(tgf_allocator *a,size_t s)
{
	a->chunk_size = MIN(MIN_CHUNK_SIZE,s);
}

int	tgf_allocator_get_chunk_size(tgf_allocator *a)
{
	return a->chunk_size;
}

static INLINE
int	add_page(tgf_allocator *a,size_t size)
{
	tgf_page *ptr;
	ptr = alloc_a4k(size);
	if (!ptr)
		return -1;
	ptr->size = size;
	ptr->prev = a->current_page;
	a->current_page = ptr;
	return 0;
}

void *	tgf_allocator_alloc(tgf_allocator *a,size_t size)
{
	char *ptr;
	tgf_uint32 allign=3;
	int allignment;
	/* some systems require allignment. On others allignment speeds up
	    access */
	if (size>=16)
		allign = 15;
	else if (size>=8)
		allign = 7;
	if (!a->current_page)
		goto __add_page;
	a->free_offs = (a->free_offs + allign) & ~allign;
	if (size>(a->current_page->size - a->free_offs)) {
	__add_page:
		allignment = (sizeof(tgf_page) + allign) & ~allign;
		if (add_page(a,MAX(a->chunk_size,size+allignment)))
			return 0;
		a->free_offs = allignment;
	}
	ptr = (char *)a->current_page+a->free_offs;
	a->free_offs += size;
	return ptr;
}
