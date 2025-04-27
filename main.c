#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRANSITIONS 100
#define MAX_STACK_SIZE 100
#define MAX_SYMBOL_LENGTH 10

typedef struct {
    char readSymbol;
    char popSymbol;
    char pushSymbol[MAX_SYMBOL_LENGTH];
    int toState;
} Transition;

typedef struct {
    Transition transitions[MAX_TRANSITIONS];
    int transitionCount;
} State;

typedef struct {
    State states[10];
    int initialState;
    int finalStates[10];
    int finalStateCount;
} Automaton;

typedef struct {
    char items[MAX_STACK_SIZE];
    int top;
} Stack;

// Stack functions
void initStack(Stack *stack) {
    stack->top = -1;
}

int isEmpty(Stack *stack) {
    return stack->top == -1;
}

void push(Stack *stack, char symbol) {
    if (stack->top < MAX_STACK_SIZE - 1) {
        stack->items[++stack->top] = symbol;
    }
}

char pop(Stack *stack) {
    if (!isEmpty(stack)) {
        return stack->items[stack->top--];
    }
    return '\0';
}

char peek(Stack *stack) {
    if (!isEmpty(stack)) {
        return stack->items[stack->top];
    }
    return '\0';
}

// Print stack from top to bottom
void printStack(Stack *stack) {
    printf("Stack (top -> bottom): ");
    for (int i = stack->top; i >= 0; i--) {
        printf("%c ", stack->items[i]);
    }
    printf("\n");
}

// Check if a state is final
int isFinalState(Automaton *automaton, int state) {
    for (int i = 0; i < automaton->finalStateCount; i++) {
        if (automaton->finalStates[i] == state) {
            return 1;
        }
    }
    return 0;
}

// Simplified automaton for balanced parentheses
void setupAutomaton(Automaton *automaton) {
    automaton->initialState = 0;
    automaton->finalStates[0] = 0;
    automaton->finalStateCount = 1;

    // State 0 transitions
    automaton->states[0].transitionCount = 0;

    // ( read, pop any -> push the popped symbol and then (
    automaton->states[0].transitions[automaton->states[0].transitionCount++] = 
        (Transition){'(', 'Z', "(Z", 0};
    automaton->states[0].transitions[automaton->states[0].transitionCount++] = 
        (Transition){'(', '(', "((", 0};
    
    // ) read, pop ( -> push nothing (matching closing parenthesis)
    automaton->states[0].transitions[automaton->states[0].transitionCount++] = 
        (Transition){')', '(', "", 0};
}

int processWord(Automaton *automaton, char *word) {
    Stack stack;
    initStack(&stack);
    push(&stack, 'Z'); // Initial stack symbol

    int currentState = automaton->initialState;
    int i = 0;
    int wordLen = strlen(word);

    printf("\n--- PDA Simulation Start ---\n");

    while (i < wordLen) {
        char currentSymbol = word[i];
        char topSymbol = peek(&stack);

        printf("Current Input Symbol: '%c' | Top of Stack: '%c'\n", currentSymbol, topSymbol);
        printStack(&stack);

        int transitionFound = 0;

        for (int j = 0; j < automaton->states[currentState].transitionCount; j++) {
            Transition t = automaton->states[currentState].transitions[j];
            
            if (t.readSymbol == currentSymbol && t.popSymbol == topSymbol) {
                printf("Transition: (%d, %c, %c) -> (", currentState, t.readSymbol, t.popSymbol);
                
                // Pop symbol from stack
                pop(&stack);
                
                // Push symbols onto stack (in reverse order)
                for (int k = strlen(t.pushSymbol) - 1; k >= 0; k--) {
                    if (t.pushSymbol[k] != 'E') {
                        push(&stack, t.pushSymbol[k]);
                        printf("%c", t.pushSymbol[k]);
                    }
                }
                printf(", %d)\n", t.toState);
                
                currentState = t.toState;
                transitionFound = 1;
                i++; // Move to next input symbol
                break;
            }
        }

        if (!transitionFound) {
            printf("No valid transition from state %d with input '%c' and stack top '%c'\n", 
                   currentState, currentSymbol, topSymbol);
            return 0;
        }
    }

    printf("--- PDA Simulation End ---\n");
    
    // Check if we reached a final state and the stack contains only Z
    int result = (isFinalState(automaton, currentState) && stack.top == 0 && peek(&stack) == 'Z');
    
    printf("Final state: %d (is final: %d ), Stack top: %c, Stack size: %d\n", 
           currentState, isFinalState(automaton, currentState), peek(&stack), stack.top + 1);
    
    return result;
}

int main() {
    Automaton automaton;
    setupAutomaton(&automaton);

    char word[100];
    printf("Enter the string of parentheses: ");
    scanf("%s", word);

    int result = processWord(&automaton, word);

    printf("\nResult: The word '%s' is %s by the automaton.\n",
           word, result ? "ACCEPTED ?" : "REJECTED ?");

    return 0;
}