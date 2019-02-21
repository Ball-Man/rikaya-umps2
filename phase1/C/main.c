/*****************************************************************************
 * Copyright 2004, 2005 Michael Goldweber, Davide Brini.                     *
 * Modified 2012 by Marco Di Felice                                          *
 * Modified 2018 by Renzo Davoli
 *                                                                           *
 * This file is part of RIKAYA.                                              *
 *                                                                           *
 * pkaya is free software; you can redistribute it and/or modify it under the*
 * terms of the GNU General Public License as published by the Free Software *
 * Foundation; either version 2 of the License, or (at your option) any      *
 * later version.                                                            *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

/*********************************P1TEST.C*******************************
 *
 *  Test program for the modules ASL and PCB (phase 1).
 *
 *  Produces progress messages on terminal 0 in addition 
 *    to the array ``okbuf[]''
 *    Error messages will also appear on terminal 0 in 
 *    addition to the array ``errbuf[]''.
 *
 *    Aborts as soon as an error is detected.
 *
 *    
 */

#include <const.h>
#include <list.h>
#include <umps/libumps.h>
#include <umps/types.h>
#include <pcb.e>
#include <semd.e>
#include <terminal.e>

#define MAXSEM  MAX_PROC

#define MAX_PCB_PRIORITY    10
#define MIN_PCB_PRIORITY    0
#define DEFAULT_PCB_PRIORITY    5

char okbuf[2048];     /* sequence of progress messages */
char errbuf[128];     /* contains reason for failing */
char msgbuf[128];     /* nonrecoverable error message before shut down */



int onesem;
pcb_t *procp[MAX_PROC], *p, *q, *maxproc, *minproc, *proc;
semd_t  *semd[MAXSEM];
int sem[MAXSEM + 1];

struct list_head qa;
char *mp = okbuf;

#define TRANSMITTED 5
#define TRANSTATUS    2
#define ACK 1
#define PRINTCHR  2
#define CHAROFFSET  8
#define STATUSMASK  0xFF
#define TERM0ADDR 0x10000250
#define DEVREGSIZE 16       
#define READY     1
#define DEVREGLEN   4
#define TRANCOMMAND   3
#define BUSY      3
#define DEV_PER_INT 8 /* Maximum number of devices per interrupt line */

typedef unsigned int devreg;

typedef unsigned int memaddr;

/******************************************************************************
 * I/O Routines to write on a terminal
 ******************************************************************************/

/* This function returns the terminal transmitter status value given its address */ 
devreg termstat(memaddr *stataddr) {
  return((*stataddr) & STATUSMASK);
}

/* This function prints a string on specified terminal and returns true if 
 * print was successful, false if not   */
unsigned int termprint(char * str, unsigned int term) {

  memaddr *statusp;
  memaddr *commandp;
  
  devreg stat;
  devreg cmd;
  
  unsigned int error = false;
  
  if (term < DEV_PER_INT) {
    /* terminal is correct */
    /* compute device register field addresses */
    statusp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANSTATUS * DEVREGLEN));
    commandp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANCOMMAND * DEVREGLEN));
    
    /* test device status */
    stat = termstat(statusp);
    if ((stat == READY) || (stat == TRANSMITTED)) {
      /* device is available */
      
      /* print cycle */
      while ((*str != '\0') && (!error)) {
        cmd = (*str << CHAROFFSET) | PRINTCHR;
        *commandp = cmd;

        /* busy waiting */
        while ((stat = termstat(statusp)) == BUSY);
        
        /* end of wait */
        if (stat != TRANSMITTED) {
          error = true;
        } else {
          /* move to next char */
          str++;
        }
      }
    } else {
      /* device is not available */
      error = true;
    }
  } else {
    /* wrong terminal device number */
    error = true;
  }

  return (!error);    
}


/* This function places the specified character string in okbuf and
 *  causes the string to be written out to terminal0 */
void addokbuf(char *strp) {

  termprint(strp, 0);
}


/* This function places the specified character string in errbuf and
 *  causes the string to be written out to terminal0.  After this is done
 *  the system shuts down with a panic message */
void adderrbuf(char *strp) {

  termprint(strp, 0);
    
  PANIC();
}


/******************************************************************************
 * Main Test File
 ******************************************************************************/


int main() {
  int i;
  initPcbs();
  addokbuf("Initialized Process Control Blocks   \n");

  /* Check allocPcb */
  for (i = 0; i < MAX_PROC; i++) {
    if ((procp[i] = allocPcb()) == NULL)
      adderrbuf("allocPcb(): unexpected NULL   ");
  }
  
  if (allocPcb() != NULL) {
    adderrbuf(" ERROR 178: allocPcb(): allocated more than MAX_PROC entries   ");
  }
  addokbuf(" allocPcb test OK   \n");

  
  /* Return the last 10 entries back to free list */
  for (i = 10; i < MAX_PROC; i++)
          freePcb(procp[i]);
  
  addokbuf(" Added 10 entries to the free PCB list   \n");

  /* Create a 10-element process queue */
  INIT_LIST_HEAD(&qa);
  
  if (!emptyProcQ(&qa)) adderrbuf("ERROR: emptyProcQ(qa): unexpected false   ");
  
  addokbuf("Testing insertProcQ ...   \n");
  
  for (i = 0; i < 10; i++) {
    if ((q = allocPcb()) == NULL)
      adderrbuf("ERROR 198: allocPcb(): unexpected NULL while insert   ");
    switch (i) {
      case 3:
        q->priority=DEFAULT_PCB_PRIORITY;
        proc = q;
      break;
      case 4:
        q->priority=MAX_PCB_PRIORITY;
        maxproc = q;
        break;  
      case 5:
        q->priority=MIN_PCB_PRIORITY;
        minproc=q;
      break;
      default:
        q->priority=DEFAULT_PCB_PRIORITY;
      break;
    }
    insertProcQ(&qa, q);
  }
  
  addokbuf("Test insertProcQ: OK. Inserted 10 elements \n");
  
  if (emptyProcQ(&qa)) adderrbuf("ERROR 221: emptyProcQ(qa): unexpected true"   );

  /* Check outProcQ and headProcQ */
  if (headProcQ(&qa) != maxproc)
    adderrbuf("ERROR 225: headProcQ(qa) failed   ");
  
  /* Removing an element from ProcQ */
  q = outProcQ(&qa, proc);
  if ((q == NULL) || (q != proc))
    adderrbuf("ERROR 230: outProcQ(&qa, proc) failed to remove the entry   ");    
  freePcb(q);
  
  /* Removing the first element from ProcQ */
  q = removeProcQ(&qa);
  if (q == NULL || q != maxproc)
    adderrbuf("ERROR 236: removeProcQ(&qa, midproc) failed to remove the elements in the right order   ");
  freePcb(q);

  /* Removing other 7 elements  */
  addokbuf(" Testing removeProcQ ...   \n");
  for (i = 0; i < 7; i++) {
    if ((q = removeProcQ(&qa)) == NULL)
      adderrbuf("removeProcQ(&qa): unexpected NULL   ");
    freePcb(q);
  }
  
  /* Removing the last element */
  q=removeProcQ(&qa);
  if (q != minproc)
    adderrbuf("ERROR 250: removeProcQ(): failed on last entry   ");
  freePcb(q);
  
  if (removeProcQ(&qa) != NULL)
    adderrbuf("ERROR 254: removeProcQ(&qa): removes too many entries   ");

  if (!emptyProcQ(&qa))
    adderrbuf("ERROR 257: emptyProcQ(qa): unexpected false   ");

  addokbuf(" Test insertProcQ(), removeProcQ() and emptyProcQ(): OK   \n");
  addokbuf(" Test process queues module: OK      \n");

  addokbuf(" Testing process trees...\n");

  if (!emptyChild(procp[2]))
    adderrbuf("ERROR 265: emptyChild: unexpected false   ");
  
  /* make procp[1],procp[2],procp[3], procp[7] children of procp[0] */
  addokbuf("Inserting...   \n");
  insertChild(procp[0], procp[1]);
  insertChild(procp[0], procp[2]);
  insertChild(procp[0], procp[3]);
  insertChild(procp[0], procp[7]);
  addokbuf("Inserted 2 children of pcb0  \n");
  
  /* make procp[8],procp[9] children of procp[7] */
  insertChild(procp[7], procp[8]);
  insertChild(procp[7], procp[9]);
  addokbuf("Inserted 2 children of pcb7  \n");

  if (emptyChild(procp[0]))
    adderrbuf("ERROR 281: emptyChild(procp[0]): unexpected true   ");
  
  if (emptyChild(procp[7]))
    adderrbuf("ERROR 281: emptyChild(procp[0]): unexpected true   ");
  
  /* Check outChild */
  q = outChild(procp[1]);
  if (q == NULL || q != procp[1])
    adderrbuf("ERROR 289: outChild(procp[1]) failed ");
  
  q = outChild(procp[8]);
  if (q == NULL || q != procp[8])
    adderrbuf("ERROR 293: outChild(procp[8]) failed ");
  
  /* Check removeChild */
  q = removeChild(procp[0]);
  if (q == NULL || q != procp[2])
    adderrbuf("ERROR 298: removeChild(procp[0]) failed ");
  
  q = removeChild(procp[7]);
  if (q == NULL || q != procp[9])
    adderrbuf("ERROR 302: removeChild(procp[7]) failed ");
  
  q = removeChild(procp[0]);
  if (q == NULL || q != procp[3])
    adderrbuf("ERROR 306: removeChild(procp[0]) failed ");

  q = removeChild(procp[0]);
  if (q == NULL || q != procp[7])
    adderrbuf("ERROR 310: removeChild(procp[0]) failed ");
  
  
  if (removeChild(procp[0]) != NULL)
    adderrbuf("ERROR 314: removeChild(): removes too many children   ");

  if (!emptyChild(procp[0]))
      adderrbuf("ERROR 317: emptyChild(procp[0]): unexpected false   ");
      
  addokbuf("Test: insertChild(), removeChild() and emptyChild() OK   \n");
  addokbuf("Testing process tree module OK      \n");

   
  freePcb(procp[0]);
  freePcb(procp[1]);
  freePcb(procp[2]);
  freePcb(procp[3]);
  freePcb(procp[4]);
  freePcb(procp[5]);
  freePcb(procp[6]);
  freePcb(procp[7]);
  freePcb(procp[8]);
  freePcb(procp[9]);
  
  
  /* check ASL */
  initASL();
  addokbuf("Initializing active semaphore list   \n");

  /* check removeBlocked and insertBlocked */
  addokbuf(" Test insertBlocked(): test #1 started  \n");
  for (i = 10; i < MAX_PROC; i++) {
    procp[i] = allocPcb();
    if (insertBlocked(&sem[i], procp[i]))
      adderrbuf("ERROR 344: insertBlocked() test#1: unexpected true   ");
  }

  addokbuf("Test insertBlocked(): test #2 started  \n");
  for (i = 0; i < 10; i++) {
    procp[i] = allocPcb();
    if (insertBlocked(&sem[i], procp[i]))
      adderrbuf("ERROR 351:insertBlocked() test #2: unexpected true   ");
  }

  /* check if semaphore descriptors are returned to the free list */
  p = removeBlocked(&sem[11]);
  if (insertBlocked(&sem[11],p))
    adderrbuf("ERROR 357: removeBlocked(): fails to return to free list   ");

  if (insertBlocked(&sem[MAXSEM], procp[9]) == false)
    adderrbuf("ERROR 360: insertBlocked(): inserted more than MAX_PROC   ");
  
  addokbuf("Test removeBlocked(): test started   \n");
  for (i = 10; i< MAX_PROC; i++) {
    q = removeBlocked(&sem[i]);
    if (q == NULL)
      adderrbuf("ERROR 366: removeBlocked(): wouldn't remove   ");
    if (q != procp[i])
      adderrbuf("ERROR 368: removeBlocked(): removed wrong element   ");
  }

  if (removeBlocked(&sem[11]) != NULL)
    adderrbuf("ERROR 372: removeBlocked(): removed nonexistent blocked proc   ");
  
  addokbuf("Test insertBlocked() and removeBlocked() ok   \n");

  if (headBlocked(&sem[11]) != NULL)
    adderrbuf("ERROR 378: headBlocked(): nonNULL for a nonexistent queue   ");
  
  if ((q = headBlocked(&sem[9])) == NULL)
    adderrbuf("ERROR 380: headBlocked(1): NULL for an existent queue   ");
  if (q != procp[9])
    adderrbuf("ERROR 382: headBlocked(1): wrong process returned   ");
  p = outBlocked(q);
  if (p != q)
    adderrbuf("ERROR 385: outBlocked(1): couldn't remove from valid queue   ");
  
  /* Creating a 2-layer tree */
  insertChild(procp[0], procp[1]);
  insertChild(procp[0], procp[2]);
  insertChild(procp[0], procp[3]);
  insertChild(procp[3], procp[4]);
  
  /* Testing outChildBlocked */
  outChildBlocked(procp[0]);
  
  if (headBlocked(&sem[0]) != NULL)
    adderrbuf("ERROR 397: outChildBlocked(): nonNULL for a nonexistent queue (0)  ");
  if (headBlocked(&sem[1]) != NULL)
    adderrbuf("ERROR 399: outChildBlocked(): nonNULL for a nonexistent queue (1)  ");
  if (headBlocked(&sem[2]) != NULL)
    adderrbuf("ERROR 401: outChildBlocked(): nonNULL for a nonexistent queue  (2) ");
  if (headBlocked(&sem[3]) != NULL)
    adderrbuf("ERROR 403: outChildBlocked(): nonNULL for a nonexistent queue (3)  ");
  if (headBlocked(&sem[4]) != NULL)
      adderrbuf("ERROR 405: outChildBlocked(): nonNULL for a nonexistent queue (4)  "); 
  if (headBlocked(&sem[5]) == NULL)
      adderrbuf("ERROR 407: outChildBlocked(): NULL for an existent queue  (5) ");
  
  addokbuf("Test headBlocked() and outBlocked(): OK   \n");
  
  addokbuf("ASL module OK   \n");
  addokbuf("So Long and Thanks for All the Fish\n");
  
  return 0;
}
