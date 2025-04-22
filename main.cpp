#include <iostream>
#include <stack>
#include <string>
using namespace std;

bool isBalanced(const string& input) {
    stack<char> stack;

    for (char ch : input) {
        if (ch == '(') {
            // Push opening parenthesis onto the stack
            stack.push(ch);
        } else if (ch == ')') {
            // If stack is empty or top isn't an opening parenthesis, it's unbalanced
            if (stack.empty() || stack.top() != '(') {
                return false;
            }
            // Pop the matched opening parenthesis
            stack.pop();
        }
        // Ignore other characters (if any)
    }

    // If stack is empty, all parentheses were matched
    return stack.empty();
}

int main() {
    string input;
    cout << "Enter a string of parentheses: ";
    getline(cin, input);

    if (isBalanced(input)) {
        cout << "The parentheses are balanced.\n";
    } else {
        cout << "The parentheses are not balanced.\n";
    }

    return 0;
}
