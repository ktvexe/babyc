#include <stdio.h>
#include "syntax.h"

#define WORD_SIZE 4

void emit_header(FILE *out, char *name) {
    // The assembler requries at least 4 spaces for indentation.
    fprintf(out, "%s\n", name);
}

void emit_insn(FILE *out, char *insn) {
    // The assembler requries at least 4 spaces for indentation.
    fprintf(out, "    %s\n", insn);
}

void write_header(FILE *out) {
    emit_header(out, ".text");
    emit_insn(out, ".global _start\n");
    emit_header(out, "_start:");
    emit_insn(out, "movl    %esp, %ebp");
    emit_insn(out, "subl    $4, %esp");
    emit_header(out, "");
}

void write_footer(FILE *out) {
    emit_insn(out, "mov     %eax, %ebx");
    emit_insn(out, "movl    $1, %eax");
    emit_insn(out, "int     $0x80");
}

void write_syntax(FILE *out, Syntax *syntax, int stack_offset) {
    if (syntax->type == UNARY_OPERATOR) {
        UnarySyntax *unary_syntax = syntax->unary_expression;

        write_syntax(out, unary_syntax->expression, stack_offset);

        if (unary_syntax->unary_type == BITWISE_NEGATION) {
            emit_insn(out, "not     %eax");
        } else {
            emit_insn(out, "test    $0xFFFFFFFF, %eax");
            emit_insn(out, "setz    %al");
        }
    } else if (syntax->type == IMMEDIATE) {
        fprintf(out, "    movl    $%d, %%eax\n", syntax->value);
    } else if (syntax->type == BINARY_OPERATOR) {
        BinarySyntax *binary_syntax = syntax->binary_expression;

        write_syntax(out, binary_syntax->left, stack_offset - WORD_SIZE);
        fprintf(out, "    mov     %%eax, %d(%%ebp)\n", stack_offset);
        write_syntax(out, binary_syntax->right, stack_offset);
        fprintf(out, "    add     %d(%%ebp), %%eax\n", stack_offset);
    }
}

void write_assembly(Syntax *syntax) {
    FILE *out = fopen("out.s", "wb");

    write_header(out);
    write_syntax(out, syntax, -1 * WORD_SIZE);
    write_footer(out);
    
    fclose(out);
}
