.data 1,2
MAIN:  prn    #48
; this is a comment
    macro m1
        inc  r1
        mov r1, Z
    endm
    mov R19, r1
    lea r4, #3
    x: lea L, r2    
    inc r5
    ZZ
    m1  
                m2 
        inc m3
        inc R55
.entry         MAIN
.extern ZZ
    stop  
M: .data 1,2,3
rts
K: .string "abcd"
l: .data 1,2
p: .data 1,2
inc r2