#include <stddef.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

struct my_allocator  *new_allocator(size_t size);
char * c_alloc(struct my_allocator **allocator, size_t size);
void c_free(struct my_allocator **allocator, char *ptr);