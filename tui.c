// REQUIRES <stdio.h> <termios.h> <sys/select.h>

#ifndef TUI
    #define TUI
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <termios.h>
    #include <sys/select.h>
#endif

#define CTRL_C      3
#define CTRL_D      4
#define CTRL_Q      17
#define CTRL_X      24
#define CTRL_ESC    27
#define BACKSPACE   8 /* '\b' */
#define ARROW_LEFT  0x44
#define ARROW_RIGHT 0x43
#define ARROW_UP    0x41
#define ARROW_DOWN  0x42

#define block_100   "█"
#define block_75    "▓"
#define block_50    "▒"
#define block_25    "░"
#define block_half  "▄"
#define block_check "▚"

#define txtK        "\e[0;30m" // black
#define txtR        "\e[0;31m"
#define txtG        "\e[0;32m"
#define txtY        "\e[0;33m"
#define txtB        "\e[0;34m"
#define txtP        "\e[0;35m"
#define txtC        "\e[0;36m"
#define txtW        "\e[0;37m"
#define bxtR        "\e[41m"
#define bxtG        "\e[42m"
#define bxtY        "\e[43m"
#define bxtB        "\e[44m"
#define bxtP        "\e[45m"
#define bxtC        "\e[46m"
#define bxtW        "\e[47m"
#define reset       "\e[0m"
#define blink       "\e[5m"

void tui_return() { printf("\e[0F"); }
void tui_row(int y) { printf("\e[H\e[%dB", y); }
void tui_col(int x) { printf("\e[%dG", x); }
void tui_position(int x, int y) { tui_row(y); tui_col(x); }
void tui_clear() { tui_position(0,0); printf("\e[0J"); }

void tui_loadbar(int x, char *colorA, char *colorB) {
    printf("\n"); tui_col(0); printf(colorA);
    int i; for (i=0; i < 80; i++) {
        if ( i > x ) { printf(colorB); }
        printf(block_100);
    }   printf(txtW "\n");
}

// http://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html#user-input

/*
void terminal_set_raw_mode() { struct termios term; tcgetattr(STDIN_FILENO, &term); term.c_lflag &= ~(ICANON | ECHO); tcsetattr(STDIN_FILENO, TCSANOW, &term); }
void terminal_reset_terminal_mode(struct termios *original_term) { tcsetattr(STDIN_FILENO, TCSANOW, original_term); }
char tui_input() {
    // Save the original terminal attributes
    struct termios original_term; tcgetattr(STDIN_FILENO, &original_term); terminal_set_raw_mode();

    char ch = 0; struct timeval tv; fd_set readfds;
    FD_ZERO(&readfds); FD_SET(STDIN_FILENO, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0; // Set timeout to 0 for non-blocking

    int retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
    if (retval == -1) { perror("select");
    } else if (retval) { read(STDIN_FILENO, &ch, 1); }
    terminal_reset_terminal_mode(&original_term);
    return ch;
}
*/
void terminal_set_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void terminal_reset_terminal_mode(struct termios *original_term) {
    tcsetattr(STDIN_FILENO, TCSANOW, original_term);
}

struct termios original_term;
char tui_input() {
    //struct termios original_term;
    tcgetattr(STDIN_FILENO, &original_term);
    terminal_set_raw_mode();

    char ch = 0;
    struct timeval tv;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
    if (retval == -1) {
        perror("select");
    } else if (retval) {
        read(STDIN_FILENO, &ch, 1);
    }

    terminal_reset_terminal_mode(&original_term);
    return ch;
}