#ifndef _ASSEMBLE_H
#define _ASSEMBLE_H

#include "symbol.h"
int mnemonic_getbytes(mnemonic *mne, unsigned char *buffer, char *arg, int addr, symbol *symbol_table);
#endif
