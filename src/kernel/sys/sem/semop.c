#include <nanvix/const.h>
#include <sys/sem.h>
/**
 * @brief Runs a certain number of operations on a semaphore.
 *
 * @details Runs up() or down() an op number of times.
 * 
 *      If op is greater than zero, then we run the up() operation.
 *	    If op is lower than zero, then we run the down() operation.
 *	    If op is equal to zero, then we do nothing
 * 
 * @param semid    The id of the semaphore in the semaphore table
 * @param op       The amount of times we have to run the operation
 *
 * @return 0 if everything went well, -1 if there was a problem
 */ 

PUBLIC int sys_semop(int semid, int op){
	Semaphore s = tab[semid].s;

	/* If there was an issue with getting the semaphore, we can't run operations on it */
	if(s.waiting_queue==NULL)
		return -1;

	int oldvalue, newvalue;

	if(op<0){
		oldvalue = print_value(s);
		for (int i=0; i>op; i--)
			down(s);
		newvalue = print_value(s);

		/* We call GETVAL before and after running our operations to check if it worked. If it didn't we return -1 */
		if(newvalue != oldvalue-op)
			return -1;
	}

	else if (op>0) {
		oldvalue = print_value(s);
		for (int i = 0; i<op; i++)
			down(s);
		newvalue = print_value(s);

		/* We call GETVAL before and after running our operations to check if it worked. If it didn't we return -1 */
		if(newvalue != oldvalue+op)
			return -1;
	}

	return 0;
}
