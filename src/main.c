#include <stdio.h>
#include "../include/memory_allocator.h"
#include "../include/block.h"


int main() {
    block_t* root = create_tree();
    print_level_order_with_spacing(root);
    block_t temp_block;
    temp_block.size = 5;
    block_t **node_ptr = find_free_tree(&root, &temp_block);
    remove_free_tree(&root, new_block(5));
    node_ptr = find_free_tree(&root, &temp_block);
    remove_free_tree(&root, new_block(5));
    printf("\nLevel Order Traversal with Spacing:\n");
    print_level_order_with_spacing(root);
    printf("\n");

    return 0;
}
