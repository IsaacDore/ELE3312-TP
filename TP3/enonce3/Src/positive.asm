        PRESERVE8
        AREA |.text|, CODE, READONLY
        EXPORT positive
positive PROC
        LDR r0, [r0]    ; r0 = *value
        ASR r1, r0, #31   ; r1 = y
        EOR r0, r1       ; r0 = *value^y
        SUB r0, r1       ; r0 = (*value^y)-y
        BX LR
        ENDP
        END