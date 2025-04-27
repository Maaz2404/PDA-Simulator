#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  // For portable delay function

#define MAX_TRANSITIONS 100
#define MAX_STACK_SIZE 100
#define MAX_SYMBOL_LENGTH 10
#define DELAY_SECONDS 1 // Delay between steps in seconds

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

// Portable delay function using time.h (works in both Windows and Linux)
void delay(int seconds) {
    clock_t start_time = clock();
    while (clock() < start_time + seconds * CLOCKS_PER_SEC)
        ;  // Empty loop to create delay
}

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

// Print a separator line for better readability
void printSeparator() {
    printf("--------------------------------------------------------\n");
}

// Print stack contents with better formatting
void printStack(Stack *stack) {
    printf("Stack: [");
    for (int i = stack->top; i >= 0; i--) {
        printf("%c", stack->items[i]);
        if (i > 0) {
            printf(", ");
        }
    }
    printf("] (top is leftmost)\n");
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

// Print the remaining input with a cursor showing current position
void printRemainingInput(char *word, int position) {
    printf("Input: ");
    for (int i = 0; i < strlen(word); i++) {
        if (i == position) {
            printf("[%c]", word[i]); // Highlight current symbol
        } else if (i < position) {
            printf(" "); // Space for processed symbols
        } else {
            printf("%c", word[i]);
        }
    }
    printf("\n");
}

// Setup automaton for balanced parentheses
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
    int stepCount = 0;

    printf("\n====== PDA SIMULATION START ======\n");
    printSeparator();
    
    // Print initial configuration
    printf("STEP %d: INITIAL CONFIGURATION\n", stepCount++);
    printf("Current State: q%d\n", currentState);
    printRemainingInput(word, i);
    printStack(&stack);
    printSeparator();
    delay(DELAY_SECONDS);  // Using our portable delay function

    while (i < wordLen) {
        char currentSymbol = word[i];
        char topSymbol = peek(&stack);

        printf("STEP %d: PROCESSING\n", stepCount++);
        printf("Current State: q%d\n", currentState);
        printf("Current Input Symbol: '%c'\n", currentSymbol);
        printf("Top of Stack: '%c'\n", topSymbol);
        printRemainingInput(word, i);
        printStack(&stack);
        
        int transitionFound = 0;

        for (int j = 0; j < automaton->states[currentState].transitionCount; j++) {
            Transition t = automaton->states[currentState].transitions[j];
            
            if (t.readSymbol == currentSymbol && t.popSymbol == topSymbol) {
                // Print transition details
                printf("\nApplying Transition: Delta(q%d, %c, %c) -> (q%d, ", 
                      currentState, t.readSymbol, t.popSymbol, t.toState);
                
                if (strlen(t.pushSymbol) == 0) {
                    printf("E");  
                } else {
                    printf("%s", t.pushSymbol);
                }
                printf(")\n");
                
                // Pop symbol from stack
                printf("Action: Pop '%c' from stack\n", pop(&stack));
                
                // Push symbols onto stack (in reverse order)
                if (strlen(t.pushSymbol) > 0) {
                    printf("Action: Push '");
                    for (int k = strlen(t.pushSymbol) - 1; k >= 0; k--) {
                        printf("%c", t.pushSymbol[k]);
                        push(&stack, t.pushSymbol[k]);
                    }
                    printf("' onto stack\n");
                } else {
                    printf("Action: Push nothing onto stack\n");
                }
                
                printf("Moving to state q%d\n", t.toState);
                currentState = t.toState;
                transitionFound = 1;
                i++; // Move to next input symbol
                break;
            }
        }

        if (!transitionFound) {
            printf("\nERROR: No valid transition from state q%d with input '%c' and stack top '%c'\n", 
                   currentState, currentSymbol, topSymbol);
            printSeparator();
            return 0;
        }
        
        // Print updated configuration after transition
        printf("\nSTEP %d: AFTER TRANSITION\n", stepCount++);
        printf("Current State: q%d\n", currentState);
        printRemainingInput(word, i);
        printStack(&stack);
        
        printSeparator();
        delay(DELAY_SECONDS);  // Using our portable delay function
    }

    printf("\n====== PDA SIMULATION END ======\n");
    
    // Check if we reached a final state and the stack contains only Z
    int result = (isFinalState(automaton, currentState) && stack.top == 0 && peek(&stack) == 'Z');
    
    printf("Final State: q%d %s\n", 
           currentState, isFinalState(automaton, currentState) ? "(Final)" : "(Not Final)");
    printf("Final Stack: ");
    printStack(&stack);
    printf("Acceptance Criteria: Final state (q0) + only 'Z' on stack\n");
    printf("Result: The word '%s' is %s by the automaton.\n",
           word, result ? "ACCEPTED" : "REJECTED");
    
    return result;
}

int main() {
    Automaton automaton;
    setupAutomaton(&automaton);

    char word[100];
    printf("Welcome to the PDA Simulator for Balanced Parentheses!\n");
    printf("This PDA accepts strings of balanced parentheses.\n");
    printf("Enter the string of parentheses: ");
    scanf("%s", word);

    int result = processWord(&automaton, word);

    return 0;
}