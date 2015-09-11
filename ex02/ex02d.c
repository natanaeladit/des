#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include <native/task.h>
#include <native/timer.h>

#include  <rtdk.h>
RT_TASK demo_task;

void demo(void *arg)
{
  sleep(1);
  RT_TASK *curtask;
  RT_TASK_INFO curtaskinfo;
  // Read system clock
  RTIME period = 1e9;

  // inquire current task
  curtask=rt_task_self();
  rt_task_inquire(curtask,&curtaskinfo);

  // determine the period
  int num = * (int *)arg;
  period*=num;
  rt_task_set_periodic(NULL,TM_NOW,period);
  
  while(1){
	  // print task name
	  rt_printf("Task name : %s - Period %d \n", curtaskinfo.name,num);
	  rt_task_wait_period(NULL);
  }
}

int main(int argc, char* argv[])
{
  char  str[10] ;
  int i;
  int periods[3];

  // Perform auto-init of rt_print buffers if the task doesn't do so
  rt_print_auto_init(1);

  // Lock memory : avoid memory swapping for this program
  mlockall(MCL_CURRENT|MCL_FUTURE);

  rt_printf("start task\n");

  for (i=0; i<3; i++)
  {
	  /*
	   * Arguments: &task,
	   *            name,
	   *            stack size (0=default),
	   *            priority,
	   *            mode (FPU, start suspended, ...)
	   */
	  sprintf(str,"hello-%d",i);
	  periods[i]=i+1;
	  rt_task_create(&demo_task, str, 0, 50, 0);
	  
	  /*
	   * Arguments: &task,
	   *            task function,
	   *            function argument
	   */
	  rt_task_start(&demo_task, &demo, &periods[i]);
  }
  rt_printf("end program by CTRL-C\n");
  pause();
}