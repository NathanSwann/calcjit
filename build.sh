generate_asm() {
    echo -e "#ifndef CALC_ASM_DATA\n#define CALC_ASM_DATA"
    files=$(find asm/ -name '*.asm')
    for asm in $files; do
        nasm $asm 
        xxd -i ${asm%.*}
        rm ${asm%.*}
    done
    echo "#endif"
}

xxd -i help_page > help_data.h
generate_asm > asm_data.h
cc m.c utility.c calc_math.c -g -lm -o calc
