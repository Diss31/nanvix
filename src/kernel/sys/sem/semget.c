#include <nanvix/const.h>
#include <sys/sem.h>

/**
 * @brief Creates a semaphore with the key if there's none assigned to this key already
 *
 * @details Search in the semaphore table if there's a semaphore with this key
 *
 *          If yes, then we do nothing and return the key
 *	    If not, we create a semaphore with this key
 *
 * @param key    The key of the semaphore
 *
 * @return The id of the semaphore in the table if everything went well, -1 if there was a problem
 */

PUBLIC int sys_semget(unsigned key)
{
	int exists = 0;
	int id;
	for(id=0; id<SIZE_SEM_TAB; id++){
		if(tab[id].key==key){
			tab[id].nbproc++;
			exists=1;
			break;
		}
	}

	if(!exists){
		Semaphore semaphore = create(1);
		/* ajouter le nouveau semaphore dans la table */
		for(id=0;id<SIZE_SEM_TAB; id++){
			if(tab[id].key==0){
				tab[id].s=semaphore;
				tab[id].key=key;
				tab[id].nbproc=0;
				break;
			}
		}
	}

	return id;
}
