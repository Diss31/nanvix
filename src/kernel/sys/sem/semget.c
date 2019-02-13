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
 * @return key if everything went well, -1 if there was a problem
 */ 

PUBLIC int sys_semget(unsigned key)
{
	int exists = 0;
	for(int i=0; i<SIZE_MAX_TAB; i++){
		if(tab[i][1]==key){
			exists=1;
			break;
		}
	}

	if(!exists){
		Semaphore s = create(0);
		/* ajouter le nouveau semaphore dans la table */
		for(int i=0; i<SIZE_MAX_TAB; i++){
			if(tab[i][0]==NULL){
				tab[i][0]=s;
				tab[i][1]=key;
				tab[i][2]=0;
				break;
			}
		}
	}

	return key;
}
