/*
 * Copyright(C) 2011-2018 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2018-2018 Davidson Francis <davidsondfgl@gmail.com>
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

#include <nanvix/syscall.h>
#include <unistd.h>
#include <errno.h>

/*
 * Gets sys ticks since initialization, may be useful
 * as seed for random numbers, but mktime would be better.
 */
int gticks()
{
	register ssize_t ret
		__asm__("r11") = NR_gticks;
	
	__asm__ volatile (
		"l.sys 1"
		: "=r" (ret)
		: "r"  (ret)
	);

	/* Error. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return ((ssize_t)ret);
}
