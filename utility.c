#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "utility.h"
#include "a.h"

Vt fixed_stack_init(FIXED_STACK *stack, SZt stack_size, SZt element_size)
{
    stack->curr_item = 0;
    stack->stack_size = stack_size;
    stack->element_size = element_size;
    stack->data = malloc(stack_size * element_size);
    MEMCL(stack->data, stack_size * element_size); // ensure the data is actually clear
}

Vt fixed_stack_destroy(FIXED_STACK *st)
{
    free(st->data);
    st->stack_size = 0; // setting to zero st when attempting to pop will return null
}

Vt *fixed_stack_alloc(FIXED_STACK *st)
{
    if (st->stack_size <= st->curr_item)
        R(NULL);
    Vt *p = st->data + st->curr_item * st->element_size;
    st->curr_item++;
    R(p);
}

Vt *fixed_stack_pop(FIXED_STACK *st)
{
    if (st->curr_item == 0)
        R(NULL);
    st->curr_item--;
    R(st->data + st->curr_item * st->element_size);
}

Vt copymem(char* x, char* y, U64t l)
{
    ITR(i,l){
        x[i]=y[i];
    }
}