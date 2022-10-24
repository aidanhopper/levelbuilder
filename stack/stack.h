#ifndef STACK_H
#define STACK_H

typedef struct StackNode StackNode;
struct StackNode {
  unsigned long index;
  unsigned char value;
  StackNode *next;
};


typedef struct {
  StackNode *head;
  unsigned long size;
} Stack;

void InitStack(Stack *stack);
StackNode *StackTop(Stack stack);
unsigned long StackSize(Stack stack);
char StackEmpty(Stack stack);
void StackPush(Stack *stack, unsigned long index, unsigned char value);
void StackPop(Stack *stack);

#endif
