#define _XOPEN_SOURCE 700
#include <stdalign.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "config.h"
#include "mlpt.h"

size_t ptbr = 0;

size_t translate(size_t va) {
    size_t offset = va & ((1 << POBITS) - 1); // get offset
    size_t vpn; // get v page #
    size_t segment_length = POBITS - 3;

    if (!ptbr) { // check if ptbr is valid
        return (size_t) - 1;
    }

    size_t* ptr = (size_t*) ptbr; // make pointer to ptbr
    for (int i = 0; i < LEVELS; ++i) { // iterating through the levels
        vpn = (va >> (POBITS + (LEVELS - 1 - i) * segment_length)) & ((1 << segment_length) - 1); // calculating vpn 

        if ((ptr[vpn] & 1) == 0) { // check if valid bit set, if not, then no physical address and return all 1 bits
            return (size_t) - 1;
        }

        ptr = (size_t *) ((ptr[vpn] >> POBITS) << POBITS); // setting the last \POBITS bits to 0 so that i can add offset
    }
    
    return (size_t) ptr | offset; // Return physical address
}

void page_allocate(size_t va) {
    size_t vpn; // get v page #
    size_t segment_length = POBITS - 3;
    double page_size = pow(2, POBITS); // changed variable name from feedback from lab

    if (!ptbr) { // check if ptbr is valid
        posix_memalign((void **) &ptbr, page_size, page_size); // allocating ptbr
        memset((void *) ptbr, 0, page_size); // initializing as 0
    }

    size_t* ptr = (size_t*) ptbr; // make pointer to ptbr
    for (int i = 0; i < LEVELS; ++i) { // iterating through the levels
        vpn = (va >> (POBITS + (LEVELS - 1 - i) * segment_length)) & ((1 << segment_length) - 1); // calculating vpn

        if ((ptr[vpn] & 1) == 0) { //check if valid bit set
            posix_memalign((void **) &ptr[vpn], page_size, page_size); // allocating ptr[vpn]
            memset((void *) ptr[vpn], 0, page_size); // initializing each ptr[vpn] as 0
            ptr[vpn] = ptr[vpn] | 1; // setting valid bit
        }

        ptr = (size_t *) ((ptr[vpn] >> POBITS) << POBITS); //setting the last POBITS bits to 0 and set ptr to next
    }

    return;
}