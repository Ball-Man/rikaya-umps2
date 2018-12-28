#ifndef FASE0_TAPE_E_
#define FASE0_TAPE_E_

#include <tape.h>
#include <lang.h>

/* Function used to get a tape's structure, indexes goes from 0 to 7
 * Note that this is slightly less efficient that using the right macro
 * to get the register. I'm sacrificing efficiency for this time since
 * I prefer giving the user a function to use instead of a macro.
 */
extern tapereg_t *tape_get(uint8_t index);

/* Function used to read one block from a specified tape. The block is copied into 
 * RAM through DMA
 */
extern uint32_t tape_readblock(tapereg_t *tape, void *mem);

/* Resets the device(rewind the tape to the tape start). Returns the last status from the device */
extern uint32_t tape_rewind(tapereg_t *tape);

/* Skips one block on the tape without reading its content(setting the marker on the next block).
 * Returns the last status from the device
 */
extern uint32_t tape_skipblock(tapereg_t *tape);

/* Rewinds the tape to the previous block without reading any content.
 * Returns the last status from the device
 */
extern uint32_t tape_backblock(tapereg_t *tape);

/* Returns a non-zero value if the tape is at its start(marker Tape Start) */
extern bool tape_ts(tapereg_t *tape);

/* Returns a non-zero value if the tape is at its end(marker End Of Tape) */
extern bool tape_eot(tapereg_t *tape);

/* Returns a non-zero value if the tape is at the end of a file(marker End Of File) */
extern bool tape_eof(tapereg_t *tape);

/* Returns a non-zero value if the tape is at the end of a block(marker End of Block) */
extern bool tape_eob(tapereg_t *tape);

#endif
