/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testmain.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kweihman <kweihman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 13:45:39 by kweihman          #+#    #+#             */
/*   Updated: 2025/01/15 13:49:21 by kweihman         ###   ########.fr       */
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

// Prototype
int	sandbox((void) (*f)(void), unsigned long timeout, bool verbose);

void	test_function(void)
{
	sleep(5);
}

int	main(void)
{
	int	ret;

	ret = sandbox(test_function, 3, true);
	printf("Return: %d\n", ret);
}
