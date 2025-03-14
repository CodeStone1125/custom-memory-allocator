#include "../include/memory_allocator.h"

typedef struct block {
    size_t size;
    struct block *l, *r;
    char * freechunk;
} block_t;

typedef struct my_allocator {
    size_t total_size;
    size_t peak_size;
    size_t used_size;
    block_t *free_root;
} my_allocator_t;

block_t* new_block(size_t size) {
    block_t *node = (block_t *)malloc(sizeof(block_t));
    node->size = size;
    node->l = node->r = NULL;
    node->freechunk = (char *)malloc(size);
    return node;
}

block_t **find_free_tree(block_t **root, block_t *target)
{
    if(*root == NULL) return NULL;
    block_t **ptr = root;

    if((*ptr)->size >= target->size) 
        return ptr;
    // else if ((*ptr)->size > target->size) 
    //     return find_free_tree(&(*ptr)->l, target);
    else if ((*ptr)->size < target->size) 
        return find_free_tree(&(*ptr)->r, target);
    

    return NULL;
}

block_t *find_predecessor_free_tree(block_t **root, block_t *node)
{
    block_t **ptr = find_free_tree(root, node);

    if (ptr == NULL || *ptr == NULL) {
        return NULL;
    }
    
    if ((*ptr)->l) {
        ptr = &(*ptr)->l;
        while((*ptr)->r){
            ptr = &(*ptr)->r;
        }
    }
    return *ptr;
}

my_allocator_t *new_allocator(size_t size){
    my_allocator_t *allocator = (my_allocator_t *)malloc(sizeof(my_allocator_t));
    allocator->total_size = size;
    allocator->peak_size = 0;
    allocator->used_size = 0;
    allocator->free_root = new_block(size);
    if(allocator->free_root == NULL) {
        free(allocator);
        return NULL;
    }
    return allocator;
}

/*
 * Structure representing a free memory block in the memory allocator.
 * The free tree is a binary search tree that organizes free blocks (of type block_t)
 * to efficiently locate a block of appropriate size during memory allocation.
 */
block_t *remove_free_tree(block_t **root, block_t *target)
{
    /* Locate the pointer to the target node in the tree. */
    block_t **node_ptr = find_free_tree(root, target);

    if (!node_ptr || !(*node_ptr)) {
        return NULL;
    }

    block_t *removed_node = *node_ptr;

    /* If the target node has two children, we need to find a replacement. */
    if ((*node_ptr)->l && (*node_ptr)->r) {
        /* Find the in-order predecessor (rightmost node in the left subtree). */
        block_t **pred_ptr = &(*node_ptr)->l;
        while ((*pred_ptr)->r)
            pred_ptr = &(*pred_ptr)->r;

        block_t *expected_pred = find_predecessor_free_tree(root, *node_ptr);
        assert(expected_pred == *pred_ptr);

        if (*pred_ptr == (*node_ptr)->l) {
            block_t *old_right = (*node_ptr)->r;
            *node_ptr = *pred_ptr;
            (*node_ptr)->r = old_right;
        } else {
            block_t *old_left = (*node_ptr)->l;
            block_t *old_right = (*node_ptr)->r;
            block_t *pred_node = *pred_ptr;
            remove_free_tree(&old_left, *pred_ptr);
            *node_ptr = pred_node;
            (*node_ptr)->l = old_left;
            (*node_ptr)->r = old_right;
        }
    }
    /* If the target node has one or no children, replace it directly. */
    else if ((*node_ptr)->l || (*node_ptr)->r) {
        *node_ptr = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
    } else {
        *node_ptr = NULL;
    }

    removed_node->l = NULL;
    removed_node->r = NULL;

    return removed_node;
}


void insert_free_tree(block_t **root, block_t *target){
    block_t ** ptr = root;
    while((*ptr)){
        if(target->size > (*ptr)->size){
            ptr = &(*ptr)->r;
        }else{
            ptr = &(*ptr)->l;
        }
    }
    *ptr = target;
    return;
}

char *c_alloc(struct my_allocator **allocator, size_t size) {
    if (allocator == NULL || *allocator == NULL) 
        return NULL;

    block_t target;
    target.size = size;
    // block_t **sel_ptr = NULL;
    block_t *sel_ptr = remove_free_tree(&(*allocator)->free_root, &target);

    char *alloc_ptr = sel_ptr->freechunk;

    sel_ptr->freechunk += size;
    sel_ptr->size -= size;

    if (sel_ptr->size > 0) {
        insert_free_tree(&(*allocator)->free_root, sel_ptr);
    }

    (*allocator)->used_size += size;
    (*allocator)->peak_size = ((*allocator)->used_size > (*allocator)->peak_size) ? (*allocator)->used_size : (*allocator)->peak_size;
    return alloc_ptr;
}


int main() {
    size_t size = 1000000;
    my_allocator_t *alloc = new_allocator(size);
    if (alloc == NULL) {
        printf("Allocator creation failed\n");
        return 1;
    }

    // Ensure the free_root block is valid before accessing
    if (alloc->free_root == NULL) {
        printf("Free block root is NULL\n");
        return 1;
    }

    // Print the starting free chunk addresses
    printf("Free chunk addresses before allocation:\n");
    char *current = alloc->free_root->freechunk;
    printf("the struct p address = %p\n", current);
    printf("%s", "--------------------\n");

    // Try to allocate memory
    size_t alloc_size = rand() % 1000;
    char *p = c_alloc(&alloc, alloc_size);
    if (p == NULL) {
        printf("c_alloc failed\n");
        return 1;
    }
    
    // Print the address returned by c_alloc
    printf("Allocated address: %p\n", p);
    printf("%s", "--------------------\n");
    current = alloc->free_root->freechunk;

    printf("the struct p address = %p after %lx allocated\n", current, alloc_size);
    


    return 0;
}
 