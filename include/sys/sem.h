
/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/const.h>
#include <nanvix/config.h>

#ifndef SEM_H_
#define SEM_H_

/**
 * @brief Comand values for semaphores.
 */
/**@{*/
#define GETVAL   0 /**< Returns the value of a semaphore. */
#define SETVAL   1 /**< Sets the value of a semaphore.    */
#define IPC_RMID 3 /**< Destroys a semaphore.            */
/**@}*/

#define SIZE_SEM_TAB PROC_MAX // Size of the semaphore table
#define DEAD_SEM -999 // Semaphore's value to a dead one

#ifndef _ASM_FILE_

	/**
	 * @brief Semaphore's struct 
	 * 
	 * @details Contains the semaphor's value et its waiting queue
	 *
	**/
	PUBLIC typedef struct {
		int val;
		struct process **waiting_queue;
	} Semaphore;

	/**
	 * @brief Stockage struct for the semaphore 
	 * 
	 * @details Contains a semaphor, its key and the numbre of current process using it
	 *
	**/
	PUBLIC typedef struct { 
		Semaphore s;
		unsigned key;
		int nbproc;
	} unit;

	/**
	 * @brief Table of the active semaphore
	 *
	**/
	PUBLIC unit tab[SIZE_SEM_TAB];

	PUBLIC int nb_sem; // Number of active semaphore

	// Functions to edit a semaphore

	PUBLIC Semaphore create(int n);
	PUBLIC Semaphore down(Semaphore sem);
	PUBLIC Semaphore up(Semaphore sem);
	PUBLIC Semaphore destroy(Semaphore sem);

	// Functions to use semaphore
	/* Forward definitions. */
	EXTERN int semget(unsigned);
	EXTERN int semctl(int, int, int);
	EXTERN int semop(int, int);

#endif
#endif /* SEM_H_ */
