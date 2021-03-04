#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_LENGTH 32
#define MAX_PROCESS 100

#define MAX_ID_LEN 
#define INVALID_IDX -1

#define FILENAME_OUTPUT "output.txt"
#define FILENAME_INPUT "input.txt"

typedef enum
{
    READY,
    RUNNING,
    WAITING,
    TERMINATED,
    INVALID
} process_state_e;

struct info
{
    /**
     * ID of the process.
     * Provided to us via input file.
     */
    int     id;
    
    /**
     * When the process is available for processing (effectively when it moves
     * from new state to ready state). Before this, the process should never be
     * moved into the running state.
     * Provided to us via input file.
     */
    int     arrivalTime;
    
    /**
     * Total ammount of time this process must spend in the running state. This
     * does not include time spent in the waiting state (waiting on I/O).
     * Provided to us via input file.
     */
    int     totalCpuTime;
    
    /**
     * Frequency that the process must wait for I/O actions to complete.
     * Provided to us via input file.
     */
    int     IOfreq;
    
    /**
     * How long the I/O takes. The process must sit in the waiting state for the
     * length of this timer.
     * Provided to us via input file.
     */
    int     IOdur;
    
    /**
     * Running counter of how long the process has spent waiting for I/O. Once
     * this hits the IO Duration, then the process must move back into the
     * ready state.
     */
    int     IOtime;
    
    /**
     * Running counter of how long the process has spent in the running state.
     * Once this hits the Total CPU Time, then the process must move into the
     * termintated state.
     */
    int     currentRunTime;
    
    /**
     * Current state the process is in.
     */
    process_state_e state;
};



/**
 * Function: getProcess
 *
 * Parameters:
 *  proc    - Pointer to list of processes
 *  entries - Number of processes to print
 *  state   - State we want to get
 *  tick    - Current clock tick
 *
 * Iterates through passed in array to find an entry that matches the passed in
 * state and it greater than or equal to the passed in tick (should be the
 * current tick).
 *
 * Returns:
 *  Index of the matching process in the passed in array, or INVALID_IDX if
 *  there are no matches.
 */
int getProcess(struct info *proc, int entries, process_state_e state, int tick)
{
    for (int i = 0; i < entries; proc++)
    {
        if ((proc->state == state) && (proc->arrivalTime <= tick))
        {
            /* Return the line number of the process that is in the desired state */
            return i;   
        }
        i++;
    }
    
    /* Nothing is in the desired state */
    return INVALID_IDX;
}

int countTerminated(struct info *proc, int entries, int tick){
    int termCount = 0;
    for(int i = 0; i < entries; i++){
        if(proc[i].state == TERMINATED && (proc[i].arrivalTime <= tick)){
            termCount++;
        }
    }
    return termCount;
}


int main(int argc, char *argv[])
{
    process_state_e oldState;


    
    // Reading the input file
    char    line[MAX_PROCESS][MAX_LENGTH];
	char    fname[10] = "input.txt";
    FILE    *fptr = NULL;
    FILE    *outputFile = NULL;
    int     i = 0;
    int     tot = 0;
    int     opt;
    int     tick = 0;
    int     runningIdx;
    int     readyIdx;
    int     processTerminated;

    
    
       
    /**
     * If no filename to read from has been intered, then abort. 
     */
    if (strncmp(fname, "", 1) == 0)
    {
        printf("Invalid filename\n");
        return 0;   
    }
    
    /**
     * Open default output file to write to
     */
    outputFile = fopen(FILENAME_OUTPUT, "a");

    /**
     * Read input file into memory, we will parse it later. We will also count
     * how many lines we have read, so that we know how many processes to
     * expect.
     */
    fptr = fopen(fname, "r");

    if(fptr == NULL){
        printf("Failed to open input file\n");
        return 0;
    }

        while (fgets(line[tot], sizeof(line[1]), fptr)) {
            /* note that fgets don't strip the terminating \n, checking its
            presence would allow to handle lines longer that sizeof(line) */
            printf("%s : Read into memory", line[tot]);
            tot++;
        }



    /**
     * Allocate memory based on how many processes we have read in from the
     * input file, and assign any other parameters to default values.
     */
    struct info *processes = (struct info*)malloc(tot*sizeof(struct info));
    struct info processArray[tot];
    processes = processArray;
    for(i = 0; i < tot; ++i)
    {
        int charIndex;
        //temp string for holding each process parameter, MAX DIGITS: 8
        char stringified[8];
        for(charIndex = 0; !isblank(line[i][charIndex]); charIndex++){
            stringified[charIndex] = line[i][charIndex];
        }
        int value;
        //parse temp string for integer
        sscanf(stringified, "%d", &value);
        processes->id = value;
        //clear temp string and value holder
        memset(stringified,0,strlen(stringified));
        value = 0;
        int ii = 0;
        for(charIndex = charIndex + 1; !isblank(line[i][charIndex]); charIndex++){
            stringified[ii] = line[i][charIndex];
            ii++;
        }
        sscanf(stringified, "%d", &value);
        processes->arrivalTime = value;
        //clear temp string and value holder
        memset(stringified,0,strlen(stringified));
        value = 0;
        ii = 0;
        for(charIndex = charIndex + 1; !isblank(line[i][charIndex]); charIndex++){
            stringified[ii] = line[i][charIndex];
            ii++;
        }
        sscanf(stringified, "%d", &value);
        processes->totalCpuTime = value;
        //clear temp string and value holder
        memset(stringified,0,strlen(stringified));
        value = 0;
        ii = 0;
        for(charIndex = charIndex + 1; !isblank(line[i][charIndex]); charIndex++){
            stringified[ii] = line[i][charIndex];
            ii++;
        }
        sscanf(stringified, "%d", &value);
        processes->IOfreq = value;
        //clear temp string and value holder
        memset(stringified,0,strlen(stringified));
        value = 0;
        ii = 0;
        for(charIndex = charIndex + 1; !isblank(line[i][charIndex]); charIndex++){
            stringified[ii] = line[i][charIndex];
            ii++;
        }
        sscanf(stringified, "%d", &value);
        processes->IOdur = value;
        //clear temp string and value holder
        memset(stringified,0,strlen(stringified));
        value = 0;        

        //assign defaults to new process
        processes->state = READY;
        processes->IOtime = 0;
        processes->currentRunTime = 0;
        //check if on last process to keep from iterating past
        if(i < tot - 1) processes++;
    }

   
    /**
     * Run in an infinite loop until all of the processes have been put into the
     * terminated state.
     */
    while(1)
    {
        processes = processArray;
        processTerminated = 0;
        for (i = 0; i < tot; i++)
        {
            /**
             * Count all the processes that are in the terminated state, we will
             * use this to see if we should terminate the program.
             */
            if(getProcess(processes, tot, TERMINATED, tick) != -1){
                processTerminated++;
                processes++;
            }
        }
        
        if (processTerminated == tot)
        {
            /**
             * Check to see if the number of terminated processes is the same as
             * the number of processes we have, if so, we have terminated all
             * the processes and will finish, otherwise keep going.
             */
                
            fclose(outputFile);
            free(line);
            free(processArray);
            free(processes);
            return 0;
        }
        
        /**
         * Check all the processes that are in the waiting state, and see if 
         * the IO they are waiting on has completed. If so, move it into the
         * ready state, otherwise increment its wait timer.
         */
        processes = processArray;
        for (i = 0; i < tot; processes++)
        {
            if (processes->state == WAITING)
            {
                ++processes->IOtime;
                if (processes->IOtime >= processes->IOdur)
                {
                    /**
                     * It is time to move the process from waiting back to ready
                     * so that it is available to be run again.
                     */
                    oldState = processes->state;
                    processes->state = READY;
                    fprintf(outputFile, "%d %d %d %d\n", tick, processes->id, oldState, processes->state);
                }
            }
            i++;
        }
        
        /**
         * Check all the processes that are in the running state and increment
         * their run time. If this puts them to their required run time, then
         * move the process into the terminated state. Otherwise, if it is time
         * to run I/O, then move it into the waiting state.
         * If nothing is running, then move something from ready state to 
         * running.
         */
        processes = processArray;
        runningIdx = getProcess(processes, tot, RUNNING, tick);
        if (runningIdx != INVALID_IDX)
        {
            /**
             * We have a process that is currently running, so we will increment
             * its running time and wait for the next tick.
             */
            ++processArray[runningIdx].currentRunTime;
            if (processArray[runningIdx].currentRunTime >= processArray[runningIdx].totalCpuTime)
            {
                /**
                 * This process has finished running, it will be marked as 
                 * terminated 
                 */
                oldState = processArray[runningIdx].state;
                processArray[runningIdx].state = TERMINATED;
                fprintf(outputFile, "%d %d %d %d\n", tick, processArray[runningIdx].id, oldState, processArray[runningIdx].state);
           }
            else
            {
                /**
                 * Check to see if it is time to interrupt the running process
                 * for I/O actions
                 */
                if (processArray[runningIdx].currentRunTime % processArray[runningIdx].IOfreq == 0)
                {
                    oldState = processArray[runningIdx].state;
                    processArray[runningIdx].state = WAITING;
                    fprintf(outputFile, "%d %d %d %d\n", tick, processArray[runningIdx].id, oldState, processArray[runningIdx].state);
                }
            }
        }       
        else
        {
            /**
             * There are no running processes, move something that is ready to
             * running
             */
            processes = processArray;
            readyIdx = getProcess(processes, tot, READY, tick);
            if (readyIdx != INVALID_IDX)
            {
                oldState = processArray[readyIdx].state;
                processArray[readyIdx].state = RUNNING;
                fprintf(outputFile, "%d %d %d %d\n", tick, processArray[readyIdx].id, oldState, processArray[readyIdx].state);
            }
            
        }
        ++tick;
    }

    fclose(outputFile);
    free(line);
    free(processArray);
    free(processes);
    return 0;
}