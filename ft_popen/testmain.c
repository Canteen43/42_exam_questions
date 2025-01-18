/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testmain.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kweihman <kweihman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 12:30:48 by kweihman          #+#    #+#             */
/*   Updated: 2025/01/18 12:45:52 by kweihman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// For pipe(), fork(), dup2(), execvp(), STDIN_FILENO, and STDOUT_FILENO
#include <unistd.h>
// For pid_t
#include <sys/types.h>
// For NULL
#include <stdlib.h>
// For printf()
#include <stdio.h>
// For strlen()
#include <string.h>

// Prototype:
int	ft_popen(char *file, char **argv, char type);

int	main(void)
{
	int		fd;
	char	*str;
	char	str2[100];

	str = "Hi, I am Karl.\nThe cool guy.\n";
	fd = ft_popen("cat", (char *[]){"cat", NULL}, 'w');
	printf("fd: %d\n", fd);
	write(fd, str, strlen(str));
	close(fd);
	sleep(1);
	printf("\n\n");
	fd = ft_popen("ls", (char *[]){"ls", NULL}, 'r');
	printf("fd: %d\n", fd);
	read(fd, str2, 100);
	printf("%s", str2);
	close(fd);
}
