.SUFFIXES:	.c .h .s .o
.PHONY:		clean
FLAGS=-m32 -g

OBJFILES = videoasm.o videodisp.o

all: video1 video2 video3

video1:	video1.c video.h ${OBJFILES}
	gcc $(FLAGS) -o video1 video1.c videoasm.o videodisp.o

video2:	video2.c video.h ${OBJFILES}
	gcc $(FLAGS) -o video2 video2.c videoasm.o videodisp.o

video3:	video3.c video.h ${OBJFILES}
	gcc $(FLAGS) -o video3 video3.c videoasm.o videodisp.o

videoasm.o:	videoasm.s
	gcc $(FLAGS) -c -Wa,-alsnm=videoasm.lst videoasm.s

videodisp.o:	videodisp.c
	gcc $(FLAGS) -c videodisp.c

clean:
	-rm *.o videoasm.lst video1 video2 video3
