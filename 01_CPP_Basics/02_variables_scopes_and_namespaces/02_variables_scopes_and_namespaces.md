# 📘 C++ Notes — Chapter 02: Variables, Scopes & Namespaces
  
## Overview
 
This chapter covers how C++ manages **named data** — from the moment you introduce a variable to the rules that govern where it can be seen and how long it lives.
 
---
 
## 🗂️ Core Topics Covered
 
### 1. 🔩 Foundations
- The difference between **declaring**, **defining**, and **assigning** a variable
- The **One Definition Rule (ODR)** — a variable must be defined exactly once
- Why C++ being a **statically typed** language means every variable's memory size (its *storage class*) is known at compile time
---
 
### 2. 🔢 Types & Values
- The full range of built-in numeric types: `int`, `double`, `float`, `long long`, `std::byte`, `bool`
- How their **sizes and precision** differ across platforms
- **Literal prefixes & suffixes** to specify exact types (e.g. `1ULL`, `3.3f`, `0xFF`)
- **Type inference** with `auto` — deduced at compile time, not runtime
---
 
### 3. ⚡ Casts & Safety
- **Narrowing casts** (data loss risk) vs. **widening casts** (safe promotions)
- How **uniform initialization** `{}` blocks dangerous narrowing at compile time
- Making casts explicit and intentional with `static_cast<T>()`
---
 
### 4. 🔭 Scopes
- **Local** vs. **global** variables and their lifetimes
- **Variable shadowing** — a local name temporarily hiding an outer one
- Sub-scopes created by bare `{}` blocks and control structures (`if`, `while`, `for`)
- The `::` **global scope resolution operator** to bypass shadowing
---
 
### 5. 🔗 Lifetime & Linkage
- `static` — makes a global variable **file-private** (invisible to the linker)
- `thread_local` — gives each thread its **own independent copy** of a variable
- **Unnamed namespaces** `namespace { }` — the modern C++ replacement for `static` globals
---
 
### 6. 📦 Namespaces
- Grouping related names to **avoid global clashes**
- **Nested namespaces** using `::` or nested `namespace` blocks
- `using` directives and declarations for convenient access
- The **unnamed namespace** for translation-unit-private symbols
---
 
### 7. 🔒 Constants
- `const` — read-only after initialization, value may be known only at **runtime**
- `constexpr` — strictly **compile-time**, required for array sizes and template parameters
- `static_assert` — zero-cost **compile-time truth checks**, with optional custom error messages
---
 
### 8. 🔗 References
- **L-value references** (`&`) as aliases for existing variables
- `const` references — read-only access without copying
- How `auto` **strips `const` and `&`** unless you explicitly re-add them
---
 
## 🎯 Underlying Theme
 
> C++ gives you **precise control** over where data lives, who can see it, whether it can change, and when it is evaluated.
> This chapter builds the vocabulary and mental model you need to write **safe, predictable, and well-structured** C++ programs.
 
---
 
> **Compiler used in examples:** `g++-14 -std=c++23`
> **Headers used:** `<print>` (for `println()`), `<cmath>`, `<thread>`, `<limits>`
 
 
## 🧭 What is a Translation Unit? *(New — Read First!)*
 
Before diving in, you need to know one key term used throughout this chapter:
 
> A **translation unit** is one `.cpp` source file + all the headers it `#include`s, **compiled together into one `.o` object file**.
 
Think of it as the smallest unit the compiler processes at one time. Multiple translation units are later combined by the **linker** into a final executable. This matters because:
- Variables defined in one translation unit are **not automatically visible** in another.
- Some keywords (`static`, unnamed namespaces) specifically control translation-unit visibility.
```
[first.cpp + headers]  →  compiler  →  first.o  ┐
[second.cpp + headers] →  compiler  →  second.o  ├→ linker → program
[third.cpp + headers]  →  compiler  →  third.o   ┘
```
 
---
 
## 1. Declaration vs. Definition vs. Assignment
 
| Term | What it means |
|------|---------------|
| **Declaration** | Introduces the variable to the program ("I exist with this name and type") |
| **Definition** | Declaration + reserves memory ("allocate space for me") |
| **Assignment** | Sets an initial/new value ("here is my value") |
 
> 🧠 **Key Rule — One Definition Rule (ODR):**
> A variable can be *declared or assigned* many times, but must be **defined exactly once**.
> The same name can exist in **different scopes** without violating the ODR.
 
> 🧠 **The Concept of "Storage Class":**
> C++ is a **statically typed** language — the type of every variable is fixed and fully known at **compile time** (this is true even for `auto` variables, whose types are deduced by the compiler, not at runtime). Because each type is fixed, the compiler also knows exactly how many bytes of memory that variable needs. This specific, fixed amount of memory reserved for a variable is formally called its **storage class**. This applies equally to built-in types (`int`, `double`) and to every user-defined type you will encounter later. The ODR itself exists because of this property: if a variable could be defined multiple times, the compiler wouldn't know how much memory to allocate, or which definition to use.
 
```cpp
int i = 1;    // definition + assignment (most common)
int j{};      // definition with default value (= 0 for int)
unsigned k;   // definition WITHOUT assignment → DANGER: random garbage value (UB!)
```
 
> ⚠️ **Common Error / UB Trap:** Never use an uninitialized variable.
> ```cpp
> unsigned k;
> println("{}", k);  // Undefined Behavior — could print: 16151149 (anything!)
> ```
 
> 💡 **Beginner note:** The difference between declaration and definition matters most when working with **multiple files**. In a single-file program, every declaration is also a definition.
 
### 📄 Program: [02-assign.cpp](/02_variables_scopes_and_namespaces/02_assign.cpp) — Assignments & Undefined Behavior
 
```cpp
// 02-assign.cpp : assign to local variables
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
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 02-assign.cpp -o assign
./assign
```
 
**Output:**
```
(1) i = 1, j = 0, k = 4294967286
(2) i = 0, j = 3, k = 4294967295
```
 
**What this demonstrates:**
- `i = 1` — straightforward assignment.
- `j{}` — default-initialized to `0` (safe!).
- `unsigned k = -10` — **legal but surprising**: `-10` wraps around to `4294967286` (that's 2³² − 10). No error, just modular arithmetic.
- After `i = j`, `i` gets `j`'s value (`0`). They are **not linked** — changing `j` afterward does not change `i`.
- `k = -1` wraps to `4294967295` (= 2³² − 1, all bits set to 1 in binary).

#### 🧪 Experiments (try these!)</summary>
- **Fix k:** Give `k` an initial positive value like `k = 5`. What do you learn about `unsigned`?
    <details>
    <summary>Program with Explanation</summary>
    </details>

- **Break j:** Remove `j`'s initializer (`int j;`). How does that affect `i` at step (2)? (UB — the value could be anything.)
- **ODR check:** Try adding `int j{};` again after it already exists. You'll get a compile error — this is the ODR: one definition per scope.

 
---
 
## 2. The Stack vs. The Heap
 
| Feature | Stack | Heap |
|---------|-------|------|
| Default for | Local variables | Large objects, dynamic allocation |
| Memory released | Automatically (when scope ends) | Manually (`delete`) or via smart pointers |
| Address | Unique per scope | Always unique |
| Speed | Very fast | Slower (allocation overhead) |
| Size limit | Usually a few MB | Much larger (limited by RAM) |
 
> 💡 **Tip:** Two variables with the same name can exist in nested scopes — they have **different addresses** even if same name.
 
> 🧠 **Beginner note:** For now, all variables you'll write are stack variables. The heap (dynamic allocation with `new`/`delete` or smart pointers) becomes important later in the course.
 
---
 
## 3. Identifier Rules (Valid Variable Names)
 
**Must start with:** letter (`a-z`, `A-Z`) or underscore `_`
**Can contain:** letters, digits, underscores — in any order after the first char
 
> 🧠 **These rules apply to ALL identifiers in C++, not just variables.** The exact same naming rules govern:
> - **Classes, structs, enums, and unions** — e.g. `class PlayerStats` or `enum class Color`
> - **Functions** — e.g. `int calculateScore()`
> - **Namespaces** — e.g. `namespace GameEngine`
> - **Macros** — e.g. `#define MAX_SIZE 100`
>
> There is one unified set of identifier rules for the entire language. Your organization may choose different *conventions* for each category (e.g. `SentenceCase` for types, `snake_case` for functions), but the underlying *syntax rules* are the same across all of them.
 
**❌ Reserved / Avoid:**
- C++ keywords (~100 of them: `int`, `auto`, `const`, etc.)
- Names starting with `_CapitalLetter` → reserved for Standard Library
- Names with `__double__underscore` → reserved (name mangling)
- At global scope: any name starting with `_`
> 💡 **Bonus:** Modern C++ compilers (and the C++ standard) also allow **UTF-8 Unicode characters** in identifiers, including as the first character. So `int café = 3;` is technically valid C++23 — but sticking to ASCII is strongly recommended for portability and readability.
 
**Naming conventions table:**
 
| Style | Example | Common Use |
|-------|---------|------------|
| Lower Case | `avariablename` | (avoid — hard to read) |
| Sentence Case | `AVariableName` | User-defined types, classes |
| Upper Case | `AVARIABLENAME` | Macros (avoid macros!) |
| Snake Case | `a_variable_name` | Variables, functions |
| Upper Snake Case | `A_VARIABLE_NAME` | Constants |
| Camel Case | `aVariableName` | Member functions |
 
> 💡 **Common practice:** constants → `UPPER_SNAKE_CASE`, types/classes → `SentenceCase`, functions/variables → `camelCase` or `snake_case`. Pick one and stay consistent in your project.
 
---
 
## 3b. Advanced `print()` and `println()` Formatting
 
These functions do more than just insert a variable at `{}`. Two rules that come up frequently:
 
**Brace Matching — counts must be equal:**
The number of `{}` pairs in your format string must exactly match the number of extra arguments you pass. Too few or too many is a compile-time or runtime error.
```cpp
println("i = {}, j = {}", i, j);    // ✅ 2 braces, 2 args
println("i = {}", i, j);            // ❌ 1 brace, 2 args — error!
println("i = {}, j = {}", i);       // ❌ 2 braces, 1 arg  — error!
```
 
**Escaping Literal Braces:**
If you want to print an actual `{` or `}` character (not a placeholder), you must double it up:
```cpp
println("Set: {{1, 2, 3}}");    // Output: Set: {1, 2, 3}
println("Value: {}", x);        // {} is a placeholder — replaced by x
println("Literal: {{}}");       // Output: Literal: {}
```
 
> 🧠 **Why doubling?** The formatter treats `{` and `}` as special control characters. Doubling them (`{{` and `}}`) is the escape sequence that tells it: "I literally mean a brace here, not a placeholder."
 
---
 
## 4. Casts: Narrowing vs. Widening
 
### 📄 Program: `02-swap.cpp` — Narrowing vs. Widening Casts
 
```cpp
// 02-swap.cpp : attempt to swap the values of an int and a double
#include <print>
using namespace std;
 
int main() {
    int a = 1;
    double b = 2.5;
    println("(1) a = {}, b = {}", a, b);
    a = 2.5;   // narrowing: 2.5 → 2 (fraction silently lost)
    b = 1;     // widening: 1 → 1.0 (safe)
    println("(2) a = {}, b = {}", a, b);
}
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 02-swap.cpp -o swap
./swap
```
 
**Output:**
```
(1) a = 1, b = 2.5
(2) a = 2, b = 1
```
 
**What this demonstrates:**
- `a = 2.5` is a **narrowing cast** — the decimal part `.5` is silently truncated. `a` stays type `int`; only its *value* changes to `2`.
- `b = 1` is a **widening cast** — `1` promotes to `1.0` safely. `b` stays type `double`.
- Neither variable ever changes its type. Types are fixed at definition time.
<details>
<summary>🧪 Experiments (try these!)</summary>
- **Eliminate narrowing:** Change `a = 2.5` to `a = 2` (integer literal). Now there's no cast at all.
- **Integer outputs only:** Change both types to `int`. What happens to `b = 2.5`?
- **Explicit casts:** Replace `a = 2.5` with `a = static_cast<int>(2.5)` and `b = 1` with `b = static_cast<double>(1)`. The output is identical — but now your *intent* is documented in the code.
</details>
```cpp
int a = 1;
double b = 2.5;
 
a = 2.5;  // ⚠️ NARROWING CAST: 2.5 → 2 (fractional part silently lost!)
b = 1;    // ✅ WIDENING CAST: 1 → 1.0 (no information lost)
```
 
**Output:**
```
(1) a = 1, b = 2.5
(2) a = 2, b = 1      ← 'a' silently truncated!
```
 
> ⚠️ **Gotcha:** The **type** of `a` never changes — it stays `int`.
> The compiler just silently chops off `.5`. No error, no warning by default!
 
### Making Casts Explicit
 
```cpp
static_cast<int>(2.5)     // explicit narrowing: clear intent
static_cast<double>(1)    // explicit widening
```
 
> 🧠 **Other C++ casts (brief overview):**
> - `static_cast<T>()` — compile-time type conversion (most common, use this)
> - `reinterpret_cast<T>()` — reinterpret raw bits as a different type (advanced/dangerous)
> - `const_cast<T>()` — remove `const` from a variable (use sparingly)
> - `dynamic_cast<T>()` — safe runtime cast for polymorphic types (Chapter 9+)
> For beginners: just use `static_cast<T>()` for now.
 
---
 
## 5. Uniform Initialization (Modern C++ Best Practice)
 
Use `{}` to initialize variables. It **blocks narrowing casts** at compile time.
 
### 📄 Program: `02-uniform.cpp` — Blocking Narrowing with `{}`
 
```cpp
// 02-uniform.cpp : avoid compiler error with uniform initialization and explicit narrowing cast
#include <print>
using namespace std;
 
int main() {
    //int c = {2.1};                    // ❌ ERROR: narrowing — does NOT compile
    int c = { static_cast<int>(2.5) };  // ✅ OK: explicit cast, intent is clear
    double d = { 1LL };                 // ✅ OK: widening (long long → double is safe)
    println("c = {}, d = {}", c, d);
}
```
 
**Compile & Run:**
```bash
# With narrowing as a hard error:
g++-14 -std=c++23 -Werror=narrowing 02-uniform.cpp -o uniform
 
# Without the extra flag (narrowing may slip through as warning only):
g++-14 -std=c++23 02-uniform.cpp -o uniform && ./uniform
```
 
**Output:**
```
c = 2, d = 1
```
 
**What this demonstrates:**
- `int c = {2.1}` refuses to compile — the `{}` syntax **blocks the narrowing** and tells the compiler: *"I don't want accidental truncation."*
- `static_cast<int>(2.5)` is the explicit escape hatch: you're consciously asking for the truncation.
- `double d = {1LL}` is a **widening** (`long long` → `double`) — always safe, always allowed.
- The `=` sign in uniform initialization is optional: `int c{ static_cast<int>(2.5) }` means exactly the same thing.
> 🧠 **Don't confuse:**
> - `int c{2.5}` → single value in braces → **uniform initialization**
> - `int c[] = {2.5,}` → trailing comma → **array initializer list with one element** — completely different!
 
```cpp
int c = {2.5};                       // ❌ ERROR: narrowing not allowed with {}
int c = { static_cast<int>(2.5) };   // ✅ OK: explicit cast tells compiler you know
double d = { 1LL };                  // ✅ OK: widening (long long → double is safe)
 
// The '=' is actually optional with {}:
int c{ static_cast<int>(2.5) };      // same thing — preferred Modern C++ style
int x{};                             // default-initialized to 0
```
 
> 💡 **Trick:** Compile with `-Werror=narrowing` to make implicit narrowing an error:
> ```bash
> g++-14 -std=c++23 -Werror=narrowing file.cpp -o file
> ```
 
> 🧠 **Remember:** `int c[] = {2.5,}` (trailing comma) = **array with one element** — totally different from `int c{2.5}`!
 
> 💡 **Why `{}` is better than `=` for beginners:**
> - `int x = 3.9;` → silently becomes `3`. No warning!
> - `int x{3.9};` → **compile error**. You're forced to think about it.
> The `{}` style catches bugs before they become runtime surprises.
 
---
 
## 6. Numeric Types Quick Reference
 
| Type | Bits | Range / Notes |
|------|------|---------------|
| `signed char` | 8 | -128 to 127 |
| `unsigned char` | 8 | 0 to 255 |
| `short` | 16 | -32,768 to 32,767 |
| `unsigned short` | 16 | 0 to 65,535 |
| `int` | 32 | -2,147,483,648 to 2,147,483,647 |
| `unsigned` | 32 | 0 to 4,294,967,295 |
| `long` | 64 (32 on 32-bit OS) | same as `long long` on 64-bit |
| `long long` | 64+ | ±9.2 × 10¹⁸ |
| `float` | 32 | ~7 decimal digits precision |
| `double` | 64 | ~15 decimal digits precision (IEEE 754) |
| `long double` | 128 | ~18-19 decimal digits |
 
> 🧠 **Floating-point precision details:**
> The three floating-point types correspond to different levels of precision defined by the **IEEE 754 standard** for floating-point arithmetic:
> - `float` — **Single precision** (32 bits). Stores a real number approximation in 32 bits of memory. Good for graphics and situations where memory is tight and ~7 digits of precision is enough.
> - `double` — **Double precision** (64 bits). The name literally comes from "double precision" as defined in IEEE 754. This is the default floating-point type in C++ and gives ~15-16 significant decimal digits.
> - `long double` — **Extended precision**, typically using 96 or 128 bits of storage (though only 80 bits of actual precision bits are used on many x86 platforms). Offers ~18-19 significant digits.
>
> Note: the *storage class* (memory reserved) and the actual *precision bits used* can differ — for example, `long double` may have 80 bits of precision stored in a 96-bit or 128-bit slot.
| `size_t` | 64 (32 on 32-bit) | unsigned, for sizes/counts — needs `<cstddef>` |
| `ssize_t` | 64 (32 on 32-bit) | signed, negatives = errors — needs `<cstddef>` |
 
> 💡 **Name origin:** `double` = "double-precision" (64-bit IEEE 754 float)
 
> 💡 **Tip — Check limits programmatically with `<limits>`:**
> ```cpp
> #include <limits>
> println("int max = {}", std::numeric_limits<int>::max());      // 2147483647
> println("double min = {}", std::numeric_limits<double>::min()); // smallest positive
> ```
> This is far more reliable than memorizing numbers!
 
> ⚠️ **Memory Best Practice — Don't Always Reach for the Biggest Type:**
> It can be tempting to always use `long long` for integers and `long double` for floats "just to be safe." Resist this habit. Using a type larger than your data actually needs wastes memory and can hurt performance (larger types take more cache space and may require more CPU cycles on some platforms). Choose the smallest type that correctly fits your data:
> ```cpp
> // If you're counting items in a small list (never more than a few thousand):
> short item_count{ 0 };      // ✅ 16 bits is plenty — no need for 64-bit long long
>
> // If you need a large integer (billions):
> long long population{ 8'000'000'000LL };  // ✅ now long long is justified
>
> // For everyday math and science:
> double result{ 3.14 };     // ✅ double is almost always the right float choice
> // long double is rarely needed outside of specialized numerical computation
> ```
 
### Assigning Negative to Unsigned — Surprising but Legal!
 
```cpp
unsigned k = -1;
println("{}", k);   // Output: 4294967295  (that's 2³² - 1 in two's complement!)
```
 
> 🧠 **Why?** `-1` in two's-complement binary (32-bit) = all 1s = `0xFFFFFFFF` = 4,294,967,295 as unsigned. This is legal C++ (but almost always a bug).
 
> **Two's complement in plain English:** Computers store negative numbers by flipping all the bits and adding 1. So `-1` has every bit set to `1`. When read as an unsigned number, all-ones means the maximum value for that type. This is why `unsigned k = -1` gives you `4294967295` (for 32-bit unsigned), not a compiler error.
 
---
 
## 7. `auto` — Type Inference
 
`auto` tells the compiler: **"deduce the type from the right-hand side."**
 
```cpp
int i = 1;       // i is int
auto j = i;      // j is also int (deduced from i)
auto k{ 1.0 };   // k is double (1.0 is a double literal)
auto q;          // ❌ ERROR: auto requires initialization!
```
 
> ⚠️ **Critical:** C++ is **statically typed**. `auto` doesn't change this — the type is still fixed at compile time. It just saves you from writing it out.
 
### Why `auto` is Preferred in Modern C++
 
`auto` is not just about convenience — it actively helps you write **correct, safe, and maintainable** code:
 
- **Avoids accidental narrowing:** `auto x = some_function();` — you get exactly the return type, no silent truncation.
- **Avoids commitment to wrong types:** If a function changes its return type, `auto` variables update automatically.
- **Eliminates verbose type names:** STL types like `std::vector<std::pair<int, std::string>>::iterator` become just `auto`.
- **Works with `const` and `&`:** Combine as `const auto&` for efficient read-only access.
> ⚠️ **`auto` strips `const` and `&`!** When you copy with `auto b = a`, even if `a` is `const`, `b` is **mutable**. You must explicitly add them back if you want them:
> ```cpp
> const auto b = a;    // b is also const
> const auto& c = a;   // c is a const reference (no copy!)
> auto& d = a;         // d is a reference (mutable)
> ```
 
---
 
## 8. Literal Prefixes & Suffixes
 
### Prefixes (base of the number)
 
| Prefix | Base |
|--------|------|
| `0b` or `0B` | Binary |
| `0` | Octal |
| `0x` or `0X` | Hexadecimal |
 
```cpp
int a = 0b1010;   // binary: 10
int b = 0777;     // octal:  511
int c = 0xFF;     // hex:    255
```
 
> 💡 **Hex floating-point literals** also exist (less common): use `p`/`P` as the exponent separator instead of `e`/`E`:
> ```cpp
> double d = 0x1.8p+0;   // hex float = 1.5 (1 + 0.5 in hex)
> ```
 
### Suffixes (type of the literal)
 
| Suffix | Type |
|--------|------|
| `f` / `F` | `float` |
| `l` / `L` | `long` (integer) or `long double` (float) |
| `u` / `U` | `unsigned` |
| `ll` / `LL` | `long long` |
| `uz` / `UZ` | `size_t` |
| `z` / `Z` | `ssize_t` |
 
> ⚠️ **Reserved suffix:** `s` is **not** a numeric suffix! It is used for `std::string` literals (`"hello"s`) and for seconds (`1s`) from `<chrono>`. To use these, add:
> ```cpp
> using namespace std::literals;  // enables "hello"s and 1s etc.
> ```
 
> 🧠 **Standard Library Literals — When and How to Enable Them:**
> The suffixes for built-in types (`f`, `u`, `L`, `LL`, `UZ`, `Z`) work automatically with no extra setup. However, suffixes that come from the **Standard Library** — such as `s` for `std::string` literals (`"hello"s`) or `s` for `std::chrono::seconds` (`5s`) — require you to explicitly opt in:
> ```cpp
> using namespace std::literals;   // enables ALL Standard Library literal suffixes
> ```
> This is also automatically activated if you already have `using namespace std;` in your file. Without it, writing `"hello"s` or `5s` will produce a compile error. Note that this directive is only for the Standard Library suffixes — it is completely separate from the built-in type suffixes.
 
> 💡 **Good news:** In Modern C++, you rarely *need* suffixes on integer literals — they automatically promote (widen) to a type large enough to hold the value. Suffixes matter most when using `auto` or when you need a specific type for a function call.
 
```cpp
auto a = 3.3f;          // float
auto b = 100'000l;      // long (digit separator ' for readability)
auto c = 1'000'000ULL;  // unsigned long long
auto d = 0uz;           // size_t (0)
```
 
> 💡 **Digit separator trick:** Use `'` to group digits:
> ```cpp
> auto million = 1'000'000;   // Same as 1000000, way more readable!
> auto pi = 3.141'592'653'589'793;
> ```
 
> ⚠️ **Note:** Digit separators **only work in source code literals**, not in strings read from input/files.
 
---
 
## 9. `bool` and `std::byte`
 
```cpp
bool success{ true };        // true = 1, false = 0 (NOT -1!)
bool are_equal = (a == b);   // holds result of a condition
```
 
> 🧠 **Beginner note:** `bool` prints as `0` or `1` by default with `println`. To print `true`/`false` as text, use `std::boolalpha`:
> ```cpp
> #include <iostream>
> std::cout << std::boolalpha << success;  // prints: true
> ```
 
```cpp
std::byte b{ 254 };                     // can hold 0-255
auto i = std::to_integer<int>(b);       // convert back to int
// Only bitwise ops supported — no + or - on byte!
```
 
> 🧠 **Technical details of `std::byte`:**
> - **Implementation:** `std::byte` is actually implemented as an `enum class` under the hood (you'll learn about `enum class` in Chapter 6). This is what gives it its strict, limited behavior.
> - **No arithmetic:** Because of this implementation, addition and subtraction are **not allowed** on `std::byte` values. Only bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`) are supported. Trying to write `b + 1` will give a compile error.
> - **Conversion:** To turn a `std::byte` back into an integer you can do math with, you must use the specific conversion function `std::to_integer<int>(b)`. This is intentionally verbose — it forces you to make the conversion explicit. You can compare it conceptually with `static_cast<T>()`.
> ```cpp
> std::byte b{ 200 };
> auto val = std::to_integer<int>(b);   // val = 200 (as a regular int)
> // b + 1;                             // ❌ ERROR: no arithmetic on std::byte!
> b = b | std::byte{ 0x0F };           // ✅ bitwise OR is allowed
> ```
 
> 💡 **When to use `std::byte`:** Use it for raw binary data buffers, not for numeric math. It signals intent: "this is a chunk of memory, not a number."
 
---
 
## 10. Local & Global Scope + Variable Shadowing
 
### 📄 Program: `02-scopes.cpp` — Three Variables, One Name, Three Scopes
 
```cpp
// 02-scopes.cpp : define three variables with the same name in one program
#include <print>
using namespace std;
 
auto a{ 1.5f };   // GLOBAL: float (initialized before main() runs)
 
int main() {
    println("(1) {}", a);   // reads global a
 
    auto a{ 2u };           // LOCAL: shadows the global — type: unsigned
    println("(2) {}", a);   // reads local a
 
    {
        auto a{ 2.5 };      // SUB-SCOPE: shadows local — type: double
        println("(3) {}", a); // reads sub-scope a
    }
    // sub-scope 'a' is destroyed here
 
    println("(4) {}", a);   // back to the local unsigned a
}
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 02-scopes.cpp -o scopes
./scopes
```
 
**Output:**
```
(1) 1.5
(2) 2
(3) 2.5
(4) 2
```
 
**What this demonstrates:**
- Three variables all named `a` exist simultaneously in **three different scopes** — this does **not** violate the ODR because each lives in its own scope with its own memory address.
- Each inner `a` **shadows** (temporarily hides) the outer one until its closing `}`.
- After the sub-scope closes, the local `unsigned a` from step (2) is visible again — giving `2` at step (4).
<details>
<summary>🧪 Experiments (try these!)</summary>
- **Integer literals:** Change to `int a{ 1 }`, `int a{ 2 }`, `int a{ 3 }`. Does this still satisfy the ODR? Yes — different scopes. But `int a{ 2.5 }` gives an error: can't narrow `double → int` with `{}`.
- **Global scope operator:** Change `, a)` to `, ::a)` in all four `println` calls. All four lines now print `1.5` — `::a` always refers to the global `a`, bypassing any shadow.
- **Shadowing warning:** Compile with `-Wshadow` to get a compiler warning each time a variable shadows an outer one.
</details>
```cpp
// 02-scopes.cpp
auto a{ 1.5f };   // GLOBAL: float
 
int main() {
    println("(1) {}", a);   // Output: 1.5  ← global float
 
    auto a{ 2u };           // LOCAL: shadows global — unsigned
    println("(2) {}", a);   // Output: 2    ← local unsigned
 
    {
        auto a{ 2.5 };      // SUB-SCOPE: shadows local — double
        println("(3) {}", a); // Output: 2.5  ← sub-scope double
    }
    // sub-scope 'a' is gone here
 
    println("(4) {}", a);   // Output: 2    ← back to local unsigned
}
```
 
**Full output:**
```
(1) 1.5
(2) 2
(3) 2.5
(4) 2
```
 
> 💡 **Trick — Access global directly with `::`:**
> ```cpp
> println("{}", ::a);   // Always reads the GLOBAL 'a', ignores any local shadow
> ```
 
> 🧠 **ODR check:** Three `a` definitions here don't violate ODR because they live in **three different scopes** — global, local, and sub-scope.
 
> 💡 **Best practice:** Variable shadowing is legal but **confusing**. Enable the `-Wshadow` compiler flag to get warnings whenever you accidentally shadow a variable:
> ```bash
> g++-14 -std=c++23 -Wshadow file.cpp -o file
> ```
 
> 🧠 **Scope lifetime:** Variables defined inside a `{}` block are destroyed when the block ends. The *memory* may not be released until the function returns, but you can no longer access or name the variable after its closing `}`.
 
> 🧠 **Scopes from Control Structures:**
> Sub-scopes that limit variable visibility aren't only created by bare `{}` blocks — they are also **automatically introduced by C++ keywords** like `if`, `while`, `for`, and `switch`. Any variable you define inside one of these constructs loses visibility at the construct's closing `}` and can no longer be referenced:
> ```cpp
> if (true) {
>     int x{ 42 };          // x is created here, inside the if-scope
>     println("{}", x);     // ✅ visible inside the if block
> }
> println("{}", x);         // ❌ ERROR: x is out of scope — gone at the closing }
>
> while (condition) {
>     int temp{ 0 };        // a fresh 'temp' is created each loop iteration
>     // ...
> }
> // temp does not exist here
> ```
> This is identical behavior to a plain `{}` block — the keyword just happens to be providing the scope.
 
---
 
## 11. `static` and `thread_local` Global Variables
 
### `static` at Global Scope — File-Local Visibility
 
### 📄 Programs: `first.cpp` + `second.cpp` + `third.cpp` — Static Across Multiple Files
 
**first.cpp**
```cpp
#include <print>
using namespace std;
 
static int i = 10;   // Only visible within THIS translation unit
 
int first_i() {
    println("This is first i: {}", i);
    return i;
}
```
 
**second.cpp**
```cpp
#include <print>
using namespace std;
 
static int i = 20;   // Also fine — different translation unit, no clash
 
int second_i() {
    println("This is second i: {}", i);
    return i;
}
```
 
**third.cpp**
```cpp
#include <print>
using namespace std;
 
int first_i();
int second_i();
 
int main() {
    println("This is first i: {} and second i: {} from the third:",
            first_i(), second_i());
    return 0;
}
```
 
**Compile & Run (all three files together):**
```bash
g++-14 -std=c++23 first.cpp second.cpp third.cpp -o third
./third
```
 
**Output:**
```
This is second i: 20
This is first i: 10
This is first i: 10 and second i: 20 from the third:
```
 
**What this demonstrates:**
- Both files define `static int i` with different values — no linker error because `static` makes each one invisible to the linker.
- `main()` in `third.cpp` can only access each `i` through the functions `first_i()` and `second_i()` — it has **no direct access** to either `i`.
- Removing `static` from both files causes a **linker error**: `multiple definition of 'i'`.
<details>
<summary>🧪 Experiments (try these!)</summary>
- **Remove `static` from both:** Compile — you'll get `multiple definition of 'i'`. This is exactly what `static` prevents.
- **Remove `static` from only one file:** Does it link? Which `i` does `main` see?
- **Add `static int i = 30;` to `third.cpp`:** Does `main` use this or one of the others?
</details>
```cpp
// first.cpp
static int i = 10;   // Only visible in THIS translation unit
 
// second.cpp
static int i = 20;   // Also fine! Different translation unit, no clash
```
 
> 💡 **Why?** `static` at global scope = "file-private". The **linker** won't see it — so two files can each have `static int i` with different values without a conflict.
 
**Without `static`:** Linker error — `multiple definition of 'i'`
 
> 🧠 **`static` has multiple meanings in C++ depending on context!**
>
> | Context | What `static` means |
> |---------|---------------------|
> | Global variable | File-local visibility (translation unit only) |
> | Local variable inside a function | Preserved between calls (one instance, not recreated) |
> | Class member | Shared by all instances of the class |
>
> Here we're only discussing the global scope meaning. The others come later in the course.
 
### `thread_local` — Per-Thread Variable
 
### 📄 Program: `threadlocal.cpp` — Each Thread Gets Its Own Copy
 
```cpp
#include <print>
#include <thread>
 
// Every thread gets its own personal 'my_tasks' variable
thread_local int my_tasks = 0;
 
void perform_work(std::string name) {
    my_tasks = my_tasks + 1;  // Increments ONLY this thread's copy
    std::println("{} has finished {} task(s).", name, my_tasks);
}
 
int main() {
    // Thread A does the work twice
    std::thread workerA([&]() {
        perform_work("Worker A");
        perform_work("Worker A");
    });
 
    // Thread B does the work once
    std::thread workerB([&]() {
        perform_work("Worker B");
    });
 
    workerA.join();
    workerB.join();
    return 0;
}
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 threadlocal.cpp -o threadlocal
./threadlocal
```
 
**Output** (order may vary — threads run concurrently):
```
Worker B has finished 1 task(s).
Worker A has finished 1 task(s).
Worker A has finished 2 task(s).
```
 
**What this demonstrates:**
- `thread_local int my_tasks = 0` — each thread gets its **own fresh copy** initialized to `0`.
- Worker A increments its counter to 1, then 2. Worker B increments its own separate counter to 1. They never interfere.
- Without `thread_local`, both threads share one `my_tasks` — a **data race**.
```cpp
thread_local int my_tasks = 0;   // Each thread gets its OWN copy
 
void perform_work(std::string name) {
    my_tasks++;   // Only changes THIS thread's copy
    println("{} finished {} task(s).", name, my_tasks);
}
```
 
> ⚠️ **Cost:** `thread_local` variables add initialization overhead every time a new thread is launched (time + memory). Use deliberately.
 
> 💡 **Combining keywords:** A variable can be both `static` and `thread_local`:
> ```cpp
> static thread_local int counter = 0;  // file-local AND per-thread
> ```
 
---
 
## 12. Namespaces — Preventing Name Clashes
 
### 📄 Program: `02-height.cpp` — Same Variable Name, Two Namespaces
 
```cpp
// 02-height.cpp : define the same variable name in two different namespaces
#include <print>
using namespace std;
 
namespace Wonderland {
    auto alice_height_m{ 0.15 };
}
 
namespace VictorianEngland {
    auto alice_height_m{ 0.9 };
}
 
int main() {
    println("Alice's height varies between {}m and {}m",
            Wonderland::alice_height_m,
            VictorianEngland::alice_height_m);
}
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 02-height.cpp -o height
./height
```
 
**Output:**
```
Alice's height varies between 0.15m and 0.9m
```
 
**What this demonstrates:**
- Two variables with **identical names** (`alice_height_m`) exist in the same file without any conflict — each is wrapped in its own namespace.
- The `::` operator selects which namespace's version to use.
- Without namespaces, defining the same name twice at global scope would be an ODR violation.
<details>
<summary>🧪 Experiments (try these!)</summary>
- **Add `using namespace VictorianEngland;` inside `main()`:** Does the output change? (No — fully qualified names still work.)
- **Now remove `VictorianEngland::` from the second argument:** Does it still compile and give the same output? (Yes — `using namespace` makes unqualified `alice_height_m` resolve to `VictorianEngland::alice_height_m`.) This shows that `using namespace` only affects *unqualified* names.
</details>
```cpp
// 02-height.cpp
namespace Wonderland {
    auto alice_height_m{ 0.15 };
}
 
namespace VictorianEngland {
    auto alice_height_m{ 0.9 };
}
 
int main() {
    println("{}m and {}m",
        Wonderland::alice_height_m,         // 0.15
        VictorianEngland::alice_height_m);  // 0.9
}
```
 
**Output:**
```
Alice's height varies between 0.15m and 0.9m
```
 
### `using namespace` — Bringing Names into Scope
 
```cpp
using namespace VictorianEngland;
println("{}", alice_height_m);         // ✅ resolves to VictorianEngland::alice_height_m
println("{}", Wonderland::alice_height_m); // still need full qual for Wonderland
```
 
> 💡 **`using` declaration vs `using namespace` directive:**
>
> | Syntax | What it does |
> |--------|-------------|
> | `using std::cout;` | Imports **one specific name** — safer, preferred |
> | `using namespace std;` | Imports **everything** from `std` — convenient but risky |
>
> Prefer `using std::cout;` over `using namespace std;` when possible. It avoids accidentally pulling in names you didn't intend.
 
> ⚠️ **Never `using namespace std;` in headers** — it pollutes every file that includes that header. In `.cpp` files it's OK, but scoping it inside a function is even cleaner.
 
### Namespaces Are Open
 
A namespace can be **extended from multiple files or multiple places** in the same file. You don't have to define everything in one block:
 
```cpp
// file1.cpp
namespace MyApp {
    int config = 42;
}
 
// file2.cpp
namespace MyApp {        // adding more to the SAME namespace
    void run() { ... }
}
```
 
> ⚠️ **Warning:** You *can* technically add to `namespace std`, but **never do this** — it causes undefined behavior and breaks portability.
 
### Nested Namespaces (two ways)
 
```cpp
// Method 1: nested blocks
namespace Wonderland {
namespace Animals {
    auto white_rabbit{ 1 };
}
}
 
// Method 2: scope resolution (C++17+, cleaner)
namespace Wonderland::Animals {
    auto mouse{ 2 };
}
 
// Both accessed as:
Wonderland::Animals::white_rabbit
Wonderland::Animals::mouse
```
 
### Unnamed Namespace — Modern Replacement for `static`
 
### 📄 Program: `02-namespace.cpp` — Unnamed Namespace in Action
 
```cpp
// 02-namespace.cpp
#include <iostream>
#include <print>
using namespace std;
 
namespace {
    int i = 3000;   // variable i is only visible later within this file
}
 
int main() {
    std::cout << "The Value of i is: " << i << std::endl;
    return 0;
}
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 02-namespace.cpp -o namespace
./namespace
```
 
**Output:**
```
The Value of i is: 3000
```
 
**What happens if you try to access it from another file?**
 
**02_namespace_data.cpp**
```cpp
namespace {
    int i = 3000;
}
```
 
**02_namespace_main.cpp**
```cpp
#include <iostream>
extern int i;   // tries to declare i as external — linker will look for it
 
int main() {
    std::cout << i << std::endl;
    return 0;
}
```
 
**Compile:**
```bash
g++-14 -std=c++23 02_namespace_data.cpp 02_namespace_main.cpp -o namespace_main
```
 
**Linker Error:**
```
/usr/bin/ld: undefined reference to 'i'
collect2: error: ld returned 1 exit status
```
 
**Why?** The unnamed namespace makes `i` completely invisible to the linker. Even declaring `extern int i` cannot reach it — the whole point is that it stays private to its translation unit.
 
```cpp
namespace {
    int i = 3000;   // Only visible within THIS file (translation unit)
}
 
int main() {
    println("{}", i);   // ✅ works — i is visible here in the same file
}
```
 
**What happens if you try to access it from another file?**
 
```cpp
// main2.cpp
extern int i;           // tries to find 'i' externally
println("{}", i);       // ❌ LINKER ERROR!
```
 
**Error:**
```
undefined reference to 'i'
```
 
> 🧠 **Why?** The unnamed namespace makes `i` invisible to the linker. Even `extern` can't reach it. This is the **Modern C++ preferred way** over `static` for file-private globals.
 
> 💡 **What is `extern`?** The `extern` keyword tells the compiler: *"this variable is defined in another translation unit — trust me, the linker will find it."* It is a **declaration without a definition** — no memory is allocated, the linker is expected to find the real definition elsewhere. Here it fails because the unnamed namespace hides `i` from the linker entirely.
> ```cpp
> // main2.cpp
> extern int i;   // declaration only — "i is defined somewhere else"
>                 // linker will search all .o files for a definition
>                 // → fails here because unnamed namespace blocks it
> ```
 
---
 
## 13. `const` — Constants
 
### 📄 Program: `02-constants.cpp` — Defining and Using Constants
 
```cpp
// 02-constants.cpp : introducing the const keyword
#include <print>
using namespace std;
 
const double PI = 3.14159265358979;   // global constant
 
int main() {
    auto const APPROX_E = 3;          // local constant — const before or after type
    println("pi is almost exactly {}, while e is approximately {}",
            PI, APPROX_E);
}
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 02-constants.cpp -o constants
./constants
```
 
**Output:**
```
pi is almost exactly 3.14159265358979, while e is approximately 3
```
 
**What this demonstrates:**
- `const` can be placed **before or after** the type/`auto` keyword — both are valid.
- `PI` is a global constant; `APPROX_E` is local to `main()`.
- Attempting `PI = 3.0;` inside `main()` gives a compile error: `assignment of read-only variable 'PI'`.
- Adding the two constants: `PI + APPROX_E` outputs `6.14159...` — the `int` `APPROX_E` is widened to `double` automatically.
<details>
<summary>🧪 Experiments (try these!)</summary>
- **Try to reassign PI:** Add `PI = 3.0;` inside `main()`. What error do you get?
- **Add the constants:** Try `println("{}", PI + APPROX_E)`. Is the result what you expect given that they have different types?
</details>
```cpp
const double PI = 3.14159265358979;   // global constant
auto const APPROX_E = 3;              // const can go before OR after type/auto
 
// Trying to reassign → compile error:
PI = 3.0;  // ❌ ERROR: assignment of read-only variable 'PI'
```
 
> 💡 **Convention:** Constants in `UPPER_SNAKE_CASE`
 
> 🧠 **Important subtlety:** A `const` value is **not necessarily known at compile time!**
> ```cpp
> int x = 5;
> const int y = x;   // y is const but value is known only at RUNTIME
> ```
> This means `y` **cannot** be used as an array size or template parameter.
> For that, you need `constexpr` (see Section 15).
 
> 🧠 **Why Constants Matter Beyond "Read-Only":**
> Constants aren't just about preventing accidental modification — they are **required** in certain places where plain variables are simply forbidden by the language:
> - **Array sizes** — the size of a C-style array must be a compile-time constant:
>   ```cpp
>   constexpr int SIZE = 10;
>   int scores[SIZE];       // ✅ only works because SIZE is a constexpr constant
>   int n = 10;
>   int scores2[n];         // ❌ Error on most compilers: n is a runtime value
>   ```
> - **Template parameters** — template arguments must also be compile-time constants:
>   ```cpp
>   std::array<int, SIZE> arr;   // ✅ SIZE is constexpr, so this works
>   ```
> - **`static_assert`** — the condition being tested must be evaluable at compile time, which requires `constexpr` values.
>
> This is why the distinction between `const` (may be runtime) and `constexpr` (always compile-time) is so important — only `constexpr` satisfies these requirements.
 
### `const`-Correctness
 
This is the habit of marking every variable, parameter, or reference that **shouldn't be modified** as `const`. It's considered good C++ style:
 
```cpp
const double PI = 3.14159;         // won't change — mark it const
void print_name(const std::string& name) {  // function won't modify name
    println("{}", name);
}
```
 
Benefits: catches bugs at compile time, documents intent, enables compiler optimizations.
 
> 💡 **`const` vs `#define` for constants:**
> ```cpp
> #define PI 3.14159    // ❌ Old C style — no type, no scope, hard to debug
> const double PI = 3.14159;    // ✅ Better — typed and scoped
> constexpr double PI = 3.14159; // ✅✅ Best — compile-time, typed and scoped
> ```
> Always prefer `constexpr` or `const` over `#define` in Modern C++.
 
---
 
## 14. References (`&`) — Aliases for Variables
 
A reference is an **alternative name** for an already-existing variable.
 
### 📄 Program: `02-references.cpp` — L-Value References
 
```cpp
// 02-references.cpp : introducing l-value references
#include <print>
using namespace std;
 
int alice_age{ 9 };   // global variable
 
int main() {
    println("Alice's age is {}", alice_age);      // Output: 9
 
    int& alice_age_ref = alice_age;               // reference — same memory as alice_age
    alice_age_ref = 10;                           // modifying the reference changes the original!
 
    println("Alice's age is now {}", alice_age);  // Output: 10
}
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 02-references.cpp -o references
./references
```
 
**Output:**
```
Alice's age is 9
Alice's age is now 10
```
 
**What this demonstrates:**
- `int& alice_age_ref = alice_age` — `alice_age_ref` is not a copy; it **is** `alice_age`. Both names refer to the same memory location.
- Assigning to the reference (`alice_age_ref = 10`) changes the original variable directly.
- References must be initialized at declaration — `int& ref;` alone is a compile error.
<details>
<summary>🧪 Experiments (try these!)</summary>
- **Change `int` to `auto`:** Does `auto alice_age{ 9 }` and `auto& alice_age_ref = alice_age` still compile? (Yes.)
- **Make `alice_age` const:** Add `const` before `int alice_age`. Does it compile? (No — you can't bind a non-const reference to a const variable.)
- **Make `alice_age_ref` const instead:** Use `const int& alice_age_ref = alice_age`. Does it compile? (Yes — but now you can't assign through the reference.)
- **Remove the `&`:** Change to `int alice_age_ref = alice_age`. Now `alice_age_ref` is a **copy**. Setting it to `10` no longer changes `alice_age`. What does the program print?
</details>
```cpp
int alice_age{ 9 };
int& alice_age_ref = alice_age;   // alice_age_ref IS alice_age (same memory)
 
alice_age_ref = 10;               // changes the original!
println("{}", alice_age);         // Output: 10
```
 
> ⚠️ **References must be initialized when declared** — you can't have an empty reference.
> ⚠️ **Dangling reference** = reference that outlives its variable → **Undefined Behavior**.
 
> 🧠 **l-value vs r-value references (brief intro):**
> There are actually two types of references in C++:
> - **l-value reference** (`&`) — binds to a named variable that "has a location". This is what we use here.
> - **r-value reference** (`&&`) — binds to a temporary value (covered later in the course when discussing move semantics).
> For now, whenever you see `&`, it's an l-value reference.
 
### `const` + `auto` + References — The Full Picture
 
```cpp
const auto a{ 10 };    // a: const int
auto b = a;            // b: int (copy, strips const — b is mutable!)
const auto c = a;      // c: const int (copy, but also const)
 
const auto d{ 11 };
auto e{ 12 };
const auto& f{ 12 };   // f: const ref to literal (compiler extends lifetime)
const auto& g = d;     // g: const ref to d
auto& h = e;           // h: ref to e (mutable)
const auto& i = e;     // i: const ref to e (can read, can't modify via i)
auto& j = d;           // ❌ ERROR: can't bind non-const ref to const variable!
auto& k = f;           // ❌ ERROR: same reason
```
 
> 🧠 **Re-assignable variables from the above:** only `b`, `e`, and `h`
 
> 💡 **Key rule:** `const` and `&` are **stripped** from the right-hand side when assigning to `auto b = a`. You must explicitly put them back on the left side if you want them.
 
> 💡 **Shortcut for performance:** To read a large object without copying it, use `const auto&`:
> ```cpp
> const auto& ref = expensive_object;  // no copy, read-only access
> ```
 
---
 
## 15. `constexpr` — Compile-Time Constants
 
Stronger than `const` — value **must** be known and evaluated **at compile time**.
 
### 📄 Program: `02-constexpr.cpp` — Compile-Time Constants & `static_assert`
 
```cpp
// 02-constexpr.cpp : introducing the constexpr keyword
#include <print>
#include <cmath>
using namespace std;
 
// Note: not all compilers mark `acos` as constexpr in <cmath>.
// This may not compile with clang++ but works with g++-14.
constexpr double PI1 = acos(-1.0);   // computed at compile time
constexpr double PI2 = 22.0 / 7.0;  // simple arithmetic — always constexpr
 
// Compile-time truth tests: if these fail, the program won't compile at all
static_assert(PI1 > 3.141 && PI1 < 3.143);
static_assert(PI2 > 3.141 && PI2 < 3.143);
 
int main() {
    println("PI1 = {}", PI1);
    println("PI2 = {}", PI2);
}
```
 
**Compile & Run:**
```bash
g++-14 -std=c++23 02-constexpr.cpp -o constexpr
./constexpr
```
 
**Output:**
```
PI1 = 3.141592653589793
PI2 = 3.142857142857143
```
 
**What this demonstrates:**
- Both `PI1` and `PI2` are computed entirely **at compile time** — no runtime cost at all.
- `static_assert` runs its test **at compile time**. If the condition is false, the compiler refuses to build the program. Zero overhead in the final binary.
- `acos(-1.0)` = π (exact mathematical identity), giving `PI1` full `double` precision.
- `22.0 / 7.0` is a rational approximation of π — close but slightly off.
<details>
<summary>🧪 Experiments (try these!)</summary>
- **Make `static_assert` fail:** Change to `static_assert(PI2 > 4.0)`. The compiler gives an error immediately — no runtime, no test runner needed.
- **Add a custom message:** `static_assert(PI2 > 4.0, "PI2 must be greater than 4 — check your formula!")`. This makes the error self-documenting.
- **Check e at compile time:** Add `constexpr double E = exp(1.0);` and `static_assert(E > 2.71 && E < 2.72);`. Does it pass?
</details>
```cpp
constexpr double PI1 = acos(-1.0);    // computed at compile time
constexpr double PI2 = 22.0 / 7.0;
 
// static_assert: compile-time truth test (zero runtime cost!)
static_assert(PI1 > 3.141 && PI1 < 3.143);   // ✅ passes silently
static_assert(PI2 > 3.141 && PI2 < 3.143);   // ✅ passes silently
 
// If assertion fails → compile error with optional custom message:
static_assert(PI2 > 4.0, "PI2 is not greater than 4!");  // ❌ compile error
```
 
> 💡 **`static_assert` — Optional Second Parameter (Custom Error Message):**
> `static_assert` can take a second argument: a **string literal** that the compiler will display as the error message if the assertion fails. Without it, the compiler's error message can be cryptic. Always add one — it makes your code self-documenting and easier to debug:
> ```cpp
> static_assert(sizeof(int) == 4, "This code assumes 32-bit int — won't work here!");
> static_assert(PI > 3.141 && PI < 3.143, "PI constant is out of expected range!");
> ```
> The message has **zero runtime cost** — `static_assert` is checked entirely at compile time and disappears from the compiled binary. If the condition is true, no code is generated at all.
 
> 💡 **Always add a message to `static_assert`** — it makes the error self-documenting:
> ```cpp
> static_assert(sizeof(int) == 4, "This code assumes 32-bit int");
> ```
 
**Output:**
```
PI1 = 3.14159265358979
PI2 = 3.14285714285714
```
 
| Keyword | Compile-time? | Can be array size? | Floating point? |
|---------|--------------|-------------------|-----------------| 
| `const` | Not always | Only if known at compile time | ✅ |
| `constexpr` | Always ✅ | Always ✅ | ✅ |
 
> 💡 **Tip:** Use `constexpr` for any constant you know at compile time (math constants, array sizes, config values). It enables compiler optimizations and lets you use `static_assert`.
 
> 🧠 **Metaprogramming sneak peek:** `constexpr` is the foundation of compile-time computation in Modern C++. Later in the course you'll see `constexpr` functions and `if constexpr` for choosing code paths at compile time — all with zero runtime cost.
 
---
 
## 16. Type Aliases — Giving Types a New Name
 
A **type alias** lets you create a shorter or more descriptive name for an existing type.
 
```cpp
using Score = int;              // Score is now an alias for int
using LargeNumber = long long;  // clearer name
 
Score player_score{ 100 };      // same as: int player_score{ 100 };
LargeNumber big{ 9'000'000'000LL };
```
 
> 💡 **When to use:** When a type name is too verbose (especially with templates later), or when you want to make the code's intent clearer. `using` aliases are the **Modern C++ way** — the old C-style `typedef int Score;` does the same thing but is harder to read.
 
> 🧠 **Difference from `auto`:** `auto` deduces the type from a value. `using` just gives a type a new name — there's no value involved.
 
---
 
## 17. The `[[maybe_unused]]` Attribute
 
Sometimes you define a variable you might not always use (e.g. in debug-only code). The compiler will warn about it. Silence the warning intentionally:
 
```cpp
[[maybe_unused]] int debug_counter = 0;  // tells compiler: "yes, I know"
```
 
> 💡 **Use this sparingly.** If a variable is always unused, just remove it. This attribute is for cases where it *might* be used depending on build configuration (e.g., `#ifdef DEBUG`).
 
---
 
## 18. Common Errors Cheatsheet
 
| Error | Cause | Fix |
|-------|-------|-----|
| Random garbage output | Uninitialized variable used | Always initialize: `int x{};` or `int x = 0;` |
| Silent data loss | Narrowing cast `double → int` | Use `static_cast<int>()` or uniform init `{}` |
| `error: narrowing conversion` | `int c = {2.5}` | Use `static_cast` or change type |
| `error: assignment of read-only variable` | Assigning to `const` | Remove `const` or use a different variable |
| `error: 'auto' requires initializer` | `auto q;` with no value | Always assign with `auto`: `auto q = 0;` |
| `undefined reference to 'i'` (linker) | Accessing unnamed namespace var from another file | Keep it in same file — that's the point! |
| `multiple definition of 'X'` (linker) | Two files both define same global without `static` | Use `static` or unnamed namespace |
| Dangling reference | Reference outlives its variable | Keep reference lifetime ≤ variable lifetime |
| `error: cannot bind non-const lvalue ref to rvalue` | `auto& x = 42;` | Use `const auto& x = 42;` or a named variable |
| Unexpected large value for unsigned | Assigned a negative number to `unsigned` | Check your logic; use `signed` if negatives are possible |
| `constexpr` compile error | Using a runtime value in `constexpr` | Only use compile-time values; use `const` instead |
 
---
 
## 19. Summary — Hierarchy of "Constant-ness"
 
```
variable           → can change anytime
const              → fixed after init, may be runtime value
constexpr          → fixed at compile time, usable in static_assert/array sizes
```
 
---
 
## 20. ✅ Best Practices for Beginners
 
These are the habits that separate clean, professional C++ from buggy, hard-to-maintain code:
 
| # | Best Practice | Why |
|---|--------------|-----|
| 1 | **Always initialize every variable** (`int x{};`) | Uninitialized variables → undefined behavior |
| 2 | **Prefer `{}` uniform initialization** | Catches narrowing bugs at compile time |
| 3 | **Use `constexpr` instead of `const`** when value is known at compile time | Enables more compiler optimizations and `static_assert` |
| 4 | **Prefer `constexpr`/`const` over `#define`** | Typed, scoped, and debugger-visible |
| 5 | **Use `auto` for complex or verbose types** | Avoids accidental narrowing; easier to maintain |
| 6 | **Keep variables in the smallest scope possible** | Reduces bugs and makes code easier to reason about |
| 7 | **Never `using namespace std;` in headers** | Pollutes every file that includes the header |
| 8 | **Use unnamed `namespace {}` instead of `static` for file-local globals** | Modern, idiomatic C++ |
| 9 | **Prefer `using X = Type;` over `typedef`** | Cleaner, more readable syntax |
| 10 | **Add messages to `static_assert`** | Self-documenting compile-time checks |
| 11 | **Avoid global mutable state** | Hard to trace bugs; use local variables or namespaces |
| 12 | **Use `UPPER_SNAKE_CASE` for constants** | Universal C++ convention, immediately recognizable |
 
---
 
## 21. 🛠️ Compiler Flags Cheatsheet
 
These flags are your best friends for writing safe C++ code:
 
```bash
# Catch narrowing conversions as errors
g++-14 -std=c++23 -Werror=narrowing file.cpp -o file
 
# Warn when a local variable shadows an outer variable
g++-14 -std=c++23 -Wshadow file.cpp -o file
 
# Enable ALL common warnings (recommended always!)
g++-14 -std=c++23 -Wall -Wextra file.cpp -o file
 
# Enable all warnings + treat them as errors (strictest, great for learning)
g++-14 -std=c++23 -Wall -Wextra -Werror file.cpp -o file
 
# Compile with debug info (for using a debugger like gdb)
g++-14 -std=c++23 -g file.cpp -o file
 
# Compile with optimizations (for release builds)
g++-14 -std=c++23 -O2 file.cpp -o file
```
 
> 💡 **Rule of thumb:** During learning, always compile with `-Wall -Wextra`. It catches many beginner mistakes automatically.
 
---
 
## 22. 🚀 Quick Tricks & Tips Recap
 
| # | Tip |
|---|-----|
| 1 | Always initialize variables — `int x{};` gives you `0`, not garbage |
| 2 | Use `{}` (uniform init) to catch narrowing bugs at compile time |
| 3 | Prefer `auto` for complex types; it avoids accidental narrowing |
| 4 | `::varname` always refers to the **global** version of a name |
| 5 | Use `'` digit separator: `1'000'000` is cleaner than `1000000` |
| 6 | Unnamed `namespace { }` is the modern replacement for `static` globals |
| 7 | `thread_local` has hidden costs — use only when truly needed |
| 8 | `const` doesn't guarantee compile-time — use `constexpr` for that |
| 9 | `static_assert` has zero runtime cost — use it to validate `constexpr` values |
| 10 | `using namespace std;` is OK in `.cpp` files, **never** in headers |
| 11 | Prefer `using X = Type;` over `typedef X Type;` — cleaner Modern C++ syntax |
| 12 | Use `std::numeric_limits<T>::max()` to check type limits at runtime safely |
| 13 | Compile with `-Wall -Wextra` during development to catch subtle bugs |
| 14 | Add `-Wshadow` to your flags to warn whenever a local shadows an outer variable |
| 15 | Use `const auto&` to read a large object cheaply — no copy, no modification |
 
---
 
## 23. 🧪 Self-Check Questions
 
Test yourself without looking at the notes. Answers are below — but try first!
 
**Q1.** What is the difference between `const int y = x;` and `constexpr int y = x;`? When does each fail?
 
**Q2.** What does `auto b = a;` do to `a`'s `const`-ness and reference-ness? How do you preserve them?
 
**Q3.** Why does `unsigned k = -1;` give `4294967295` instead of a compile error?
 
**Q4.** You have two `.cpp` files that both define `int counter = 0;` at global scope. What happens when you link them, and what are two ways to fix it?
 
**Q5.** What is the difference between `using std::cout;` and `using namespace std;`? Which is safer and why?
 
**Q6.** Compile-time or runtime? Classify each:
- `const int a = 5;`
- `const int b = some_function();`
- `constexpr int c = 5;`
- `constexpr int d = some_function();` *(where `some_function` is not `constexpr`)*
**Q7.** What linker error does `static` at global scope prevent? What is the Modern C++ alternative?
 
**Q8.** You write `auto& ref = 42;` — why does this fail? How do you fix it?
 
**Q9.** What does `::a` mean inside a function that has a local variable also named `a`?
 
**Q10.** Name two costs of `thread_local`. When is it appropriate to use it?
 
---
 
<details>
<summary>💡 Answers (expand when ready)</summary>

**A1.** `const int y = x;` — fixed after initialization, but value is determined at **runtime** (when `x` is evaluated). Cannot be used as an array size or template argument. `constexpr int y = x;` — value must be a compile-time constant; if `x` is a runtime value the compiler rejects it. `constexpr` values can be used in `static_assert`, array sizes, and template parameters.
 
**A2.** `auto b = a;` **strips both** `const` and `&` — `b` is a plain mutable copy, even if `a` was `const` or a reference. To preserve them: `const auto b = a;` (keeps const), `const auto& b = a;` (const reference, no copy), `auto& b = a;` (mutable reference, only works if `a` is non-const).
 
**A3.** `-1` in 32-bit two's-complement binary is all 1-bits (`0xFFFFFFFF`). When reinterpreted as an `unsigned int`, all-ones equals 2³²−1 = 4,294,967,295. C++ allows this implicitly (modular arithmetic on unsigned types), but it is almost always a logic bug.
 
**A4.** Linker error: `multiple definition of 'counter'`. Fix 1: add `static` to both — `static int counter = 0;` (file-local visibility). Fix 2: wrap both in an unnamed namespace — `namespace { int counter = 0; }`.
 
**A5.** `using std::cout;` imports **one specific name** — safe, precise, no chance of name collisions. `using namespace std;` imports **everything** from `std` — convenient but can silently clash with your own names (e.g. if you define a function called `count` or `distance`). Never use `using namespace` in headers; in `.cpp` files, `using std::cout;`-style declarations are preferred.
 
**A6.** `a` — compile-time if compiler can see the value (usually yes for simple literals). `b` — **runtime** (depends on function result). `c` — **compile-time** always. `d` — **compile error**: `constexpr` requires a `constexpr`-compatible function.
 
**A7.** Without `static`, defining the same global name in two translation units causes a **linker error**: `multiple definition of 'X'`. Modern C++ alternative: unnamed namespace `namespace { int i = 10; }` — same effect, but idiomatic and works for types and functions too, not just variables.
 
**A8.** `42` is an **r-value** (a temporary with no memory address). A non-const l-value reference (`auto&`) cannot bind to a temporary. Fix: `const auto& ref = 42;` — a `const` reference can bind to a temporary, and the compiler extends its lifetime to match the reference.
 
**A9.** `::a` is the **global scope resolution operator** — it explicitly names the global variable `a`, bypassing any local variable with the same name in the current scope.
 
**A10.** Costs: (1) **initialization overhead** every time a new thread starts (time cost), (2) **memory overhead** — each thread gets its own copy of the variable. Appropriate when: you have per-thread state that must not be shared (e.g. per-thread error codes, per-thread random number generators, per-thread counters like in the example).
 
</details>