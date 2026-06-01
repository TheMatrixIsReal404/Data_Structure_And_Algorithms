// 01_exp12_helloworld.cpp : Custom centered title layout based on an 80-character width
#include <print>
using namespace std;

int main() {
    // 1+ skips the accidental internal opening newline character 
    print(1 + R"(
                                   THE MATRIX
                                     (1999)

                                   written by
                                THE WACHOWSKIS
)");
}