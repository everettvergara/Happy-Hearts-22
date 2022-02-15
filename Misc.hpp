#ifndef _MISC_
#define _MISC_

#include <cstdio>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>

int is_key_pressed() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting); 
    return bytesWaiting;
}

#endif