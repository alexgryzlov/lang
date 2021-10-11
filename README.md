# lang

C-like language.
Supports basic arithmetic, if/else/while and functions with local or global variables.

```shell script
cmake .
make

./Compiler/xzyc [input_file] [asm_output] [AST_img]                 # produces ASM code
./ASM/compile -i [input_file] -o [output_file] -l (enable listing)  # produces obj file
./ASM/execute [obj_file]                                            # runs   
```

### TODO
Binary translation