typedef enum LINE_TYPE {EMPTY = 15 , COMMENT, COMMA, OTHER = -1} LINE_TYPE;
typedef enum REG{   r0 = 0x0, 
                    r1,
                    r2,
                    r3,
                    r4,
                    r5,
                    r6,
                    r7,
                    r8,
                    r9,
                    r10,
                    r11,
                    r12,
                    r13,
                    r14,
                    r15
} REG;


int get_mode(char * instr);
unsigned int instruction_to_word(char * instr);
void word_to_string(unsigned int n, char ** str_p);
LINE_TYPE check_line_type(char * buffer);