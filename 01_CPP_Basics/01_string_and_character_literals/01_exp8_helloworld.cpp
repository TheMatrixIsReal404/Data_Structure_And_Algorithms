// 01_exp8_helloworld.cpp : Printing currency symbols portably using Unicode sequences
#include <print>
using namespace std;

int main() {
    // Using explicit hex values for the Pound (\u00A3) and Euro (\u20AC)
    println("$(USD) \u00A3(GBP) \u20AC(EUR)");
}