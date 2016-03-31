#include "video.h"

int main(int argc, char** argv) {
    int i, j;
    cls();
    video[2][38][0] = 'h';  video[2][40][0] = 'i';  video[2][42][0] = '!';
    video[2][38][1] = 0x51; video[2][40][1] = 0x43; video[2][42][1] = 0x35;
    // video[3][38][0] = 'h';  video[3][40][0] = 'i';  video[3][42][0] = '!';
    // video[3][38][1] = 0x51; video[3][40][1] = 0x43; video[3][42][1] = 0x35;

    // for (i=0;i<25;i++) {
    //     for (j=0;j<80;j++) {
    //         video[i][j][0] = ' ';  
    //         video[i][j][1] = 0x43; 
    //     }
    // }
    // display();
    cls();
    display();
    return 0;
}

