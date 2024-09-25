#include <stddef.h>

typedef struct
{
    size_t stack_size;  // size of the stack
    size_t curr_item;  // current top of the stack
    size_t element_size; // size of a single element
    void* data;   // actual data
} FIXED_STACK;

void fixed_stack_init(FIXED_STACK* stack,size_t size,size_t element_size);
void fixed_stack_destroy(FIXED_STACK* stack);

void* fixed_stack_alloc(FIXED_STACK* stack);
void* fixed_stack_pop(FIXED_STACK* stack);

void copymem(char* x, char* y, size_t l);