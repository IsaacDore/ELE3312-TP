PRESERVE8
            AREA |.data|
asm_rand    DCD 0
            AREA |.text|, CODE, READONLY
            EXPORT asm_rand
            EXPORT asm_update_myball_array
            IMPORT myball_array
            IMPORT update_rand
asm_init_myball_array PROC