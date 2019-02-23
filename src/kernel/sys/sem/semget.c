#include <nanvix/const.h>
#include <sys/sem.h>

/**
 * @brief Creates a semaphore with the key if there's none assigned to this key already
 *
 * @details Search in the semaphore table if there's a semaphore with this key
 *
 *          If yes, then we do nothing and return the key
 *	    	If not, we create a semaphore with this key
 *
 * @param key    The key of the semaphore
 *
 * @return The id of the semaphore in the table if everything went well, -1 if there was a problem
 */

PUBLIC int sys_semget(unsigned key)
{
	for(int id = 0; id<NB_SEM; id++){ // If the semaphore already exists
		if(current_semaphores[id].key==key){
			current_semaphores[id].nbproc++;
			return id;
		}
	}

	// Else

	/* add a new semaphore in the table */

	current_semaphores[NB_SEM].s=create(EMPTY_SEM);
	current_semaphores[NB_SEM].key=key;
	current_semaphores[NB_SEM].nbproc=1;

	NB_SEM++;

	return NB_SEM-1;
}
