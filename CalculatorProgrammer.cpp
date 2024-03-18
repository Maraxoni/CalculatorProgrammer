#include <iostream>

using namespace std;

float calculate(float x, float y, char op) {
    float result;

    asm volatile (
        "movss %1, %%xmm0 \n"   // Przenoszenie wartości x do xmm0
        "movss %2, %%xmm1 \n"   // Przenoszenie wartości y do xmm1

        "cmp $'+', %3 \n"       // If "+" jump to addition
        "je addition \n"
        "cmp $'-', %3 \n"       // If "-" jump to subtraction
        "je subtraction \n"
        "cmp $'*', %3 \n"       // If "*" jump to multiplication
        "je multiplication \n"
        "cmp $'/', %3 \n"       // If "/" jump to division
        "je division \n"
        "jmp end \n"            // Jump to end

        "addition: \n"
        "addss %%xmm1, %%xmm0 \n" // Add xmm1 to xmm0
        "jmp end \n"            // Jump to end

        "subtraction: \n"
        "subss %%xmm1, %%xmm0 \n" // Subtract xmm1 from xmm0
        "jmp end \n"            // Jump to end

        "multiplication: \n"
        "mulss %%xmm1, %%xmm0 \n" // Multiply xmm0 by xmm1
        "jmp end \n"            // Jump to end

        "division: \n"
        "divss %%xmm1, %%xmm0 \n" // Divide xmm0 by xmm1
        "jmp end \n"            // Jump to end

        "end: \n"
        "movss %%xmm0, %0 \n"   // Moving output form xmm0 to result
        : "=m" (result)         // Output - result
        : "m" (x), "m" (y), "r" (op) // Input - x, y, op
        : "xmm0", "xmm1"        // SSE registers xmm0 i xmm1 used in SSE instructions
        );

    return result;
}

int main() {
    float num1, num2;
    char operation;

    cout << "Enter operation(+,-,*,/): ";
    cin >> num1 >> operation >> num2;

    if (cin && (operation == '+' || operation == '-' || operation == '*' || operation == '/')) {
        float result = calculate(num1, num2, operation);

        cout << "Result: " << result << "\n";
    }
    else {
        cout << "Invalid input!" << endl;
    }

    return 0;
}