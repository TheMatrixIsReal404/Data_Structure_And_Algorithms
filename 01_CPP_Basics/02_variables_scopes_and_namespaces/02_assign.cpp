#include <print>
using namespace std;
 
int main() {
    int i = 1;
    int j{};
    unsigned k = -10;
 
    println("(1) i = {}, j = {}, k = {}", i, j, k);
    i = j;
    j = 3;
    k = -1;
    println("(2) i = {}, j = {}, k = {}", i, j, k);
}