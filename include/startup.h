#ifndef STARTUP_H
#define STARTUP_H

extern char _etext, _bstart, _bend, _data, _edata, _estack;

void startup(void);

#endif
