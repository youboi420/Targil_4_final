Z: .data 1,2
MAIN:  prn    #48
cmp M, #1
cmp #1, #2
cmp r12, r2
; this is a comment


    macro m1
        inc  r1
        mov r1, Z
    endm
    
    mov MAIN, r1
    lea r4, #3
    x: lea L, r2    
    inc r5
    m1  
                m2 
        inc m3
        inc R55
.entry  MAIN
.entry Z
    stop  
M: .data 1,2,3
rts
K: .string "abcd"
l: .data 1,2
p: .data 1,2