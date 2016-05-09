#ifndef L4_KDEBUG_H
#define L4_KDEBUG_H

#if 0
EXTERNC(void L4_KDB_Enter(const char* message))
#else
#define L4_KDB_Enter(message) \
	do { printf("%s\n", message); getc(); } while (0)
#endif

#endif
