#include <ucontext.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <libsys/context.h>
#include <limits.h>
#include <stdio.h>

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#ifndef STACK_GROW_DIRECTION
/* if not given. Stack grows down */
#define STACK_GROW_DIRECTION 0
#endif

/* yet very simple prototype implementation */
#define STACK_SIZE (256*1024U)

typedef void *(*ctxfunc)(void *);

struct _sc_context
{
	void *	prm;
	void *	res;
	ctxfunc func;
	ucontext_t ctx;
};

static
sc_context *current_context;

static
ucontext_t toplevel;

sc_context *context_alloc()
{
	sc_context *rv = malloc(sizeof(sc_context));
	void *stack;
	if (!rv)
		abort();
#if 1
	stack = mmap(0,STACK_SIZE,PROT_READ|PROT_WRITE,
			MAP_SHARED|MAP_ANONYMOUS,0,0);
	if (stack == MAP_FAILED)
		abort();
	/* setup red zone for stack */
#if STACK_GROW_DIRECTION
	mprotect((char *)stack+STACK_SIZE-PAGESIZE,PAGESIZE,PROT_NONE);
#else
	mprotect(stack,PAGESIZE,PROT_NONE);
#endif
#else
	stack = malloc(STACK_SIZE);
#endif
	getcontext(&rv->ctx);
	rv->ctx.uc_stack.ss_size = STACK_SIZE;
	rv->ctx.uc_stack.ss_sp = stack;
	return rv;
}

void	context_free(sc_context *ctx)
{
#if 1
	munmap(ctx->ctx.uc_stack.ss_sp,STACK_SIZE);
#else
	free(ctx->ctx.uc_stack.ss_sp);
#endif
	free(ctx);
}

void	context_init()
{
	getcontext(&toplevel);
}

void	context_deinit()
{
}

static
void	__in_context(sc_context *ctx)
{
	ctx->res = ctx->func(ctx->prm);
}

void *	context_call(sc_context *ctx,ctxfunc f,void *prm)
{
	ctx->res = 0;
	ctx->ctx.uc_link = &toplevel;
	ctx->prm = prm;
	ctx->func = f;
	makecontext(&ctx->ctx,(void (*)(void))__in_context,1,ctx);
	current_context = ctx;

	swapcontext(&toplevel,&ctx->ctx);
	current_context = 0;
	return ctx->res;
}

void	context_switchback()
{
	sc_context *ctx = current_context;
	current_context = 0;
	swapcontext(&ctx->ctx,&toplevel);
}

void *	context_recall(sc_context *ctx)
{
	current_context = ctx;
	swapcontext(&toplevel,&ctx->ctx);
	return ctx->res;
}

