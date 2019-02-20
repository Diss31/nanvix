#include <nanvix/const.h>
#include <sys/sem.h>
/**
 * @brief Make some operations on the semaphore indicated by semid
 *
 * @details Make some operations according to the value of cmd
 * 
 *      If cmd is equal to GETVAL, then we try to get the value of the semaphore
 *	    If cmd is equal to SETVAL, then we try to set the value of the semaphore to val
 *	    If cmd is equal to IPC_RMID, then we destroy the semaphore if it's not used
 * 
 * @param semid    The id of the semaphore in the semaphore table
 * @param cmd      The identifier of the command to run
 * @param val      The new value to give to the semaphore if we do a SETVAL
 *
 * @return 0 if everything went well, the value of the semaphore if we did a GETVAL
 */ 


PUBLIC int sys_semctl(int semid, int cmd, int val){
	Semaphore s = tab[semid].s;
	
	int value = -1;

	switch(cmd){
		case GETVAL:
			value = s.val;
			break;
		case SETVAL:
			s.val = val;
			value = 0;
			break;
		case IPC_RMID:
			if(tab[semid].nbproc==1){ //if it's the only process to use this semaphore
				s = destroy(s);
				nb_sem--;
			}
			else{
				tab[semid].nbproc--; // else
			}
			value=0;
	}
	return value;
}
