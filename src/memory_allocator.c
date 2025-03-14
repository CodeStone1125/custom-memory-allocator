#include "../include/memory_allocator.h"
#include "../include/block.h"


typedef struct my_allocator {
    size_t total_size;
    size_t peak_size;
    block_t *free_root;
} my_allocator_t;

block_t* new_block(size_t size) {
    block_t *node = (block_t *)malloc(sizeof(block_t));
    node->size = size;
    node->l = node->r = NULL;
    node->freechunk = (char *)malloc(size);
    return node;
}

my_allocator_t *new_allocator(size_t size){
    my_allocator_t *allocator = (my_allocator_t *)malloc(sizeof(my_allocator_t));
    allocator->total_size = size;
    allocator->peak_size = 0;
    allocator->free_root = new_block(size);
    return allocator;
}


int main() {
    size_t size = 100;
    my_allocator_t *alloc = new_allocator(size);
    for(int i = 0; i < alloc->free_root->size; i++) {
        printf("the struct p[%d] address =%p\n", i, alloc->free_root->freechunk + i);
    }
    return 0;
}
 