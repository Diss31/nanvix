
#include <nanvix/const.h>
#include <nanvix/pm.h>
#include <nanvix/hal.h>
#include <sys/sem.h>

/**
 * @brief Create a new semaphore
 * 
 * @details Take the initial value of the semaphor et return the initializing semaphore
 *
**/
PUBLIC Semaphore create(int n){
	Semaphore sem;
	sem.val = n; 
	sem.waiting_queue = NULL;
	
	return sem;
}

/**
 * @brief Decrease the value of the giving semaphore
 * 
 * @details If value become negative, we put the current process in sleep with a PRIO_SEM priority. All of that is in a critical section.
 *
**/
PUBLIC Semaphore down(Semaphore sem){
	disable_interrupts();
	sem.val--;
	if(sem.val<0){		
		sleep(sem.waiting_queue,PRIO_SEM);
	}
	enable_interrupts();
	return sem;
}

/**
 * @brief Increase the value of the giving semaphore
 * 
 * @details If value become positive, we wake up the first process in sleep. All of that is in a critical section.
 *
**/
PUBLIC Semaphore up(Semaphore sem){
	disable_interrupts();
	sem.val++;
	if(sem.val>=0){
		wakeup(sem.waiting_queue);
	}
	enable_interrupts();
	return sem;
}

/**
 * @brief Destroy the giving semaphore
 * 
 * @details We set the semaphore's value at DEAD_SEM and we flush its waiting_queue
 *
**/
PUBLIC Semaphore destroy(Semaphore sem){
	sem.val=DEAD_SEM;
	sem.waiting_queue=NULL;
	return sem;
}