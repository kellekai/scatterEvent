#ifndef _SCATTEREVENT_H
#define _SCATTEREVENT_H

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int scatterEvent( int check, int initSync, int syncIter, MPI_Comm gcomm, int iter );

#endif
