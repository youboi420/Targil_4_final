; handle dups
 .data 1,2
s: .data 1,2
s: .data 1,2
s: .data 1,2
.string "abcd"
MAIN:  prn    #48
; this is a comment
    macro m44
        inc  r1
        mov r1, Z
    endm
    mov R19, r1
    lea r4, #3
    x: lea L, r2    
    inc r5
    m44
    inc r2
.extern MAIN