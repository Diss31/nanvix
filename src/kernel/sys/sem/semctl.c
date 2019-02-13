#include <nanvix/const.h>
#include <sys/sem.h>
/**
 * @brief Make some operations on the semaphore indicated by semid
 *
 * @details Make some operations according to the value of cmd
 * 
 *      If cmd is equal to zero, then we try to get the value of the semaphore
 *	    If cmd is equal to one, then we try to set the value of the semaphore to val
 *	    If cmd is equal to three, then we destroy the semaphore if it's not used
 * 
 * @param semid    The id of the semaphore in the semaphore table
 * @param cmd      The identifier of the command to run
 * @param val      The new value to give to the semaphore if we do a SETVAL
 *
 * @return 0 if everything went well, -1 if there was a problem, the value of the semaphore if we did a GETVAL
 */ 


PUBLIC int sys_semctl(int semid, int cmd, int val){
	Semaphore s = tab[semid].s;
	if(s.waiting_queue==NULL)
		return -1;
	int value = -1;

	switch(cmd){
		case 0: //GETVAL
			value = s.val;
			break;
		case 1: //SETVAL
			s.val = val;
			value=0;
			break;
		case 3: //IPC RMID
			if(tab[semid].nbproc==0){
				destroy(s);
			}
			value=0;
	}
	return value;
}
