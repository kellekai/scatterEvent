#include "scatterEvent.h"

int scatterEvent( int check, int initSync, int syncIter, MPI_Comm gcomm ) {

    // constants
    const unsigned int EVENT = 1111;
    const unsigned int SYNCH = 2222;

    // local variables
    int rank, size, mflag, who, ri, buffer, sflag;
    MPI_Status mstatus, teststatus;

    MPI_Comm_rank(gcomm, &rank);
    MPI_Comm_size(gcomm, &size);

    // static variables
    static bool isInit = true;
    static int syncflag;
    static int sendflag;
    static int si; // sync iteration
    static long syncCounter = 0;
    static MPI_Request *mrequest, srequest=MPI_REQUEST_NULL;


    // initialize
    if (isInit) {
        syncflag = initSync;
        sendflag = 0;
        si = 0;
        isInit = false;
        mrequest = (MPI_Request*) malloc(sizeof(MPI_Request)*size);
    }

    if (syncflag == 0) {

        // RANK 0
        if ( rank == 0 ) {

            // if event happened to rank 0
            if (sendflag == 0) {

               MPI_Iprobe(MPI_ANY_SOURCE, EVENT, gcomm, &mflag, &mstatus);

                if (mflag == 1) {
                    MPI_Recv(&who, 1, MPI_INT, 
                            mstatus.MPI_SOURCE, EVENT, gcomm, MPI_STATUS_IGNORE);
                }
            }

            // event happened in rank 1 -> maxRank
            if (sendflag == 1 || mflag == 1) {
                 for (ri=1; ri<size; ri++) {
                    MPI_Isend(&who, 1, MPI_INT, ri, SYNCH, gcomm, &mrequest[ri-1]);
                }
                syncflag = 1;
            }

        // RANK 1 TO SIZE-1
        } else {
            MPI_Iprobe(0, SYNCH, gcomm, &mflag, MPI_STATUS_IGNORE);
            if (mflag == 1) {
                // check if rank sent a message and cancel if yes
                MPI_Recv(&who, 1, MPI_INT, 0, SYNCH, gcomm, MPI_STATUS_IGNORE);
                if (who != rank) {
                    MPI_Cancel(&srequest);
                }
                syncflag = 1;
            }
        }
    }

    if (syncflag == 0) {
        if ( check && sendflag == 0 ) {
            if (rank == 0) {
                sendflag = 1;
            } else {
                MPI_Isend(&rank, 1, MPI_INT, 0, EVENT, gcomm, &srequest);
                sendflag = 1;
            }
        }
    }

    // SYNC
    if (syncflag == 1) {
        // INIT SYNC, CLEAN UP MESSAGES
        if (rank == 0) {
            int isMsg = true;
            while (isMsg) {
                MPI_Iprobe(MPI_ANY_SOURCE, EVENT, gcomm, &mflag, &mstatus);
                if ( mflag == 1 ) {
                    MPI_Recv(&who, 1, MPI_INT, 
                            mstatus.MPI_SOURCE, EVENT, gcomm, MPI_STATUS_IGNORE);
                } else {
                    isMsg = false;
                }
            }
        }
        if (si==0) {
            if ( rank == 0 ) {
                syncCounter++;
            }
        }
        if (si < syncIter) { 
            // do_sync();
            si ++;
        } else {
            syncflag = 0;
            sendflag = 0;
            si = 0;
        }
    }  
    return syncflag;
}

