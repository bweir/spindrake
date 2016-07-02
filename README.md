# spindrake

Experiments in using Bison/Flex for Spin parsing

## Design Goals

- Fewer operators
    - Replaced operators
      - `^^x` -> `sqrt(x)`
      - `||x` -> `abs(x)`
      - `x #> y` -> `max(x, y)`
      - `x <# y` -> `min(x, y)`
      - `?x` -> `rand(x)`
      - `=<` -> `<=`
      - `=>` -> `>=`
    - Removed operators
      - `=>=`
      - `=<=`
      - `<=`
      - `>=`
      - `<>=`
      - `===`
      - `and=`
      - `or=`
      - `**`
      - `**=`
      - `x?`
      - `|<`
      - `>|`
      - `+x`
- Single address operator: `@`
- Automatic constant folding
- Automatic string termination (no `string()` keyword)
- `address[0]` -> `address`
- Unused code removal
- Subdirectory support
- Dedicated `ASM` block for assembly

## Specification

### Operators 

From highest precedence to lowest:

1. Unary: `--`, `++`, `~`, `~~`, `@`
1. Unary: `-`, `!`
1. Shift: `<<`, `>>`, `~>`, `<-`, `->`, `><`
1. Bitwise and: `&`
1. Bitwise or: `|`, `^`
1. Multiplicative: `*`, `/`, `//`
1. Additive: `+`, `-`
1. Relational: `==`, `<>`, `<`, `>`, `<=`, `=>`
1. Boolean not: `not`
1. Boolean and: `and`
1. Boolean or: `or`
1. Assignment: `:=`, `+=`, `-=`, `*=`, `/=`, `//=`, `<<=`, `>>=`, `~>=`, `->=`, `<-=`, `><=`, `&=`, `|=`, `^=` 
