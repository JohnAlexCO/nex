#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "tui.c"
#include "error.c"
#include "file.c"

int filelength; int buffersize;
char *buffer;
int edity = 0; int editp = 0; /* viewport y & file position */
#define Height  16
#define Columns 32 /* 3 chars: "00 " */
#define ScreenFill Height*Columns
#define version "nex version 0.0.0, https://Modula.dev"

void printCommand(char *keys, char *command) {
    int len = strlen(keys)+strlen(command);
    printf(txtK bxtW "%s" reset " %s  ", keys, command);
    while(len<20) { printf(" "); len++; }
}

char *insert(int position, char character){
    if (filelength+1 >= buffersize) { buffersize *= 2; buffer = realloc(buffer, buffersize); }
    memcpy(buffer+position+1, buffer+position, filelength-position);
    buffer[position] = character;
    return buffer;
}

char *delete(int position){
    memcpy(buffer+position, buffer+position+1, filelength-position);
    if (filelength <= buffersize/2) { buffersize /= 2; buffer = realloc(buffer, buffersize); }
    return buffer;
}

void save(char *filename) { file_write(filename, buffer, filelength); }

void refresh(){
    tui_clear(); printf(txtK bxtW version reset " File Size %d bytes (%x)\n", filelength, filelength);
    printf(txtP"      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F"reset);
    int c=(edity*Columns); int end=(edity*Columns)+ScreenFill;
    while(c < end && (c < filelength || c == 0)) {
        if (c % Columns == 0) { printf("\n"txtP"%04x  "reset, c); }
        if (c == editp)
            { printf(txtW bxtB blink""); }
        else {
            if (buffer[c]==0) { printf(txtR""); }
            else if (buffer[c]<32 || buffer[c]>126) { printf(txtG""); }
        }
        int h = (buffer[c] >> 4)&15;
        int l = buffer[c] & 15;
        printf("%1x%1x"reset" ", h, l);
        c++;
    }   printf("\n");

    tui_row(20);
    printCommand("Arrows", "Navigate");
    printCommand("Space", "Insert Zeros");
    printCommand("Z", "Delete character");
    printf("\n");
    printCommand("Ctrl+X", "Save");
    printCommand("Ctrl+C", "Quit");
    printf("\n");
}

void quit(){
    terminal_reset_terminal_mode(&original_term);
    tui_clear(); tui_col(0); tui_row(0);
    exit(0);
}

#define initsize 1024 * 1024 * 4
int main(int argc, char **argv) {

    if (argc != 2) { error("nex", "expects a filename as input"); }
    if (argv[1][0] == '-') {
        if (argv[1][1] == 'v') { ok("nex", version "\n"); }
        if (argv[1][1] == 'h') { ok("nex", "\n    nex (filename)      opens and edits a file in hex"); }
    }
    char *filename = argv[1];
    buffer = malloc(sizeof(char)*initsize); buffersize = initsize;
    filelength = file_read(filename, buffer, initsize);
    if (filelength == false) { filelength = 1; buffer[0] = 0; }

    refresh();
    char charcode; bool changed = false; int fileheight = (filelength / Height);
    bool escaped = false; bool leftbracket = false; char value = 1;
    while(true) {
        charcode = tui_input();
        /*if (charcode != 0) { printf("[%x] ", (int)(charcode)); fflush(stdout); }*/
        if (charcode == 0) { usleep(1000); continue; }
        switch(charcode) {
            case 0: break;
            case CTRL_C:        quit(); break;
            case CTRL_X:        save(filename); break;
            case '\e':          escaped = true; break;
            case '[':           leftbracket = true; break;
            case ' ': buffer = insert(editp, '\0'); changed = true; filelength++; break;
            case 'z': if (filelength>0) { buffer = delete(editp); changed = true; filelength--; } break;
            case ARROW_LEFT:
                if (escaped == true && leftbracket == true )
                if (editp > 0) { editp--; } changed=true; break;
            case ARROW_RIGHT:
                if (escaped == true && leftbracket == true )
                editp++; if (editp >= filelength ) { editp = filelength-1; } changed=true; break;
            case ARROW_UP:
                if (escaped == true && leftbracket == true ) {
                if (editp > 31) { editp-=32;}
                if ( edity * Columns > editp ) { edity--; }
                changed=true;
                } break;
            case ARROW_DOWN:
                if (escaped == true && leftbracket == true ) {
                if (editp <= filelength - 32) { editp+=32;}
                if ((edity+Height-1) * Columns < editp ) { edity++; }
                changed=true;
                } break;
            case '0': if (value==1) {value = 0x00;} else { value += 0x00; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '1': if (value==1) {value = 0x10;} else { value += 0x01; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '2': if (value==1) {value = 0x20;} else { value += 0x02; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '3': if (value==1) {value = 0x30;} else { value += 0x03; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '4': if (value==1) {value = 0x40;} else { value += 0x04; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '5': if (value==1) {value = 0x50;} else { value += 0x05; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '6': if (value==1) {value = 0x60;} else { value += 0x06; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '7': if (value==1) {value = 0x70;} else { value += 0x07; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '8': if (value==1) {value = 0x80;} else { value += 0x08; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case '9': if (value==1) {value = 0x90;} else { value += 0x09; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case 'a': if (value==1) {value = 0xA0;} else { value += 0x0A; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case 'b': if (value==1) {value = 0xB0;} else { value += 0x0B; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case 'c': if (value==1) {value = 0xC0;} else { value += 0x0C; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case 'd': if (value==1) {value = 0xD0;} else { value += 0x0D; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case 'e': if (value==1) {value = 0xE0;} else { value += 0x0E; buffer[editp]=value; value=1; editp++; changed=true;} break;
            case 'f': if (value==1) {value = 0xF0;} else { value += 0x0F; buffer[editp]=value; value=1; editp++; changed=true;} break;
            default: escaped = false; leftbracket = false; changed = true;
        }
        if (changed == true) { refresh(); changed = false; }
    }
}