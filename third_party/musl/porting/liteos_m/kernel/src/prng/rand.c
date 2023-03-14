#include <stdlib.h>
#include <stdint.h>

void srand(unsigned s)
{
	return srandom(s);
}

int rand(void)
{
	return random();
}