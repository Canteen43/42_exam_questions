/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kweihman <kweihman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 10:46:14 by kweihman          #+#    #+#             */
/*   Updated: 2025/01/15 11:07:21 by kweihman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// For fork(), alarm():
#include <unistd.h>
// For sigaction():
#include <signal.h>
// For printf():
#include <stdio.h>
// For strsignal():
#include <string.h>

	pid_t				pid;
	bool				timed_out = {0};



void	f_signal_handler(int signum)
{
	if (signum != SIGALRM)
		return ;
	kill(pid, SIGKILL);
	timed_out = true;
}

int	sandbox((void) (*f)(void), unsigned long timeout, bool verbose)
{
	struct sigaction	act = {0};
	int					function_type = -1;
	int 				wstatus;

	act.sa_handler = f_signal_handler;
	sigaction(SIGALRM, act, NULL);
	pid = fork();
	if (pid == 0)
	{
		f();
		exit(0);
	}
	alarm(timeout);
	waitpid(pid, &wstatus, 0);
	if (timed_out)
		function_type = 3;
	else if (WIFSIGNALED(wstatus))
		function_type = 2;
	else if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
		function_type = 1;
	else if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0)
		function_type = 0;

	if (verbose)
	{
		if (function_type == 3)
			printf("baddy: Timeout after %d secs", timeout);
		if (function_type == 2)
			printf("baddy: terminated by %s", strsignal(WTERMSIG(wstatus)));
		if (function_type == 1)
			printf("baddy: Exit code %d", WEXITSTATUS(wstatus));
		if (function_type == 3)
			printf("goody!", timeout);
	}

	if (function_type == 1 || function_type == 2 || function_type == 3)
		return(0);
	else if(function_type == 0)
		return(1);
	else
		return(-1);
}
