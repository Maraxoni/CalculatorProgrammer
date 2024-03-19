#include <iostream>
#include <iomanip> // for std::setprecision
#include <sstream> // for std::stringstream
#include <bitset>
#include <string.h>

using namespace std;

string float_to_binary(float num) {
    // Przekształcenie liczby na wskaźnik na bajty
    unsigned char* bytes = reinterpret_cast<unsigned char*>(&num);

    // Pobranie reprezentacji binarnej bajtów
    std::bitset<sizeof(float) * 8> bits;
    for (int i = 0; i < sizeof(float); ++i) {
        for (int j = 0; j < 8; ++j) {
            bits[i * 8 + j] = (bytes[i] >> (7 - j)) & 1;
        }
    }

    // Zwrócenie reprezentacji binarnej jako ciągu znaków
    return bits.to_string();
}

float binary_to_float(const std::string& binary) {
    // Validate binary representation
    for (char bit : binary) {
        if (bit != '0' && bit != '1' && bit != '.') {
            std::cerr << "Invalid binary representation." << std::endl;
            return 0.0f;
        }
    }

    // Find the position of the decimal point
    size_t dotPosition = binary.find('.');

    // Split into integer and fractional parts
    std::string intPart = binary.substr(0, dotPosition);
    std::string fracPart = binary.substr(dotPosition + 1);

    // Convert integer part to int
    int intResult = std::stoi(intPart, nullptr, 2);

    // Convert fractional part to float
    float fracResult = 0.0f;
    float fraction = 0.5f;
    for (char bit : fracPart) {
        if (bit == '1') {
            fracResult += fraction;
        }
        fraction *= 0.5f;
    }

    // Combine integer and fractional parts
    float result = intResult + fracResult;

    return result;
}

string float_to_hex(float number) {
    // Use memcpy for type punning instead of reinterpret_cast
    unsigned int ui;
    memcpy(&ui, &number, sizeof(float));
    stringstream ss;
    ss << hex << ui;
    return ss.str();
}

float hex_to_float(const string& hex_string) {
    // Convert hex string to unsigned int
    unsigned int ui;
    stringstream ss;
    ss << hex << hex_string;
    ss >> ui;

    // Use memcpy for type punning instead of reinterpret_cast
    float result;
    memcpy(&result, &ui, sizeof(float));
    return result;
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
    string input_binary = "0100010010010.000111011011";
    string input_hex = "F.f";
    
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

        cout << "Chosen binary. Enter operation(+,-,*,/): ";
        cin >> str1 >> operation >> str2;

        if (cin && (operation == '+' || operation == '-' || operation == '*' || operation == '/')) {
            cout << "Result: " << calculate_bin(str1, str2, operation) << "\n";
        }
        else {
            cout << "Invalid input!" << "\n";
        }

    }
    else if (type == "dec") {

        cout << "Chosen decimal. Enter operation(+,-,*,/): ";
        cin >> num1 >> operation >> num2;

        if (cin && (operation == '+' || operation == '-' || operation == '*' || operation == '/')) {
            float result = calculate_dec(num1, num2, operation);
            cout << "Result: " << setprecision(6) << result << "\n";
        }
        else {
            cout << "Invalid input!" << "\n";
        }

    }
    else if (type == "hex") {

        cout << "Chosen hexadecimal. Enter operation(+,-,*,/): ";
        cin >> str1 >> operation >> str2;

        if (cin && (operation == '+' || operation == '-' || operation == '*' || operation == '/') &&
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
