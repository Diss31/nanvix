/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
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

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <nanvix/klib.h>

/**
 * @brief Schedules a process to execution.
 * 
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 * 
 * @param proc Process to be resumed.
 * 
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{	
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
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

	if(proc->pid==0){ //If the proc is IDLE
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
PUBLIC void rm_tickets(struct process* proc){
	int nb = num_tickets(proc->priority);
	nb_total_tickets -= nb; // We work out the number of tickets to remove
	
	int i=0;
	while(i < nb_total_tickets && array_tickets[i]!=proc->pid){ //We search where is the group of tickets of proc
		i++;
	}
	
	if(i == nb_total_tickets){ // If we don't fine it, we make nothing 
		return;
}
	for(;i<nb_total_tickets;i++){ // Else, we copy the tickets on the left to crush the tickets to delete
		array_tickets[i]=array_tickets[i+nb];
	}
	
	for(;i<i+nb;i++){ // And we delete the last tickets, in case of the tickets to delete are the last ones
		array_tickets[i]=-1;
	}
}

/**
 * @brief Draw a ticket inside the existing tickets.
 */
PRIVATE int draw_ticket(void){ 
	return krand()%(nb_total_tickets);
}

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;    /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;
		
		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);

	}

	/* Choose a process to run next. */
	next = IDLE;
	int ticket = draw_ticket();
	for (p = FIRST_PROC; p <= LAST_PROC; p++){
	
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		if(p->pid == array_tickets[ticket]){
			next->counter++;
			next=p;
		}

		else
			p->counter++;
		
	}
	
	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	switch_to(next);
}
