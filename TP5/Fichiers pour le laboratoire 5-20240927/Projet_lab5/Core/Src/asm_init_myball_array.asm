AREA |.text|, CODE, READONLY
EXPORT asm_init_myball_array
IMPORT myball_array
asm_init_myball_array PROC
        LDR     r0, =myball_array
        MOV     r1, #60
        STR     r1, [r0],   #4
        STR     r1, [r0],   #4
        MOV     r1, #30
        STRH    r1, [r0],   #2
        MOV     ,   #0x1F       ;blue
        STRH    r1, [r0],   #2
        MOV     r1, #80
        STR     r1, [r0],   #4
        STR     r1, [r0],    #4
        MOV     r1, #30
        STRH    r1, [r0],   #2
        MOV     r1, #0x07E0 ;green
        STRH    r1, [r0],   #2
        MOV     r1, #100
        STR     r1, [r0],   #4
        STR     r1, [r0],    #4
        MOV     r1, #30
        STRH    r1, [r0],   #2
        MOV     r1, #0xF800 ;red
        STRH    r1, [r0],   #2
        BX      lr
ENDP
END