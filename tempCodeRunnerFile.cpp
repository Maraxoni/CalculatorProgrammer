#include <iostream>
#include <iomanip>
#include <sstream>
#include <bitset>
#include <string.h>

using namespace std;

string calculate_bin(string a, string b, char op) {
    int num1 = stoi(a, nullptr, 2);
    int num2 = stoi(b, nullptr, 2);
    int result;

    asm volatile (
        "mov %1, %%eax \n"   // Move num1 to eax
        "mov %2, %%ebx \n"   // Move num2 to ebx
        "xor %%edx, %%edx \n" // Clear edx for division
        "cmp $'+', %3 \n"    // Compare op with '+'
        "je addition_bin \n"
        "cmp $'-', %3 \n"    // Compare op with '-'
        "je subtraction_bin \n"
        "cmp $'*', %3 \n"    // Compare op with '*'
        "je multiplication_bin \n"
        "cmp $'/', %3 \n"    // Compare op with '/'
        "je division_bin \n"
        "cmp $'<', %3 \n"      // If "<<" jump to left shift
        "je leftshift_bin \n"
        "cmp $'>', %3 \n"      // If ">>" jump to right shift
        "je rightshift_bin \n"
        "cmp $'l', %3 \n"     // If "rol" jump to rotate left
        "je rotateleft_bin \n"
        "cmp $'r', %3 \n"     // If "ror" jump to rotate right
        "je rotateright_bin \n"
        "jmp end_bin \n"            // Jump to end

        "addition_bin: \n"
        "add %%ebx, %%eax \n"  // Add ebx to eax
        "jmp end_bin \n"       // Jump to end

        "subtraction_bin: \n"
        "sub %%ebx, %%eax \n"  // Subtract ebx from eax
        "jmp end_bin \n"       // Jump to end

        "multiplication_bin: \n"
        "imul %%ebx, %%eax \n" // Multiply eax by ebx
        "jmp end_bin \n"       // Jump to end

        "division_bin: \n"
        "idiv %%ebx \n"        // Divide eax by ebx
        "mov %%eax, %0 \n"     // Move quotient to result
        "jmp end_bin \n"       // Jump to end

        "leftshift_bin: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shl %%cl, %%eax \n"     // Shift eax left by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rightshift_bin: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shr %%cl, %%eax \n"     // Shift eax right by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rotateleft_bin: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "rol %%cl, %%eax \n"     // Rotate left eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rotateright_bin: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "ror %%cl, %%eax \n"     // Rotate right eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "end_bin: \n"
        "mov %%eax, %0 \n"     // Move result to output
        : "=r" (result)        // Output - result
        : "r" (num1), "r" (num2), "r" (op) // Input - num1, num2, op
        : "eax", "ebx", "edx"  // Clobbered registers
    );

    string bin_result = bitset<32>(result).to_string(); // Convert integer result to binary string
    size_t first_one = bin_result.find('1'); // Find the index of the first '1'
    return (first_one != string::npos) ? bin_result.substr(first_one) : "0"; // Trim leading zeros
}

float calculate_dec(float x, float y, char op) {
    float result;

    asm volatile (
        "movss %1, %%xmm0 \n"   // Move value of x to xmm0
        "movss %2, %%xmm1 \n"   // Move value of y to xmm1

        "cmp $'+', %3 \n"       // If "+" jump to addition
        "je addition \n"
        "cmp $'-', %3 \n"       // If "-" jump to subtraction
        "je subtraction \n"
        "cmp $'*', %3 \n"       // If "*" jump to multiplication
        "je multiplication \n"
        "cmp $'/', %3 \n"       // If "/" jump to division
        "je division \n"
        "cmp $'<', %3 \n"      // If "<<" jump to left shift
        "je leftshift \n"
        "cmp $'>', %3 \n"      // If ">>" jump to right shift
        "je rightshift \n"
        "cmp $'l', %3 \n"     // If "rol" jump to rotate left
        "je rotateleft \n"
        "cmp $'r', %3 \n"     // If "ror" jump to rotate right
        "je rotateright \n"
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

        "leftshift: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shl %%cl, %%eax \n"     // Shift eax left by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rightshift: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shr %%cl, %%eax \n"     // Shift eax right by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rotateleft: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "rol %%cl, %%eax \n"     // Rotate left eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rotateright: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "ror %%cl, %%eax \n"     // Rotate right eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "end: \n"
        "movss %%xmm0, %0 \n"   // Move output from xmm0 to result
        : "=m" (result)         // Output - result
        : "m" (x), "m" (y), "r" (op) // Input - x, y, op
        : "xmm0", "xmm1"        // SSE registers xmm0 and xmm1 used in SSE instructions
        );

    return result;
}

string calculate_hex(string a, string b, char op) {
    int num1 = stoi(a, nullptr, 16);
    int num2 = stoi(b, nullptr, 16);
    int result;

    asm volatile (
        "mov %1, %%eax \n"   // Move num1 to eax
        "mov %2, %%ebx \n"   // Move num2 to ebx
        "xor %%edx, %%edx \n" // Clear edx for division
        "cmp $'+', %3 \n"    // Compare op with '+'
        "je addition_hex \n"
        "cmp $'-', %3 \n"    // Compare op with '-'
        "je subtraction_hex \n"
        "cmp $'*', %3 \n"    // Compare op with '*'
        "je multiplication_hex \n"
        "cmp $'/', %3 \n"    // Compare op with '/'
        "je division_hex \n"
        "cmp $'<', %3 \n"      // If "<<" jump to left shift
        "je leftshift_hex \n"
        "cmp $'>', %3 \n"      // If ">>" jump to right shift
        "je rightshift_hex \n"
        "cmp $'l', %3 \n"     // If "rol" jump to rotate left
        "je rotateleft_hex \n"
        "cmp $'r', %3 \n"     // If "ror" jump to rotate right
        "je rotateright_hex \n"
        "jmp end_hex \n"     // Jump to end

        "addition_hex: \n"
        "add %%ebx, %%eax \n"  // Add ebx to eax
        "jmp end_hex \n"       // Jump to end

        "subtraction_hex: \n"
        "sub %%ebx, %%eax \n"  // Subtract ebx from eax
        "jmp end_hex \n"       // Jump to end

        "multiplication_hex: \n"
        "imul %%ebx, %%eax \n" // Multiply eax by ebx
        "jmp end_hex \n"       // Jump to end

        "division_hex: \n"
        "idiv %%ebx \n"        // Divide eax by ebx
        "mov %%eax, %0 \n"     // Move quotient to result
        "jmp end_hex \n"       // Jump to end

        "leftshift_hex: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shl %%cl, %%eax \n"     // Shift eax left by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rightshift_hex: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shr %%cl, %%eax \n"     // Shift eax right by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rotateleft_hex: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "rol %%cl, %%eax \n"     // Rotate left eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "rotateright_hex: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "ror %%cl, %%eax \n"     // Rotate right eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp end \n"            // Jump to end

        "end_hex: \n"
        "mov %%eax, %0 \n"     // Move result to output
        : "=r" (result)        // Output - result
        : "r" (num1), "r" (num2), "r" (op) // Input - num1, num2, op
        : "eax", "ebx", "edx"  // Clobbered registers
    );

    stringstream ss;
    ss << hex << uppercase << result; // Convert integer result back to hexadecimal string
    return ss.str();
}

int main() {
    float num1, num2;
    string str1, str2;
    string type;
    char operation;

    cout << "Enter type (bin, dec, hex): ";
    cin >> type;

    if (type == "bin") {

        cout << "Chosen binary. Enter operation(+,-,*,/,>,<,l,r): ";
        cin >> str1 >> operation >> str2;

        if (cin && (operation == '+' || operation == '-' || operation == '*' || operation == '/' || operation == '>' || operation == '<' || operation == 'l' || operation == 'r')) {
            cout << "Result: " << calculate_bin(str1, str2, operation) << "\n";
        }
        else {
            cout << "Invalid input!" << "\n";
        }

    }
    else if (type == "dec") {

        cout << "Chosen binary. Enter operation(+,-,*,/,>,<,l,r): ";
        cin >> num1 >> operation >> num2;

        if (cin && (operation == '+' || operation == '-' || operation == '*' || operation == '/' || operation == '>' || operation == '<' || operation == 'l' || operation == 'r')) {
            float result = calculate_dec(num1, num2, operation);
            cout << "Result: " << setprecision(6) << result << "\n";
        }
        else {
            cout << "Invalid input!" << "\n";
        }

    }
    else if (type == "hex") {

        cout << "Chosen binary. Enter operation(+,-,*,/,>,<,l,r): ";
        cin >> str1 >> operation >> str2;

        if (cin && (operation == '+' || operation == '-' || operation == '*' || operation == '/' || operation == '>' || operation == '<' || operation == 'l' || operation == 'r') &&
            (str1.size() > 0) && (str2.size() > 0)) {
            cout << "Result: " << calculate_hex(str1, str2, operation) << "\n";
        }
        else {
            cout << "Invalid input!" << "\n";
        }

    }
    else {
        cout << "Wrong type";
    }

    return 0;
}
