#include <umps/arch.h>
#include <umps/types.h>
#include <umps/libumps.h>
#include <terminal.e>
#include <scheduler.e>
#include <log.e>
#include <init.e>
#include <sysbp.e>
#include <interrupt.e>
#include <pcb.e>
#include <time.e>
#include <sysbp.h>
#include <interrupt.h>
#include <lang.h>
#include <list.h>
#include <const.h>
#include <sysbp.h>


/* ****************************************** *
 * *                TESTS                   * *
 * ****************************************** *
 */

#define TRANSMITTED 5
#define TRANSTATUS 2
#define ACK 1
#define PRINTCHR 2
#define CHAROFFSET 8
#define STATUSMASK 0xFF
#define TERM0ADDR 0x10000250
#define DEVREGSIZE 16
#define READY 1
#define DEVREGLEN 4
#define TRANCOMMAND 3
#define BUSY 3
#define DEV_PER_INT 8 /* Maximum number of devices per interrupt line */

#define STEPS 6

int test1_baton[STEPS + 1] = {0};
int test2_baton[STEPS + 1] = {0};
int test3_baton[STEPS + 1] = {0};

/******************************************************************************
 * I/O Routines to write on a terminal
 ******************************************************************************/

/* This function returns the terminal transmitter status value given its address */
devreg termstat(memaddr *stataddr)
{
  return ((*stataddr) & STATUSMASK);
}

/* This function prints a string on specified terminal and returns true if 
 * print was successful, false if not   */
unsigned int termprint(char *str, unsigned int term)
{

  memaddr *statusp;
  memaddr *commandp;

  devreg stat;
  devreg cmd;

  unsigned int error = false;

  if (term < DEV_PER_INT)
  {
    /* terminal is correct */
    /* compute device register field addresses */
    statusp = (devreg *)(TERM0ADDR + (term * DEVREGSIZE) + (TRANSTATUS * DEVREGLEN));
    commandp = (devreg *)(TERM0ADDR + (term * DEVREGSIZE) + (TRANCOMMAND * DEVREGLEN));

    /* test device status */
    stat = termstat(statusp);
    if ((stat == READY) || (stat == TRANSMITTED))
    {
      /* device is available */

      /* print cycle */
      while ((*str != '\0') && (!error))
      {
        cmd = (*str << CHAROFFSET) | PRINTCHR;
        *commandp = cmd;

        /* busy waiting */
        while ((stat = termstat(statusp)) == BUSY)
          ;

        /* end of wait */
        if (stat != TRANSMITTED)
        {
          error = true;
        }
        else
        {
          /* move to next char */
          str++;
        }
      }
    }
    else
    {
      /* device is not available */
      error = true;
    }
  }
  else
  {
    /* wrong terminal device number */
    error = true;
  }

  return (!error);
}

/* This function places the specified character string in okbuf and
 *  causes the string to be written out to terminal0 */
void addokbuf(char *strp)
{
  termprint(strp, 0);
}

/* This function places the specified character string in errbuf and
 *  causes the string to be written out to terminal0.  After this is done
 *  the system shuts down with a panic message */
void adderrbuf(char *strp)
{

  termprint(strp, 0);

  PANIC();
}

char *toprint[] = {
  "1                        \n",
  "2          _nnnn_        \n",
  "3         dGGGGMMb       \n",
  "4        @p~qp~~qMb      \n",
  "5        M|@||@) M|      \n",
  "6        @,----.JM|      \n",
  "7       JS^\\__/  qKL     \n",
  "8      dZP        qKRb   \n",
  "9     dZP          qKKb  \n",
  "10   fZP            SMMb \n",
  "11   HZM            MMMM \n",
  "12   FqM            MMMM \n",
  "13 __| '.        |\\dS'qML\n",
  "14 |    `.       | `' \\Zq\n",
  "15_)      \\.___.,|     .'\n",
  "16\\____   )MMMMMP|   .'  \n",
  "17     `-'       `--'    \n",
  "18                       \n",
};

void test1()
{
  int i = 0;
  termprint("Entering test1!\n", 0);
  for (i = 0; i < STEPS; i++)
  {
    while (test3_baton[i] == 0)
      ;

    termprint(toprint[i * 3], 0);
    delay_ms((getRANDOM() >> 8) * 100);
    test1_baton[i] = 1;
  }
  while (test3_baton[STEPS] == 0)
    ;
  termprint("Good job from test1\n", 0);
  test1_baton[STEPS] = 1;
  SYSCALL(SYS3, 0, 0, 0);
}

void test2()
{
  int i = 0;
  termprint("Entering test2!\n", 0);
  for (i = 0; i < STEPS; i++)
  {
    while (test1_baton[i] == 0)
      ;

    termprint(toprint[i * 3 + 1], 0);
    delay_ms((getRANDOM() >> 8) * 100);
    test2_baton[i] = 1;
  }
  while (test1_baton[i] == 0)
    ;
  termprint("Good job from test2\n", 0);
  test2_baton[STEPS] = 1;
  SYSCALL(SYS3, 0, 0, 0);
}

void test3()
{
  int i = 0;
  termprint("Entering test3!\n", 0);
  test3_baton[0] = 1;
  for (i = 0; i < STEPS; i++)
  {
    while (test2_baton[i] == 0)
      ;

    termprint(toprint[i * 3 + 2], 0);
    delay_ms((getRANDOM() >> 8) * 100);
    test3_baton[i + 1] = 1;
  }
  while (test2_baton[STEPS] == 0)
    ;
  termprint("Good job from test3\n", 0);
  termprint(gantt_diagram, 0);
  termprint("\n", 0);
  SYSCALL(SYS3, 0, 0, 0);
}

/* Entry point for the last process
 * This process terminates the test, and should have the lowest possible priority.
 * Basically, it executes when all the other processes are terminated, halting
 * the system
 */
void end() {
  HALT();
}

/* Entry point */
void main() {
  /* Init pcbs */
  initPcbs();

  /* DEBUG */
  term_sel(0);

  /* Init New Areas */
  init_newarea(SYSBP_NAREA, (memaddr)sysbp);
  init_newarea(INTERRUPT_NAREA, (memaddr)interrupt);
  
  /* Init scheduler */
  scheduler_init();
  init_ready((memaddr)test1, (memaddr)test2, (memaddr)test3, (memaddr)end);

  while (1);
}
