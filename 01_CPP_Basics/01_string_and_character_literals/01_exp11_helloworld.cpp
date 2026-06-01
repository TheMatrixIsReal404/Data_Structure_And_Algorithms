// 01_exp10_2_helloworld.cpp : Concatenating standard string literals at compile time
#include <print>
using namespace std;

int main() {
    // Adjacent string literals are automatically joined by the compiler
    print(
        "All in the golden afternoon\n"
        "\tFull leisurely we glide;\n"
        "For both our oars, with little skill,\n"
        "\tBy little arms are plied,\n"
        "While little hands make vain pretence\n"
        "\tOur wanderings to guide.\n"
    );
}