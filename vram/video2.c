#include "video.h"
#include "unistd.h"

int main(int argc, char** argv) {
    int i, j;
    cls();
    video[2][38][0] = 'h';  video[2][40][0] = 'i';  video[2][42][0] = '!';
    video[2][38][1] = 0x51; video[2][40][1] = 0x43; video[2][42][1] = 0x35;

    setAttr(7);
    
    for (i = 0; i < 85; i++) {
        outc('A');
    }
    outc('\n');
    outc('A');
    outc('A');
    outc('\n');
    outc('A');
    outc('A');
    outc('A');
    //outc('\n');
    //outc('\n');

    for (i = 0; i < 21; i++) {
        outc('\n');
        outc('A');
    }
    outc('A');
    outc('\n');
    outc('A');
    outc('A');
    outc('A');
    outc('A');
    display();

    return 0;
}

