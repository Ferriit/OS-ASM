# OS-ASM Spec

## Types

### List of all types

```
int                                     -           Integer, whole number;
array                                   -           Dynamic array that can hold multiple types;
string                                  -           An array of ints, representing a string of text;
bool                                    -           True or False;
hashmap                                 -           Key-value store with string/array keys and any-type values;
void                                    -           No value; used for functions that do not return anything;
```

---

## Building Blocks (Instructions)

* Instructions are built-in operations compiled into the interpreter.
* Instructions do not use parentheses and do not return values.
* Each instruction sets the FAIL flag to `1` on failure or `0` on success.

### Standard Instructions

#### io:

```
print [string a]                         -           Prints "a" to stdout;
perr [string a]                          -           Prints "a" to stderr;
getinput [string buf]                    -           Reads user input until [Enter] and stores it in 'buf';
open [int fd, string path]               -           Opens a file and stores descriptor in "fd";
write [int fd]                           -           Writes contents to a file using file descriptor "fd";
read [string buf, int fd]                -           Reads file contents into "buf" using descriptor "fd";
```

#### conv:

```
itoa [string buf, int num, int base]     -           Converts "num" to string in base "base" and stores in "buf";
atoi [int num, string str, int base]     -           Converts "str" to integer considering base "base" and stores in "num";
```

#### array:

* `array` can be substituted with `string`

```
set [array arr, int idx, any data]       -           Sets index "idx" of array "arr" to "data";
get [array buf, array arr, int idx]      -           Gets index "idx" of array "arr" and stores in "buf";
push_back [array arr, any data]          -           Appends "data" to the end of "arr";
pop_back [any buf, array arr]            -           Removes last item from "arr" and stores it in "buf";
```

#### hashmap:

* `hashmap` keys must be of type `string` or `array`

```
h_set [hashmap map, string k, any v]     -           Sets key "k" in "map" to value "v";
h_get [hashmap map, string k, any v]     -           Retrieves value at key "k" in "map" and stores in "v";
h_keys [hashmap map, array buf]          -           Fills "buf" with all keys in "map";
h_values [hashmap map, array buf]        -           Fills "buf" with all values in "map";
```

---

## Functions

* Functions are user-defined routines written in OS-ASM.
* Syntax:

```OS-ASM
func [return type] [function name]([arg type1] [arg1], [arg type2] [arg2], ...) {
    [statements];
}
```

* Functions must have a `return` statement unless their return type is `void`.
* Functions can call other functions or instructions.
* Variables inside functions can use any supported type (`int`, `array`, `string`, `bool`, `hashmap`).

Example:

```OS-ASM
func int add(int a, int b) {
    return a + b;
}

func void main() {
    int result = add(5, 10);
    print result;
}
```

---

## Imports

* Use `using` to include another OS-ASM script into the current script.
* Imported scripts are parsed before execution so their functions can be used immediately.

```OS-ASM
using [scriptname].oas;
```

* Instruction libraries (like `io` or `conv`) do not need imports because they are compiled into the interpreter binary.
* Function libraries written in OS-ASM must be imported if you want to call their functions in another script.

---

## Notes

* No explicit type casting exists — all values are passed as-is.
* Arrays and strings are manipulated only via `get`, `set`, `push_back`, and `pop_back`.
* Semicolons are required for each statement.
* The FAIL flag allows instructions to signal runtime errors without stopping the interpreter. This cannot be set outside of parent instructions (instructions defined in the interpreter).
