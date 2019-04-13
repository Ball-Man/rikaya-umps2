#include <log.e>

/* GANTT diagram for testing purposes */
char gantt_diagram[GANTT_SIZE] = {0};

/* Log function for tests */
extern void log_process_order(int process) {
  static int index = 0;

  if (index < GANTT_SIZE)
    gantt_diagram[index++] = 48+process;
  else
    gantt_diagram[GANTT_SIZE-1] = '\0';
}
