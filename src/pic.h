#ifndef __PIC_H__
#define __PIC_H__

#include "interrupt.h"

#define MASTER_COMMAND_AND_STATUS_REG	0x20
#define MASTER_INTERMASK_AND_DATA_REG	0x21

#define SLAVE_COMMAND_ANS_STATUS_REG	0xA0
#define SLAVE_INTERMASK_AND_DATA_REG	0xA1

#define EOI                           0x20

void init_pic();

void reset_master_eoi();

void reset_slave_eoi();

#endif
