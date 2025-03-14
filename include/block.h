#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct block {
    size_t size;
    struct block *l, *r;
    char * freechunk;
} block_t;


inline block_t **find_free_tree(block_t **root, block_t *target)
{
    if(*root == NULL) return NULL;
    block_t **ptr = root;

    if((*ptr)->size == target->size) 
        return ptr;
    else if ((*ptr)->size > target->size) 
        return find_free_tree(&(*ptr)->l, target);
    else if ((*ptr)->size < target->size) 
        return find_free_tree(&(*ptr)->r, target);
    

    return NULL;
}

inline block_t *find_predecessor_free_tree(block_t **root, block_t *node)
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

/*
 * Structure representing a free memory block in the memory allocator.
 * The free tree is a binary search tree that organizes free blocks (of type block_t)
 * to efficiently locate a block of appropriate size during memory allocation.
 */
inline void remove_free_tree(block_t **root, block_t *target)
{
    /* Locate the pointer to the target node in the tree. */
    block_t **node_ptr = find_free_tree(root, target);

    /* If the target node has two children, we need to find a replacement. */
    if ((*node_ptr)->l && (*node_ptr)->r) {
        /* Find the in-order predecessor:
         * This is the rightmost node in the left subtree.
         */
        block_t **pred_ptr = &(*node_ptr)->l;
        while ((*pred_ptr)->r)
            pred_ptr = &(*pred_ptr)->r;

        /* Verify the found predecessor using a helper function (for debugging).
         */
        block_t *expected_pred = find_predecessor_free_tree(root, *node_ptr);
        assert(expected_pred == *pred_ptr);

        /* If the predecessor is the immediate left child. */
        if (*pred_ptr == (*node_ptr)->l) {
            block_t *old_right = (*node_ptr)->r;
            *node_ptr = *pred_ptr; /* Replace target with its left child. */
            (*node_ptr)->r = old_right; /* Attach the original right subtree. */
            assert(*node_ptr != (*node_ptr)->l);
            assert(*node_ptr != (*node_ptr)->r);
        } else {
            /* The predecessor is deeper in the left subtree. */
            block_t *old_left = (*node_ptr)->l;
            block_t *old_right = (*node_ptr)->r;
            block_t *pred_node = *pred_ptr;
            /* Remove the predecessor from its original location. */
            remove_free_tree(&old_left, *pred_ptr);
            /* Replace the target node with the predecessor. */
            *node_ptr = pred_node;
            (*node_ptr)->l = old_left;
            (*node_ptr)->r = old_right;
            assert(*node_ptr != (*node_ptr)->l);
            assert(*node_ptr != (*node_ptr)->r);
        }
    }
    /* If the target node has one child (or none), simply splice it out. */
    else if ((*node_ptr)->l || (*node_ptr)->r) {
        block_t *child = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
        *node_ptr = child;
    } else {
        /* No children: remove the node. */
        *node_ptr = NULL;
    }

    /* Clear the removed node's child pointers to avoid dangling references. */
    target->l = NULL;
    target->r = NULL;
}

inline void insert_free_tree(block_t **root, block_t *target){
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

inline void print_level_order_with_spacing(block_t* root) {
    if (root == NULL) return;

    // 使用隊列來執行層級遍歷，並記錄每層的節點
    block_t* queue[100];  // 用來儲存節點的隊列
    int front = 0, rear = 0;
    int level = 0;  // 記錄當前層級
    int print_next_level = 1;  // 用來判斷是否需要打印下一層

    // 將根節點加入隊列
    queue[rear++] = root;

    // 層級遍歷
    while (front < rear) {
        int nodes_at_current_level = rear - front; // 記錄當前層級有多少節點
        int all_null = 1;  // 用來檢查是否這一層全部為 NULL 節點

        // 打印當前層級的節點，並根據層級計算空格
        if (print_next_level) {
            printf("Level %d: ", level);
        }
        
        // 每一層的空格數量，根據層級控制間距
        int space_between_nodes = (10 - level);  // 控制節點之間的間隔空格
        for (int j = 0; j < space_between_nodes; j++) {
            printf(" ");
        }

        for (int i = 0; i < nodes_at_current_level; i++) {
            block_t* node = queue[front++];

            // 如果節點為NULL，打印 "null"
            if (node == NULL) {
                printf("null");
            } else {
                // 打印節點的值，並根據需要添加空格
                if (i > 0) {
                    for (int j = 0; j < 2 * (3 - level); j++) {
                        printf(" ");
                    }
                }
                printf("%zu", node->size);
                all_null = 0;  // 只要有非 NULL 節點，標誌位改為 0
            }

            // 將左子樹和右子樹加入隊列，處理 NULL 節點
            if (node != NULL) {
                if (node->l != NULL) {
                    queue[rear++] = node->l;
                } else {
                    queue[rear++] = NULL;
                }

                if (node->r != NULL) {
                    queue[rear++] = node->r;
                } else {
                    queue[rear++] = NULL;
                }
            }
        }

        // 如果這一層全部為 NULL，則跳過打印該層
        if (!all_null) {
            print_next_level = 1;
            printf("\n");
        } else {
            print_next_level = 0;
        }

        // 增加層級數
        level++;
    }
}

