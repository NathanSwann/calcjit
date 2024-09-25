A simple (linux only) JIT compiled calculator.

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

# Limitations:

- Integers only no floats strings etc
- Unable to call functions inside other functions
- No proper if statements (always executes both branches)
- Figure out windows support
