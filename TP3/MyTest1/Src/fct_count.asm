            PRESERVE8
            AREA |.data|, DATA
            mycount DCD 0
            AREA |.text|, CODE, READONLY
            EXPORT fct_count
            IMPORT HAL_Delay
fct_count PROC
            PUSH {lr}
            LDR r0,=mycount
            LDR r1,[r0]
            LDR r2,=0x40020814
            STR r1, [r2]
            ADDS r1,r1,#1
            STR r1,[r0]
            MOV r0,#1000
            BL HAL_Delay
            POP {lr}
            BX lr
            ENDP
            END