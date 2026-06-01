// 01_exp9_helloworld.cpp : Outputting the Pi symbol using its Unicode code point
#include <print>
using namespace std;

int main() {
    // \u03C0 is the universal Unicode hex code for the 'π' symbol
    println("\u03C0 has the value 3.14159...");
}