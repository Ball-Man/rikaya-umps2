#include <tape.e>
#include <tape.h>

#include <umps/arch.h>

/* Function used to get the a tape's structure, indexes goes from 0 to 8 */
extern tapereg_t *tape_get(uint8_t index) {
  return (tapereg_t *) DEV_REG_ADDR(IL_TAPE, index);
}

/* Function used to read a block from the specified tape */ 
extern uint32_t tape_readblock(tapereg_t *tape, void *mem) {
  uint32_t stat;
  
  /* If the device isn't ready(has some error status code or is busy)
   * return the status code
   */
  if (tape->status != TAPE_ST_READY)
    return tape->status;

  /* Set the fields before sending the command */
  tape->data0 = (uint32_t) mem;
  tape->command = TAPE_CMD_READBLK;

  /* Wait for execution */
  while (tape->status == TAPE_ST_BUSY);

  stat = tape->status;
  tape->command = TAPE_CMD_ACK;

  return stat;
}

/* Executes one tape command using the acknowledgement protocol. This 
 * can't be used for read commands since they requires an additional setting
 * (the memory address for the DMA operation), but can be used for all the other
 * commands(reset, skipblk, backblk). Returns the last status from the device
 */
HIDDEN uint32_t tape_command(tapereg_t *tape, uint32_t cmd) {
  uint32_t stat;

  /* If the device isn't ready return */
  if (tape->status != TAPE_ST_READY)
    return tape->status;

  tape->command = cmd;

  /* Wait for execution */
  while (tape->status == TAPE_ST_BUSY);

  stat = tape->status;
  tape->command = TAPE_CMD_ACK;

  return stat;
}

/* Resets the device(rewind the tape to the tape start). Returns the last status from the device */
extern uint32_t tape_rewind(tapereg_t *tape) {
  return tape_command(tape, TAPE_CMD_RESET);
}

/* Skips one block on the tape without reading its content(setting the marker on the next block).
 * Returns the last status from the device
 */
extern uint32_t tape_skipblock(tapereg_t *tape) {
  return tape_command(tape, TAPE_CMD_SKIPBLK);
}

/* Rewinds the tape to the previous block without reading any content.
 * Returns the last status from the device
 */
extern uint32_t tape_backblock(tapereg_t *tape) {
  return tape_command(tape, TAPE_CMD_BACKBLK);
}

/* Returns a non-zero value if the tape is at its start(marker Tape Start) */
extern bool tape_ts(tapereg_t *tape) {
  return tape->data1 == TAPE_MARKER_TS;
}

/* Returns a non-zero value if the tape is at its end(marker End Of Tape) */
extern bool tape_eot(tapereg_t *tape) {
  return tape->data1 == TAPE_MARKER_EOT;
}

/* Returns a non-zero value if the tape is at the end of a file(marker End Of File) */
extern bool tape_eof(tapereg_t *tape) {
  return tape->data1 == TAPE_MARKER_EOF;
}

/* Returns a non-zero value if the tape is at the end of a block(marker End of Block) */
extern bool tape_eob(tapereg_t *tape) {
  return tape->data1 == TAPE_MARKER_EOB;
}
