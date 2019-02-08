#ifndef PHASE0_TERMINAL_H_
#define PHASE0_TERMINAL_H_

#include <umps/types.h>

#define TAPE_ST_NOT_INSTALLED   0
#define TAPE_ST_READY           1
#define TAPE_ST_ILLEGAL_OP      2
#define TAPE_ST_BUSY            3
#define TAPE_ST_SKIP_ERROR      4
#define TAPE_ST_READ_ERROR      5
#define TAPE_ST_BACK_ERROR      6
#define TAPE_ST_DMA_ERROR       7

#define TAPE_MARKER_EOT         0
#define TAPE_MARKER_EOF         1
#define TAPE_MARKER_EOB         2
#define TAPE_MARKER_TS          3

#define TAPE_CMD_RESET          0
#define TAPE_CMD_ACK            1
#define TAPE_CMD_SKIPBLK        2
#define TAPE_CMD_READBLK        3
#define TAPE_CMD_BACKBLK        4

#define TAPE_BLOCK_SIZE         4096

typedef dtpreg_t tapereg_t;

#endif
