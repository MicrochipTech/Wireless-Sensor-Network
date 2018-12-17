/*
 * atca_utils.c
 *
 */ 


#include <stdint.h>
#include "atca_utils.h"


uint8_t bad_pointer(void* ptr)
{
	return (ptr == NULL);
}

