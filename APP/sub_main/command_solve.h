#ifndef __COMM_SOL_H
#define __COMM_SOL_H

#include "main.h"
void receive_bytes(uint8_t *d, uint32_t len);
void command_process(uint8_t *d, uint32_t len);
#endif
