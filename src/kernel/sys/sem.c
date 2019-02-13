
#include <nanvix/const.h>
#include <nanvix/pm.h>
#include <nanvix/hal.h>
#include <sys/sem.h>

PUBLIC Semaphore create(int n){
	Semaphore sem;
	sem.val = n; //assert n>0
	sem.waiting_queue = NULL;
	
	return sem;
}

PUBLIC int print_value(Semaphore sem){
	return sem.val;
}

PUBLIC Semaphore assign_value(Semaphore sem, int n){
	sem.val=n;
	return sem;
}

PUBLIC Semaphore down(Semaphore sem){
	disable_interrupts();
	sem.val--;
	if(sem.val<0){		
		sleep(sem.waiting_queue,curr_proc->priority);
	}
	enable_interrupts();
	return sem;
}

PUBLIC Semaphore up(Semaphore sem){
	disable_interrupts();
	sem.val++;
	if(sem.val>=0){
		wakeup(sem.waiting_queue);
	}
	enable_interrupts();
	return sem;
}

PUBLIC Semaphore destroy(Semaphore sem){
	sem.val=-999;
	sem.waiting_queue=NULL;
	return sem;
}