/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kweihman <kweihman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 12:18:49 by kweihman          #+#    #+#             */
/*   Updated: 2025/01/18 12:30:07 by kweihman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// For pipe(), fork(), dup2(), execvp(), STDIN_FILENO, and STDOUT_FILENO
#include <unistd.h>
// For pid_t
#include <sys/types.h>
// For NULL
#include <stdlib.h>

int	ft_popen(char *file, char **argv, char type)
{
	int		fd[2];
	pid_t	pid;

	if (type != 'r' && type != 'w')
		return (-1);
	pipe(fd);
	pid = fork();
	if (pid == 0)
	{
		if (type == 'w')
		{
			dup2(fd[0], STDIN_FILENO);
		}
		if (type == 'r')
		{
			dup2(fd[1], STDOUT_FILENO);
		}
		close(fd[1]);
		close(fd[0]);
		execvp(file, argv);
	}
	if (type == 'w')
	{
		close(fd[0]);
		return (fd[1]);
	}
	if (type == 'r')
	{
		close(fd[1]);
		return (fd[0]);
	}
	return (-1);
}
