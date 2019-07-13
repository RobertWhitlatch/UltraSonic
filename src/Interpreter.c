// Interpreter.c

#include "Master.h"

//-------------------getString--------------------//
// Constructs a string from Console input.
// Input: buffer to hold string, length of buffer
// Output: -1 if string not complete, strlen() if enter pressed or length met
//------------------------------------------------//
int getString(char* buff, int buff_length) {
    static int index = 0;
    char ch = EOF;
    while(1){
        ch = fgetc(uart);
        if(ch != EOF) {
            fputc(ch, uart);
            if(ch == '\n' || ch == '\r' || index == buff_length) {
                buff[index] = 0;
                int result = index;
                index = 0;
                return (result);
            } else if(ch == 8) {
                index--;
            } else {
                buff[index] = ch;
                index++;
            }
        }
    }
}

void Interpreter(void) {

    char in_char = EOF;

    CLEAR_TERMINAL
    fprintf(uart, "Welcome to XeroOS!\n>");
    while(1) {
        in_char = fgetc(uart);
        fprintf(uart, "%c\n", in_char);
        switch(in_char) {
            case 'h':
                fprintf(uart,
                        "h - Help Menu\n"
                        "i - Information Page\n"
                        "~ - Clear Screen\n"
                        );
                break;
            case 'i':
                fprintf(uart,
                        "Core Freqency = 80MHz\n"
                        "Interrupts = %s\n",
                        (CheckInterrupts()) ? "False" : "True"
                        );
                break;
            case '~':
                CLEAR_TERMINAL
                break;
            default:
                break;
        }
        fprintf(uart, ">");
    }
}
