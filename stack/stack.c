// TODO: limit the size of the stack so that it cant crash your computer

#include <stdlib.h>
#include "stack.h"

void InitStack(Stack *stack) {
  stack->size = 0;
  stack->head = NULL;
}

StackNode *StackTop(Stack stack) {
  return stack.head;
}

unsigned long StackSize(Stack stack) {
  return stack.size;
}

char StackEmpty(Stack stack) {
  if (stack.size == 0)
    return 1;
  return 0;
}

void StackPush(Stack *stack, unsigned long index, unsigned char value) {
  StackNode *node = malloc(sizeof(StackNode));
  stack->size++;
  node->index = index;
  node->value = value;
  node->next = stack->head;
  stack->head = node;
}

void StackPop(Stack *stack) {
  stack->size--;
  StackNode *tmp = stack->head;
  stack->head = tmp->next;
  free(tmp);
}
