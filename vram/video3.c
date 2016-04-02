#include "video.h"

int main(int argc, char** argv) {
    int i, j;
    cls();
    setAttr(0x2e);
    unsigned val = 1;
    for (i=0; i<24; i++) {
        setAttr((i&7)<<4);
        for (j=0; j<i; j++) {
            outc(' ');
        }
        outc(65+i);
        outc(97+i);
        outc(' ');
        outhex(val);
        val += val;
        outc('\n');
    }
    display();
    return 0;
}

