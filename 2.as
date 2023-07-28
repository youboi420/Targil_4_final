; handle dups
cmp #1,r1
cmp L1, L2

MAIN:  prn    #48
    macro m44
        inc  r1
        mov r1, Z
    endm
.extern MAIN