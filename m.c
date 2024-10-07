#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "a.h"
#include "asm_data.h"
#include "calc_math.h"
#include "help_data.h"
#include "ir.h"
#include "utility.h"

void push_operation(char *memory_block, CALC_OP *operation) {
  switch (operation->kind) {
  case PUSH:
    memory_block[0] = 0x68; // push number 64 bit
    char *opnum = &operation->op;
    memory_block[1] = opnum[0];
    memory_block[2] = opnum[1];
    memory_block[3] = opnum[2];
    memory_block[4] = opnum[3];
    break;
  case ADD:
    copymem(memory_block, asm_add, asm_add_len);
    break;
  case SUB:
    copymem(memory_block, asm_sub, asm_sub_len);
    break;
  case SWAP:
    copymem(memory_block, asm_swap, asm_swap_len);
    break;
  case DUP:
    copymem(memory_block, asm_dup, asm_dup_len);
    break;
  case MUL:
    copymem(memory_block, asm_mul, asm_mul_len);
    break;
  case DIV:
    copymem(memory_block, asm_div, asm_div_len);
    break;
  case MOD:
    copymem(memory_block, asm_mod, asm_mod_len);
    break;
  case PUSH_ARG:
    //                  rdi    rsi  rcx
    char push_args[] = {0x57, 0x56, 0x51};
    memory_block[0] = push_args[operation->op];
    break;
  case IF:
    copymem(memory_block, asm_if, asm_if_len);
    break;
  case RET:
    memory_block[0] = 0x58; // pop rax
    memory_block[1] = 0xc3; // return
    break;
  case SQRT:
    copymem(memory_block, asm_sqrt, asm_sqrt_len);
    void *sqrtPTR = &isqrt;
    U64t p = (U64t)sqrtPTR;
    int i = 5;
    memory_block[i] = p & 0xff;
    memory_block[i + 1] = (p >> 8) & 0xff;
    memory_block[i + 2] = (p >> 16) & 0xff;
    memory_block[i + 3] = (p >> 24) & 0xff;
    memory_block[i + 4] = (p >> 32) & 0xff;
    memory_block[i + 5] = (p >> 40) & 0xff;
    memory_block[i + 6] = (p >> 48) & 0xff;
    memory_block[i + 7] = (p >> 56) & 0xff;
    break;
  default:
    printf("DEBUG: Unkown operation kind %d\n", operation->kind);
    exit(-1);
    break;
  }
}

void *calc_compile(FIXED_STACK f) {
  U64t comp_size = 0;
  I64t cstack = 0;
  ITR(i, f.curr_item) {
    CALC_OP *op = ((CALC_OP *)f.data + i);
    if (op->reduce) {
      assert(op->stackchange == -1 && op->stackrequirement == 2 &&
             "INVALID REDUCE OPERATOR");
      comp_size += op->compiled_size * cstack;
      cstack = 1;
    } else {
      comp_size += op->compiled_size;
      cstack += op->stackchange;
    }
  }
  printf("DEBUG: Allocating %ld bytes for execution (IR size: %ld)\n",
         comp_size + 3, f.curr_item);
  // get a block of executable memory to write our instructions to
  char *exec_block = mmap(NULL, comp_size, PROT_EXEC | PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  // store our 3 arg in rcx instead of rdx
  exec_block[0] = 0x48;
  exec_block[1] = 0x89;
  exec_block[2] = 0xd1;
  char *place = exec_block + 3;
  cstack = 0;
  ITR(i, f.curr_item) {
    CALC_OP *operation = (CALC_OP *)f.data + i;
    if (cstack < operation->stackrequirement) {
      printf("ERROR: Instruction %ld requires stack to have atleast %ld "
             "item(s) but stack will have %ld\n",
             i, operation->stackrequirement, cstack);
      munmap(exec_block, comp_size);
      return NULL;
    }
    if (operation->reduce) {
      ITR(x, cstack - 1) {
        push_operation(place, operation);
        place += operation->compiled_size;
      }
      cstack = 1;
    } else {
      cstack += operation->stackchange;
      push_operation(place, operation);
      place += operation->compiled_size;
    }
  }
  if (cstack != 0) {
    printf("ERROR: Stack is marked as non empty at the end of execution!\n");
    munmap(exec_block, comp_size);
    return NULL;
  }
  return exec_block;
}

void add_operation(FIXED_STACK *s, CALC_OP_KIND k, I64t operand, I64t reduce) {
  CALC_OP *x = fixed_stack_alloc(s);
  x->kind = k;
  x->op = operand;
  x->compiled_size = COMPSIZES[k];
  x->stackchange = STACKCHANGES[k];
  x->stackrequirement = STACKREQUIREMENTS[k];
  x->reduce = reduce;
}

int is_num(char c) { return (c >= '0') && (c <= '9'); }

FIXED_STACK to_ir(char *input, U64t ilen) {
  FIXED_STACK instructions = {0};
  fixed_stack_init(&instructions, 1024, SIZE(CALC_OP));
  int secondary = 0;
  int reduce = 0;
  ITRFROM(i, 1, ilen + 1) {
    if (is_num(input[ilen - i])) {
      I64t op = input[ilen - i] - '0';
      i++;
      I64t place = 1;
      while ((i < ilen) && is_num(input[ilen - i])) {
        op += (input[ilen - i] - '0') * pow(10, place);
        i++;
        place++;
      }
      i--;
      add_operation(&instructions, PUSH, op, 0);
      continue;
    }
    switch (input[ilen - i]) {
    case '+':
      if (secondary) {
        add_operation(&instructions, SQRT, 0, 0);
      } else {
        add_operation(&instructions, ADD, 0, reduce);
      }
      break;
    case '~':
      if (secondary) {
        add_operation(&instructions, DUP, 0, 0);
      } else {
        add_operation(&instructions, SWAP, 0, 0);
      }
      break;
    case '-':
      if (secondary) {
        add_operation(&instructions, PUSH, 0, 0);
        add_operation(&instructions, SUB, 0, 0);
      } else {
        add_operation(&instructions, SUB, 0, reduce);
      }
      break;
    case '*':
      if (secondary) {
        add_operation(&instructions, END, 0, reduce);
      } else {
        add_operation(&instructions, MUL, 0, reduce);
      }
      break;
    case '%':
      if (secondary) {
        add_operation(&instructions, MOD, 0, reduce);
      } else {
        add_operation(&instructions, DIV, 0, reduce);
      }
      break;
    case '$':
      add_operation(&instructions, IF, 0, 0);
      break;
    case 'x':
      add_operation(&instructions, PUSH_ARG, 0, 0);
      break;
    case 'y':
      add_operation(&instructions, PUSH_ARG, 1, 0);
      break;
    case 'z':
      add_operation(&instructions, PUSH_ARG, 2, 0);
      break;
    case ':': // next thing will be secondary
      secondary = 1;
      continue;
    case '/': // next thing will be secondary
      reduce = 1;
      continue;
    default:
      break;
    }
    secondary = 0;
    reduce = 0;
  }
  add_operation(&instructions, RET, 0, 0);

  return instructions;
}

int main() {
  printf("calcjit 2024 - nathan swann\n");
  printf("type \\ for help\n");
  void *functions[256] = {0};
  while (1) {
    printf("calc) ");
    char input[100000];
    fgets(input, sizeof(input), stdin);
    if (input[0] == '\\') {
      if (strlen(input) == 2) {
        printf("%s\n", help_page);
        continue;
      }
      if (input[1] == '\\') {
        ITR(i, 256) {
          void *f = functions[i];
          if (f != NULL) {
            munmap(f,strlen(f));
          }
        }
        break;
      }
      if (input[1] == 'd') {
        char fname = input[2];
	FIXED_STACK ir = to_ir(input + 3, strlen(input + 3));
        __int64_t (*func)(int, int, int) = calc_compile(ir);
	fixed_stack_destroy(&r);
        if (func != NULL) {
          functions[fname] = func;
        } else {
          printf("ERROR: Failed to compile input\n");
        }
        continue;
      }
      if (input[1] == 'c') {
        char fname = input[2];
        FIXED_STACK ir = to_ir(input + 3, strlen(input + 3));
        __int64_t (*func)(int, int, int) = functions[fname];
        if (func == NULL) {
          printf("ERROR: Function not defined\n");
          continue;
        }
        CALC_OP *d = ir.data;
        int x[3] = {0};
        int j = 0;
        for (int i = ir.curr_item - 2; i >= 0; i--) {
          x[j] = d[i].op;
          j++;
          if (j == 3) {
            break;
          }
        }
        printf("%ld\n", func(x[0], x[1], x[2]));
        fixed_stack_destroy(&ir);
        continue;
      }
    }
    __int64_t (*func)(int, int, int) =
        calc_compile(to_ir(input, strlen(input) + 3));
    if (func == NULL) {
      printf("ERROR: Failed to compile input\n");
    } else {
      printf("%ld\n", func(0, 0, 0));
    }
  }
}
