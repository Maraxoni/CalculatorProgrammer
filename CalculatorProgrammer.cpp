#include <iostream>
#include <iomanip>
#include <sstream>
#include <bitset>
#include <string.h>

using namespace std;

string float_to_binary(float number) {
    std::stringstream stream;
    stream << std::bitset<sizeof(float) * 8>(*reinterpret_cast<unsigned int*>(&number));
    return stream.str();
}


float binary_to_float(const std::string& binary) {
    std::bitset<sizeof(float) * 8> bits(binary);
    int num = static_cast<int>(bits.to_ulong());
    int* numPtr = reinterpret_cast<int*>(&num);
    return *reinterpret_cast<float*>(numPtr);
}

string float_to_hex(float number) {
    std::stringstream stream;
    stream << std::hex << std::uppercase << *(reinterpret_cast<unsigned int*>(&number));
    return stream.str();
}

float hex_to_float(const std::string& hex_string) {
    union {
        float f;
        unsigned int i;
    } value;
    std::stringstream ss;
    ss << std::hex << hex_string;
    ss >> value.i;
    return value.f;
}

string calculate_bin(string a, string b, char op) {
    float result;
    float x = stof(a);
    float y = stof(b);

    asm volatile (
        "movss %1, %%xmm0 \n"   // Move value of x to xmm0
        "movss %2, %%xmm1 \n"   // Move value of y to xmm1

        "cmp $'+', %3 \n"       // If "+" jump to addition
        "je additionbin \n"
        "cmp $'-', %3 \n"       // If "-" jump to subtraction
        "je subtractionbin \n"
        "cmp $'*', %3 \n"       // If "*" jump to multiplication
        "je multiplicationbin \n"
        "cmp $'/', %3 \n"       // If "/" jump to division
        "je divisionbin \n"
        "cmp $'<', %3 \n"      // If "<<" jump to left shift
        "je leftshiftbin \n"
        "cmp $'>', %3 \n"      // If ">>" jump to right shift
        "je rightshiftbin \n"
        "cmp $'l', %3 \n"     // If "rol" jump to rotate left
        "je rotateleftbin \n"
        "cmp $'r', %3 \n"     // If "ror" jump to rotate right
        "je rotaterightbin \n"
        "jmp endbin \n"            // Jump to end

        "additionbin: \n"
        "addss %%xmm1, %%xmm0 \n" // Add xmm1 to xmm0
        "jmp endbin \n"            // Jump to end

        "subtractionbin: \n"
        "subss %%xmm1, %%xmm0 \n" // Subtract xmm1 from xmm0
        "jmp endbin \n"            // Jump to end

        "multiplicationbin: \n"
        "mulss %%xmm1, %%xmm0 \n" // Multiply xmm0 by xmm1
        "jmp endbin \n"            // Jump to end

        "divisionbin: \n"
        "divss %%xmm1, %%xmm0 \n" // Divide xmm0 by xmm1
        "jmp endbin \n"            // Jump to end

        "leftshiftbin: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shl %%cl, %%eax \n"     // Shift eax left by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp endbin \n"            // Jump to end

        "rightshiftbin: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shr %%cl, %%eax \n"     // Shift eax right by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp endbin \n"            // Jump to end

        "rotateleftbin: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "rol %%cl, %%eax \n"     // Rotate left eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp endbin \n"            // Jump to end

        "rotaterightbin: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "ror %%cl, %%eax \n"     // Rotate right eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp endbin \n"            // Jump to end

        "endbin: \n"
        "movss %%xmm0, %0 \n"   // Move output from xmm0 to result
        : "=m" (result)         // Output - result
        : "m" (x), "m" (y), "r" (op) // Input - x, y, op
        : "xmm0", "xmm1"        // SSE registers xmm0 and xmm1 used in SSE instructions
        );

    return float_to_binary(result);
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
    float result;

    float x = hex_to_float(a);
    float y = hex_to_float(b);

    asm volatile (
        "movss %1, %%xmm0 \n"   // Move value of x to xmm0
        "movss %2, %%xmm1 \n"   // Move value of y to xmm1

        "cmp $'+', %3 \n"       // If "+" jump to addition
        "je additionhex \n"
        "cmp $'-', %3 \n"       // If "-" jump to subtraction
        "je subtractionhex \n"
        "cmp $'*', %3 \n"       // If "*" jump to multiplication
        "je multiplicationhex \n"
        "cmp $'/', %3 \n"       // If "/" jump to division
        "je divisionhex \n"
        "cmp $'<', %3 \n"      // If "<<" jump to left shift
        "je leftshifthex \n"
        "cmp $'>', %3 \n"      // If ">>" jump to right shift
        "je rightshifthex \n"
        "cmp $'l', %3 \n"     // If "rol" jump to rotate left
        "je rotatelefthex \n"
        "cmp $'r', %3 \n"     // If "ror" jump to rotate right
        "je rotaterighthex \n"
        "jmp end \n"            // Jump to end

        "additionhex: \n"
        "addss %%xmm1, %%xmm0 \n" // Add xmm1 to xmm0
        "jmp endhex \n"            // Jump to end

        "subtractionhex: \n"
        "subss %%xmm1, %%xmm0 \n" // Subtract xmm1 from xmm0
        "jmp endhex \n"            // Jump to end

        "multiplicationhex: \n"
        "mulss %%xmm1, %%xmm0 \n" // Multiply xmm0 by xmm1
        "jmp endhex \n"            // Jump to end

        "divisionhex: \n"
        "divss %%xmm1, %%xmm0 \n" // Divide xmm0 by xmm1
        "jmp endhex \n"            // Jump to end

        "leftshifthex: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shl %%cl, %%eax \n"     // Shift eax left by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp endhex \n"            // Jump to end

        "rightshifthex: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "shr %%cl, %%eax \n"     // Shift eax right by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp endhex \n"            // Jump to end

        "rotatelefthex: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "rol %%cl, %%eax \n"     // Rotate left eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp endhex \n"            // Jump to end

        "rotaterighthex: \n"
        "movd %%xmm0, %%eax \n"  // Move the lower 32 bits of xmm0 to eax
        "ror %%cl, %%eax \n"     // Rotate right eax by the value in cl (y)
        "movd %%eax, %%xmm0 \n"  // Move eax back to xmm0
        "jmp endhex \n"            // Jump to end

        "endhex: \n"
        "movss %%xmm0, %0 \n"   // Move output from xmm0 to result
        : "=m" (result)         // Output - result
        : "m" (x), "m" (y), "r" (op) // Input - x, y, op
        : "xmm0", "xmm1"        // SSE registers xmm0 and xmm1 used in SSE instructions
        );

    return float_to_hex(result);
}

int main() {
    float num1, num2;
    string str1, str2;
    string type;
    char operation;

    float input_float = 3.14;
    string input_binary = "010";
    string input_hex = "F";

    if (0) {
        // Konwersja liczby zmiennoprzecinkowej na binarną i z powrotem
        cout << "Liczba zmiennoprzecinkowa: " << input_float << endl;
        cout << "Reprezentacja binarna: " << float_to_binary(input_float) << endl;
        cout << "Konwersja z powrotem: " << binary_to_float(float_to_binary(input_float)) << endl;

        // Konwersja liczby binarnej na zmiennoprzecinkową i z powrotem
        cout << "\nReprezentacja binarna: " << input_binary << endl;
        cout << "Liczba zmiennoprzecinkowa: " << binary_to_float(input_binary) << endl;
        cout << "Konwersja z powrotem: " << float_to_binary(binary_to_float(input_binary)) << endl;

        // Konwersja liczby binarnej na zmiennoprzecinkową i z powrotem
        cout << "\nReprezentacja heksadecymalna: " << input_hex << endl;
        cout << "Liczba zmiennoprzecinkowa: " << hex_to_float(input_hex) << endl;
        cout << "Konwersja z powrotem: " << float_to_hex(hex_to_float(input_hex)) << endl;
    }

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
