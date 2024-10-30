			PRESERVE8
			AREA |.text|, CODE, READONLY


			EXPORT asm_update_myball_array
			IMPORT update_rand
			IMPORT myball_array


asm_update_myball_array 		PROC


			PUSH 			{r4-r5,lr}
			; r4 = i
			; r5 = adresse de myball_array[i]
			MOV 			r4,#0               ; r4=i
			LDR 			r5,=myball_array    ; r5=&myball_array
			B 				TEST_LOOP
			
LAB1                                            ; loop 1
			LDRSH 			r1,[r5,#12]         ; r1 = radius 
			ADDS 			r1,#1               ; r1 += 1
			BNE 			LAB2                ; if(r1!=0){ goto lab2 }
                                                ; only if x was -1
			MOV 			r0,#320             ; r0 = 320
			BL 				update_rand         ; update rand(320)
			STRH 			r0,[r5, #8]        ; x=rand
			MOV 			r0,#240             ; r0=240
			BL 				update_rand         ; update rand(240)
			STRH 			r0,[r5]               ; y= rand
			MOV 			r0,#10              ; r0=10
			BL 				update_rand         ; update_rand(10)
			STR 			r0,[r5,#12]         ; radius=rand
			MOVS			r0,#7               ; r0 = 7
			BL 				update_rand         ; update rand(7)
			ADDS 			r0,#1               ; r0 +=1
			CMP 			r0,#0               ; if r0=0 TODO: can be removed maybe
			MOVEQ 			r1,#0x0000 ;black   
			CMP 			r0,#1
			MOVEQ 			r1,#0x001F ;blue
			CMP 			r0,#2
			MOVEQ 			r1,#0xF800 ;red
			CMP 			r0,#3
			MOVEQ 			r1,#0x07E0 ;green
			CMP 			r0,#4
			MOVEQ 			r1,#0x07FF ;cyan
			CMP 			r0,#5
			MOVEQ 			r1,#0xF81F ;magenta
			CMP 			r0,#6
			MOVEQ 			r1,#0xFFE0 ;yellow
			CMP 			r0,#7
			MOVEQ 			r1,#0xFFFF ;whilte
			STR 			r1,[r5,#4]         ; color = r1  
			B 				END_LOOP          
			
LAB2                                           ; if initialized
			LDR 			r1,[r5,#12]        ; r1 = radius
			ADDS 			r1,#3              ; r1 += 3
			STR 			r1,[r5,#12]        ; radius = radius + 3
			CMP             r1, #40
            BGE             CLEAR_BALL
            LDRSH 			r2,[r5,#8]         ; r2 = x
			CMP 			r1,r2               
			BGE 			CLEAR_BALL         ; if radius >= x clear
			LDRSH 			r3,[r5]              ; r3 = y
			CMP 			r1,r3
			BGE 			CLEAR_BALL         ; if radius >= y clear
			RSB 			r2,r2,#320         ; r2 = 320 - x
			CMP 			r1,r2           
			BGE 			CLEAR_BALL
			RSB 			r3,r3,#240
			CMP 			r1,r3
			BLT 			END_LOOP
			
CLEAR_BALL
			MOV 			r1,#0xFFFFFFFF
			STR 			r1,[r5,#12]        ; radius = -1
			
END_LOOP
			ADDS 			r4,#1               ; i+=1
			ADDS 			r5,#16              ; addr += 16
			
TEST_LOOP 	CMP 			r4,#3           ; end loop 1
			BLT 			LAB1
			POP 			{r4-r5,pc}
			ENDP
			END