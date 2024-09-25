A simple (linux only) stack based JIT compiled calculator.

# Quickstart
note i would recommend using rlwrap to make the repl more usable:
```bash
bash build.sh
rlwrap ./calc
```

# Examples:

Calculate the next number in the collatz sequence:
```bash
calc) \df$%:x2+1*3x%x2
DEBUG: Allocating 71 bytes for execution (IR size: 13)
calc) \cf 31
94
calc) \cf 4
2
```

calculate $\sqrt{x+x^2}$
```bash
calc) \df+:+*~:~:x
```

# Help Page:

```
def:\d[fc]code  (vars: x y z)
call:\c[fc] x y z
CHAR  PRIMARY  SECONDARY (:)       ADVERBS
+      add      sqrt                  /    - continue until 1 element
-      sub      neg
%      div      mod
*      mul      TBD
~      swap     dup
$      if       TBD
```

# Limitations:

- Integers only no floats strings etc
- Unable to call functions inside other functions
- No proper if statements (always executes both branches)
- Figure out windows support

# Code Overview:
- `a.h` - base macros
- `calc_math.*` - c functions that can be called by the compiled code
- `ir.h` - intermediate representation
- `m.c` - main entry point as well as the compiler itself
- `utility.*` - simple utilities 

additionally on build `asm_data.h` is created which is the contents of `asm/*` after compilation by `nasm`