MAIN:  prn    #48
; this is a comment
    macro m1
        inc  r1
        mov r1, Z
    endm
    lea r4, #3
    x: lea L, r2    
    inc r5
    m1  
                m2 
        inc m3
.entry MAIN
    stop