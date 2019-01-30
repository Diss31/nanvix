/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
 *              2016-2016 Subhra S. Sarkar <rurtle.coder@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/config.h>
#include <nanvix/const.h>
#include <nanvix/dev.h>
#include <nanvix/fs.h>
#include <nanvix/hal.h>
#include <nanvix/mm.h>
#include <nanvix/pm.h>
#include <nanvix/klib.h>
#include <sys/stat.h>
#include <signal.h>
#include <limits.h>

/**
 * @brief Idle process page directory.
 */
EXTERN struct pde idle_pgdir[];

/**
 * @brief Idle process kernel stack.
 */
PUBLIC char idle_kstack[KSTACK_SIZE];

/**
 * @brief Process table.
 */
PUBLIC struct process proctab[PROC_MAX];

/**
 * @brief Current running process. 
 */
PUBLIC struct process *curr_proc = IDLE;

/**
 * @brief Last running process. 
 */
PUBLIC struct process *last_proc = IDLE;

/**
 * @brief Next available process ID.
 */
PUBLIC pid_t next_pid = 0;

/**
 * @brief Current number of processes in the system.
 */
PUBLIC unsigned nprocs = 0;

/**
 * @brief Initializes the process management system.
 */
PUBLIC void pm_init(void)
{	
	int i;             /* Loop index.      */
	struct process *p; /* Working process. */
	
	/* Initialize the process table. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
		p->flags = 0, p->state = PROC_DEAD;

	/* Handcraft init process. */
	IDLE->cr3 = (dword_t)idle_pgdir;
	IDLE->intlvl = 1;
	IDLE->flags = 0;
	IDLE->received = 0;
	IDLE->kstack = idle_kstack;
	IDLE->restorer = NULL;
	for (i = 0; i < NR_SIGNALS; i++)
		IDLE->handlers[i] = SIG_DFL;
	IDLE->irqlvl = INT_LVL_5;
	IDLE->pgdir = idle_pgdir;
	for (i = 0; i < NR_PREGIONS; i++)
		IDLE->pregs[i].reg = NULL;
	IDLE->size = 0;
	for (i = 0; i < OPEN_MAX; i++)
		IDLE->ofiles[i] = NULL;
	IDLE->close = 0;
	IDLE->umask = S_IXUSR | S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH;
	IDLE->tty = NULL_DEV;
	IDLE->status = 0;
	IDLE->nchildren = 0;
	IDLE->uid = SUPERUSER;
	IDLE->euid = SUPERUSER;
	IDLE->suid = SUPERUSER;
	IDLE->gid = SUPERGROUP;
	IDLE->egid = SUPERGROUP;
	IDLE->sgid = SUPERGROUP;
	IDLE->pid = next_pid++;
	IDLE->pgrp = IDLE;
	IDLE->father = NULL;
	kstrncpy(IDLE->name, "idle", NAME_MAX);
	IDLE->utime = 0;
	IDLE->ktime = 0;
	IDLE->cutime = 0;
	IDLE->cktime = 0;
	IDLE->state = PROC_RUNNING;
	IDLE->counter = PROC_QUANTUM;
	IDLE->priority = PRIO_USER;
	IDLE->nice = NZERO;
	IDLE->alarm = 0;
	IDLE->next = NULL;
	IDLE->chain = NULL;

	nprocs++;

	enable_interrupts();
}

/**
 * @brief Work out the number of tickets allowed to a process according to its priority
 */
PRIVATE int num_tickets(int priority){ 
	int tickets;
	switch(priority){
		case PRIO_IO : 			tickets = 8; break;
		case PRIO_BUFFER : 		tickets = 7; break;
		case PRIO_INODE : 		tickets = 6; break;
		case PRIO_SUPERBLOCK : 	tickets = 5; break;
		case PRIO_REGION :		tickets = 4; break;
		case PRIO_TTY : 		tickets = 3; break;
		case PRIO_SIG : 		tickets = 2; break;
		case PRIO_USER : 		tickets = 1; break;
	}

	return tickets;
}

/**
 * @brief Allow a determinist number of tickets for a process, according to its priority.
 *
 * @note There is no verification if the process have already tickets. We allow no ticket for IDLE;
 */
PUBLIC void add_tickets(struct process* proc){

	if(proc->pid==1){ //If the proc is IDLE
		return; // Make nothing
	}

	int nb =num_tickets(proc->priority); // Number of tickets allowed
	for(int i = nb_total_tickets; i < nb_total_tickets + nb;i++){
		array_tickets[i]=proc->pid; // We add the process pid, nb times in the array_tickets. So all tickets for a process are successive
	}
	nb_total_tickets += nb; //We update the total number of tickets
}

/**
 * @brief Desallow the tickets of a process.
 *
 * @note If the process has no ticket, make nothing.
 */
PUBLIC void rm_ticket(struct process* proc){
	int nb = num_tickets(proc->priority);
	nb_total_tickets -= nb; // We work out the number of tickets to remove
	
	int i=0;
	while(i < nb_total_tickets && array_tickets[i]!=proc->pid){ //We search where is the group of tickets of proc
		i++;
	}
	
	if(i == nb_total_tickets){ // If we don't fine it, we make nothing 
		return;
	}
	for(i;i<nb_total_tickets;i++){ // Else, we copy the tickets on the left to crush the tickets to delete
		array_tickets[i]=array_tickets[i+nb];
	}
	
	for(i;i<i+nb;i++){ // And we delete the last tickets, in case of the tickets to delete are the last ones
		array_tickets[i]=NULL;
	}
}