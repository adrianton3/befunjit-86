# befunjit-86

befunjit-86 is a just-in-time compiler for Befunge-93.
It works on Linux x86-64 and it only generates code that follows the System V AMD64 ABI.

This is an educational/for-fun project - I do not recommend using this for any critical infrastructure!\
I've made this project because 1) I wanted to learn how a JIT compiler works and 2) Befunge-93 was designed to be as hard as possible to write a compiler for due to its reflexive nature.
Indeed, a fully AOT compiler is impractical so a JIT compiler is the next best thing.

When run, befunjit-86 traverses a file, following `^<v>#_|?@` and creates a graph of "static" paths.\
Machine code is generated for each path, after which they're linked against each other. Execution starts by calling into the generated machine code.
The generated code for the `p` instruction writes back to the playfield and if it alters cells that are part of any static path then it triggers a re-compile.

Take for example a hello world program:

```
               v
v"Hello World!"<
>v
,:
^_@
```

The graph contains 3 paths:

1. starting from <0, 0, east>, changing directions, going through "!dlroW olleH", `:` and until `_`
2. starting from the location of `_`, going west and coming back to `_`
3. starting from the location of `_`, going east and ending immediately at `@`

All 3 paths can be compiled separately; the jump targets can be patched in after the machine code for each path is laid in memory.


### Optimizations

`^<v>#"` and ` ` do not generate any code and already a compiled path requires less steps/operations than an interpreter.

The first optimizing pass performs constant folding:
+ sequences like `12+` rewrite to `3`; the same is done for `-` and `*`
+ `1+2+` becomes `3+`; the same is done for combinations of `+` and `-`
+ `2*3*` becomes `6*`
+ `abcg*` becomes `bcga*`; this allows strength reduction in a later pass
+ `:+` becomes `2*`
+ `0\-` becomes `(-1)*`
+ `1:` becomes `11`; even though `1:` does not typically appear in befunge source code, it can result from previous fold passes
+ `\\` becomes ``
+ `:$` becomes ``

These are followed by a pass that performs strength reduction:
+ `abg` where `a` and `b` are known at compile time generate much less code than in the general case
+ `abp` where `a` and `b` are known at compile time bypass a few checks
+ `a+` where `a` is known generates fewer instructions than even a simple `+`
+ `a-` where `a` is known generates fewer instructions than even a simple `-`
+ `a*` where `a` is known to be -1, 0, 1, 3, 5 and any powers of two avoids multiplication
+ `\-` generates faster "reverse subtraction" code
+ `` \` `` generates as much code as a simple `` ` ``
+ `` \`! `` generates as much code as a simple `` ` ``
+ `` `! `` generates as much code as a simple `` ` ``

Finally, one more pass aims to bypass the stack alltogether when executing some operations consecutively: `add`, `sub`, `sub-reverse`, `mul`, `square`.


### Performance

The script `./bench/run.sh <befunge-source-file>` runs `perf stat -r 100` on the JIT with a few arguments and the interpreter.

The _mandelbrot_ and _snowflake_ programs were found in the wild:
[mandelbrot](https://codegolf.stackexchange.com/questions/3105/generate-a-mandelbrot-fractal/106527#106527) and
[snowflake](https://codegolf.stackexchange.com/questions/148206/the-quantum-drunkards-walk/148323#148323).

The _count-down-mutate_ and _count-down-mutate-long_ programs were crafted to execute as slow as possible in the JIT.
They were both designed to trigger as many recompilations as possible. 

![perf stat -r 100 task-clock (msec)](https://github.com/user-attachments/assets/448051e7-5f08-4b67-92c4-66f0146fd627)

For the two "natural" programs the JIT is ~21 times and ~15 times faster than the interpreter. The JIT with optimizations is ~2.5 faster than without.

For the two JIT-breaking programs the interpreter is ~20 and ~34 times faster than the JIT.


### Building

```
cmake -S . -B build-release/ -D CMAKE_BUILD_TYPE=Release
cmake --build build-release/
```


### Usage

`b86 <befunge-source-file>`

`b86 --stack-size <size> <befunge-source-file>`


### Testing

Some options that help with debugging and are used by the test runner:

`b86 --read-playfield <befunge-source-file>`
reads a befunge source file and outputs the playfield as it's represented in memory.

`b86 --find-pathlet <befunge-source-file>`
finds the path starting from 0,0 going east and outputs it.

`b86 --find-graph <befunge-source-file>`
traverses the entire source and outputs all reachable paths.

`b86 --run-line <befunge-source-file>`
runs a single line disregarding any of `^<v>#`.
All lines should end with `@`; any of `_|?` should not be used as there is no graph and no paths.

The test runner uses node/JS. Install the test runner dependency via `npm i` and run the tests via `npm test`.
