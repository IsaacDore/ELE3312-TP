        PRESERVE8
        AREA |.text|, CODE, READONLY
        EXPORT MY_SUM
MY_SUM  PROC
        ADDS R0,R1
        ADDS R0,R2
        ADDS R0,R3
        BX LR
        ENDP
        END
