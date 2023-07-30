; handle dups
cmp          #+6  ,       #-2
cmp L1,       L2
cmp r4,             r14
mov #1, L1
MAIN:  prn    #48
    macro m44
        inc  r1
        mov r1, Z
    endm
    
L1: .data 1,2
L2: .data 1,2
    
.extern MAIN