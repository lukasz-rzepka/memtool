/* Prototypes for __malloc_hook, __free_hook */
#include <malloc.h>
#include <pthread.h>

/* Prototypes for our hooks.  */
static void my_init_hook (void);
static void *my_malloc_hook (size_t, const void *);
static void *my_realloc_hook (void*, size_t, const void *);
static void my_free_hook (void*, const void *);

/* Placeholders for old hooks.  */
void (*old_free_hook) (void *ptr, const void *caller);
void *(*old_malloc_hook) (size_t size, const void *caller);
void *(*old_realloc_hook) (void *ptr, size_t size, const void *caller);

/* Override initializing hook from the C library. */
void (*__MALLOC_HOOK_VOLATILE __malloc_initialize_hook) (void) = my_init_hook;

/* spinlock. */
pthread_spinlock_t lock;


// signal handler to invoke my_init_hook
//

// 

static void
my_init_hook (void)
{
    pthread_spin_init(&lock, 0);
    pthread_spin_lock(&lock);
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_free_hook = __free_hook;
    printf("Memtool init!\n");
    __malloc_hook = my_malloc_hook;
    __realloc_hook = my_realloc_hook;
    __free_hook = my_free_hook;
    pthread_spin_unlock(&lock);
}

static void
my_finit_hook (void)
{
    pthread_spin_lock(&lock);
    /* Restore all old hooks */
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __free_hook = old_free_hook;
    printf("Memtool finit!\n");
    pthread_spin_unlock(&lock);
    pthread_spin_destroy(&lock);
}

static void *
my_malloc_hook (size_t size, const void *caller)
{
    pthread_spin_lock(&lock);
    void *result;
    /* Restore all old hooks */
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __free_hook = old_free_hook;
    /* Call recursively */
    result = malloc (size);
    /* Save underlying hooks */
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_free_hook = __free_hook;
    /* printf might call malloc, so protect it too. */
    printf ("malloc (%u) returns %p\n", (unsigned int) size, result);
    /* Restore our own hooks */
    __malloc_hook = my_malloc_hook;
    __realloc_hook = my_realloc_hook;
    __free_hook = my_free_hook;
    pthread_spin_unlock(&lock);
    return result;
}

static void *
my_realloc_hook (void *ptr, size_t size, const void *caller)
{
    pthread_spin_lock(&lock);
    void *result;
    /* Restore all old hooks */
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __free_hook = old_free_hook;
    /* Call recursively */
    result = realloc (ptr, size);
    /* Save underlying hooks */
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_free_hook = __free_hook;
    /* printf might call malloc, so protect it too. */
    printf ("realloc (%p, %u) returns %p\n", ptr, (unsigned int) size, result);
    /* Restore our own hooks */
    __malloc_hook = my_malloc_hook;
    __realloc_hook = my_realloc_hook;
    __free_hook = my_free_hook;
    pthread_spin_unlock(&lock);
    return result;
}

static void
my_free_hook (void *ptr, const void *caller)
{
    pthread_spin_lock(&lock);
    /* Restore all old hooks */
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __free_hook = old_free_hook;
    /* Call recursively */
    free (ptr);
    /* Save underlying hooks */
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_free_hook = __free_hook;
    /* printf might call free, so protect it too. */
    printf ("freed pointer %p\n", ptr);
    /* Restore our own hooks */
    __malloc_hook = my_malloc_hook;
    __realloc_hook = my_realloc_hook;
    __free_hook = my_free_hook;
    pthread_spin_unlock(&lock);
}

