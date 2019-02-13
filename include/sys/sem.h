
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

#ifndef SEM_H_
#define SEM_H_
#include <nanvix/const.h>
/**
 * @brief Comand values for semaphores.
 */
/**@{*/
#define GETVAL   0 /**< Returns the value of a semaphore. */
#define SETVAL   1 /**< Sets the value of a semaphore.    */
#define IPC_RMID 3 /**< Destroys a semaphore.            */
/**@}*/

#ifndef _ASM_FILE_

	typedef struct { //Suppression des PUBLIC et PRIVATE
		int val;
		struct process **waiting_queue;
	} Semaphore;

	typedef struct { //Suppression des PUBLIC et PRIVATE
    Semaphore s;
		unsigned key;
		int nbproc;
	} unit;

	PUBLIC Semaphore create(int n);
	PUBLIC int print_value(Semaphore sem);
	PUBLIC Semaphore assign_value(Semaphore sem, int n);
	PUBLIC Semaphore down(Semaphore sem);
	PUBLIC Semaphore up(Semaphore sem);
	PUBLIC Semaphore destroy(Semaphore sem);

#define SIZE_MAX_TAB 50

	PUBLIC unit tab[SIZE_MAX_TAB];

/* Forward definitions. */
	extern int semget(unsigned);
	extern int semctl(int, int, int);
	extern int semop(int, int);

#endif
#endif /* SEM_H_ */
