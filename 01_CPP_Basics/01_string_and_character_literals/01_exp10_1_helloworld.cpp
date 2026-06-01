#include <print>
using namespace std;

int main() {
    // 1+ skips the leading internal newline immediately following the opening delimiter
    print(1 + R"( All in the golden afternoon
        Full leisurely we glide;
For both our oars, with little skill,
        By little arms are plied,
While little hands make vain pretence
        Our wanderings to guide.
)");
}