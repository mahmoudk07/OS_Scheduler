/***********Includes**********/
#include "main_header.h"
****************************Functions Definitions ************************** /
    // Multi-Level uses this please do not change
    void PRINT_MEMORY_DEALLOC(Process *curr_Proc, int Time, FILE *processess_file)
{
    char line_to_print[256];
    int start = curr_Proc->memindex * 32;
    int end = ((curr_Proc->memindex + curr_Proc->no_of_blocks) * 32 - 1);
    snprintf(line_to_print, 200, "At time %d freed %d bytes from process %d from %d to %d", Time, curr_Proc->memsize, curr_Proc->Process_ID, start, end); // puts string into buffer
    strcat(line_to_print, "\n");
    // printf(line_to_print);
    fputs(line_to_print, processess_file); // writing data into file
}

void Add_waiting_SJF(Process_List *P_Queue) // add waiting for all processes in queue
{
    /*inc waiting for not runing processes*/
    struct Processes_Node *current = P_Queue->front;
    while (current != NULL)
    {
        ++current->Process_Data.Waiting_time;
        current = current->Next;
    }
}
int RUN_CURR_PROCESS(struct Processes_Node *curr_Proc, int *Process_Semaphore, Process_List *P_Queue, int Time, FILE *processess_file, bool *memory, FILE *processess_mem_file)
{

    /*run_curr_process_logic*/
    up(Process_Semaphore[curr_Proc->Process_Data.Process_ID - 1]);
    Add_waiting_SJF(P_Queue);                 /*Function to increase waiting time for not runing processes in ready queue*/
    --curr_Proc->Process_Data.Remaining_time; /*decrease remaining time for the runing process*
    ////////////////////////////
    /*check if this clock was process last_clock*/
    if (curr_Proc->Process_Data.Remaining_time == 0)
    {

        curr_Proc->Process_Data.State = FINISHED;
        /*Print finish process information*/
        PRINT_CURR_PROCESS(&curr_Proc->Process_Data, Time + 1, processess_file);
        mem_dealloc(memory, curr_Proc->Process_Data.no_of_blocks, curr_Proc->Process_Data.memindex);
        PRINT_MEMORY_DEALLOC(&curr_Proc->Process_Data, Time + 1, processess_mem_file);
        return 1;
    }
    return 0;
}

void PRINT_CURR_PROCESS(Process *curr_Proc, int Time, FILE *processess_file)
{
    static char *state[4] = {"started", "finished", "stopped", "resumed"};
    char line_to_print[256];
    snprintf(line_to_print, 200, "At time %d process %d %s arr %d total %d remain %d wait %d", Time, curr_Proc->Process_ID, state[curr_Proc->State], curr_Proc->Arrival_time, curr_Proc->Running_time, curr_Proc->Remaining_time, curr_Proc->Waiting_time); // puts string into buffer
    if (curr_Proc->State == FINISHED)
    {
        /*calculate TA and WTA*/

        curr_Proc->TA = Time - curr_Proc->Arrival_time;
        curr_Proc->W_TA = (float)curr_Proc->TA / (float)curr_Proc->Running_time;
        /////////////////////////////////////////////
        char concatenated_line[100];
        sprintf(concatenated_line, " TA %d WTA %.2f", curr_Proc->TA, curr_Proc->W_TA); // puts string into buffer
        strcat(line_to_print, concatenated_line);
    }
    /*write to file*/
    strcat(line_to_print, "\n");
    // printf(line_to_print);
    fputs(line_to_print, processess_file); // writing data into file
}

void PRINT_MEMORY_ALLOC(Process *curr_Proc, int Time, FILE *processess_file)
{
    char line_to_print[256];
    int start = curr_Proc->memindex * 32;
    int end = ((curr_Proc->memindex + curr_Proc->no_of_blocks) * 32 - 1);
    snprintf(line_to_print, 200, "At time %d allocated %d bytes for process %d from %d to %d", Time, curr_Proc->memsize, curr_Proc->Process_ID, start, end); // puts string into buffer
    strcat(line_to_print, "\n");
    // printf(line_to_print);
    fputs(line_to_print, processess_file); // writing data into file
}

void COPY_then_DEQUEUE_HEAD(struct Processes_Node *curr_Proc, Process_List *P_Queue)
{
    curr_Proc->Process_Data = P_Queue->front->Process_Data;
    popFromQueue(P_Queue);
    curr_Proc->Process_Data.State = STARTED;
}

int get_no_of_blocks(int mem_size)
{
    if (mem_size < 32)
    {
        return 1;
    }
    int size = ceil(log2((double)mem_size));
    printf("------the size is %d---------", size);
    return (pow(2, size) / 32);
}

int check_first_empty_location_if_odd(bool *mem)
{
    for (int i = 1; i < 32; i = i + 1)
    {
        if (mem[i - 1] == true && (i % 2) == 1 && mem[i] == false)
        {
            mem[i] = true;
            return i;
        }
    }
    return -1;
}

int get_first_place(bool *mem, int blocks_to_reserve)
{
    printf("\nI'm in\n");
    bool check = true;
    for (int i = 0; i < 32; i = i + blocks_to_reserve)
    {
        check = true;
        for (int k = i; k < i + blocks_to_reserve; k++) // check if this place vacant
        {
            if (mem[k] == true)
            {
                check = false;
                break;
            }
        }
        // found
        if (check)
        {
            for (int k = i; k < i + blocks_to_reserve; k++) // check if this place vacant
            {
                mem[k] = true;
            }
            return i;
        }
    }
    return -1;
}

int mem_alloc(bool *mem, int blocks_to_reserve)
{
    int op1 = 32;
    int op2 = 32;

    bool check = true;
    bool check2 = true;
    bool check4 = true;
    bool check5 = true;
    int blocks = blocks_to_reserve;

    if (blocks == 16)
    {
        return get_first_place(mem, blocks_to_reserve);
    }
    if (blocks_to_reserve == 1)
    {
        return get_first_place(mem, blocks_to_reserve);
    }
    check2 = false;

    for (int j = 0; j < 32; j = j + blocks) // try to find empty (blocks_to_reserve) which is reserved from both sides
    {
        check = true;
        for (int k = j; k < j + blocks; k++) // check if this place vacant
        {
            if (mem[k] == true)
            {
                check = false;
                break;
            }
        }

        if (check) // found empty space now check if it's reserved from both sides
        {
            check2 = true;
            if (j == 0) // the empty place is at first place in array -> check it's left only
            {

                check4 = true;
                for (int k = j + blocks; k < j + blocks + blocks; k++) // check if this place vacant
                {
                    if (mem[k] == true)
                    {
                        check4 = false;
                        break;
                    }
                }

                if (check4 == false) // found
                {

                    for (int k = j; k < j + blocks_to_reserve; k++) // reserve
                    {
                        mem[k] = true;
                    }
                    return j;
                }
            }
            else if (j > 0 && j < 32 - blocks)
            {
                check4 = true;
                for (int k = j + blocks; k < j + blocks + blocks; k++) // check if this place vacant
                {
                    if (mem[k] == true)
                    {
                        check4 = false;
                        break;
                    }
                }
                check5 = true;
                for (int k = j - blocks; k < j; k++) // check if this place vacant
                {
                    if (mem[k] == true)
                    {
                        check5 = false;
                        break;
                    }
                }

                if (check4 == false && check5 == false) // found
                {
                    for (int k = j; k < j + blocks_to_reserve; k++) // check if this place vacant
                    {
                        mem[k] = true;
                    }
                    return j;
                }
                else if (check4 == false)
                {
                    op1 = (j < op1) ? j : op1;
                }
                else if (check5 == false)
                {
                    op2 = (j < op2) ? j : op2;
                }
            }
            else if (j == 32 - blocks) // the empty place is at second place in array -> check it's right only
            {
                check4 = true;
                for (int k = j - blocks; k < j; k++) // check if this place vacant
                {
                    if (mem[k] == true)
                    {
                        check4 = false;
                        break;
                    }
                }

                if (check4 == false) // found
                {
                    for (int k = j; k < j + blocks_to_reserve; k++) // reserve
                    {
                        mem[k] = true;
                    }
                    return j;
                }
            }
        }
    }
    if (!check2) // didnt find any space
    {
        return -1;
    }

    if (op1 == 32 && op2 == 32)
    {
        return get_first_place(mem, blocks_to_reserve);
    }
    else
    {
        int best = (op1 > op2) ? op2 : op1;
        for (int i = best; i < best + blocks_to_reserve; i++)
        {
            mem[i] = true;
        }
        return best;
    }
    /* code */
    //}
}

void mem_dealloc(bool *mem, int blocks_to_free, int index)
{
    for (int i = index; i < index + blocks_to_free; i++)
    {
        mem[i] = false;
    }
}
/* Function to Insert Process into Queue */
struct Processes_Node *Insert_Process(Process_List *Queue, Process *N)
{
    // Allocate memory for new node
    struct Processes_Node *link = (struct Processes_Node *)malloc(sizeof(struct Processes_Node));

    // insert the data to the new node and make the next point to null
    link->Process_Data.Process_ID = N->Process_ID;
    link->Process_Data.Arrival_time = N->Arrival_time;
    link->Process_Data.Remaining_time = N->Remaining_time;
    link->Process_Data.Running_time = N->Running_time;
    link->Process_Data.Waiting_time = N->Waiting_time;
    link->Process_Data.TA = N->TA;
    link->Process_Data.W_TA = N->W_TA;
    link->Process_Data.State = N->State;
    link->Process_Data.Priority = N->Priority;
    link->Process_Data.memsize = N->memsize;
    link->Process_Data.memindex = N->memindex;
    link->Process_Data.no_of_blocks = N->no_of_blocks;

    // If head is empty, create new list
    if (Queue->front == NULL)
    {
        Queue->front = link;
        Queue->rear = link;
        return link;
    }
    /* If not empty --> insert after Rear */
    link->Next = NULL;
    Queue->rear->Next = link;
    Queue->rear = link;
    return link;
}

struct Processes_Node *Insert_Circular_Queue(Process_List *Queue, Process *N)
{
    // Allocate memory for new node
    struct Processes_Node *link = (struct Processes_Node *)malloc(sizeof(struct Processes_Node));

    // insert the data to the new node and make the next point to null
    link->Process_Data.Process_ID = N->Process_ID;
    link->Process_Data.Arrival_time = N->Arrival_time;
    link->Process_Data.Remaining_time = N->Remaining_time;
    link->Process_Data.Running_time = N->Running_time;
    link->Process_Data.Waiting_time = N->Waiting_time;
    link->Process_Data.TA = N->TA;
    link->Process_Data.W_TA = N->W_TA;
    link->Process_Data.State = N->State;
    link->Process_Data.Priority = N->Priority;
    link->Process_Data.memsize = N->memsize;
    link->Process_Data.memindex = N->memindex;
    link->Process_Data.no_of_blocks = N->no_of_blocks;

    // If head is empty, create new list
    if (Queue->front == NULL)
    {
        Queue->front = link;
    }
    else
    {
        Queue->rear->Next = link;
    }
    /* If not empty --> insert after Rear */
    Queue->rear = link;
    Queue->rear->Next = Queue->front;

    return link;
}
/* Function to Read from file and insert it into linked list*/
void Read_file(Process_List *LinkedList, char *FileName, int mode)
{
    Process *proc = (Process *)malloc(sizeof(Process));
    FILE *pFile;
    pFile = fopen(FileName, "r");
    char line[256];
    int ff[4];
    while (fgets(line, sizeof(line), pFile))
    {
        if (line[0] == '#')
        {
            continue;
        }

        char *token = strtok(line, "\t");
        int c = 0;
        while (token != NULL)
        {
            ff[c] = atoi(token);
            token = strtok(NULL, "\t");
            c++;
        }

        /*set process parameters*/
        proc->Process_ID = ff[0];
        proc->Arrival_time = ff[1];
        proc->Running_time = ff[2];
        proc->Remaining_time = proc->Running_time;
        proc->Priority = (mode == SJF) ? ff[2] : ff[3];
        proc->Waiting_time = 0;
        proc->TA = 0;
        proc->W_TA = 0;
        proc->State = STOPPED;
        proc->memsize = ff[4];
        proc->memindex = -1;

        /*calculate no of blocks*/
        proc->no_of_blocks = get_no_of_blocks(proc->memsize);

        /*Insert process into linked list*/
        Insert_Process(LinkedList, proc);
    }
    /*Close file*/
    fclose(pFile);
}

/* Function to Delete from circular queue in RR Algorithm */
void remove_From_Circular(Process_List *C_Queue, int id)
{
    struct Processes_Node *delete_node = NULL;
    struct Processes_Node *current = NULL;
    struct Processes_Node *prev = NULL;

    // check if the linked list is empty
    if (C_Queue->front == NULL)
    {
        return;
    }

    // If this is the last node to be deleted
    if (C_Queue->front->Process_Data.Process_ID == id && C_Queue->front == C_Queue->rear)
    {
        free(C_Queue->front);
        C_Queue->front = NULL;
        C_Queue->rear = NULL;
        return;
    }

    // check the first node in the linked list
    if (C_Queue->front->Process_Data.Process_ID == id)
    {
        delete_node = C_Queue->front;
        // move head to next node
        C_Queue->front = C_Queue->front->Next;
        C_Queue->rear->Next = C_Queue->front;
        free(delete_node);
        return;
    }

    // check the rest of nodes in the linked list
    current = C_Queue->front->Next;
    prev = C_Queue->front;
    while (current != C_Queue->front)
    {
        if (current->Process_Data.Process_ID == id)
        {
            // element found
            delete_node = current;
            if (delete_node == C_Queue->rear)
            {
                C_Queue->rear = prev;
            }
            prev->Next = current->Next;
            free(delete_node);
            return;
        }
        prev = current;
        current = current->Next;
    }
}

/* Function to Check on circular Queue is empty used for RR Algorithm */
int IsEmpty_Queue(Process_List *C_Queue)
{
    return C_Queue->front == NULL;
}

/* Function to increase waiting time for not runing processes used for RR Algorithm */
void calc_Proc_waiting(Process_List *C_Queue, struct Processes_Node *curr_Proc)
{
    /*check on first node if not current proc to inc waiting time*/
    if (C_Queue->front == C_Queue->rear)
    {
        return;
    }
    if (C_Queue->front != curr_Proc)
    {
        ++C_Queue->front->Process_Data.Waiting_time;
    }
    /*check on rest of queue if not current so inc waiting*/
    struct Processes_Node *current = C_Queue->front->Next;
    while (current != C_Queue->front)
    {
        if (current != curr_Proc)
        {
            ++current->Process_Data.Waiting_time;
        }
        current = current->Next;
    }
}

/* Function to increase waiting time for not runing processes used for HPF Algorithm */
void calc_Proc_waiting_Prio(Process_List *P_Queue)
{
    /*inc waiting for not runing processes*/
    struct Processes_Node *current = P_Queue->front->Next;
    while (current != NULL)
    {
        ++current->Process_Data.Waiting_time;
        current = current->Next;
    }
}
struct Processes_Node *newPriorityQueueNode(Process *N)
{
    int priority = N->Priority;
    struct Processes_Node *temp = (struct Processes_Node *)malloc(sizeof(struct Processes_Node));
    temp->priority = priority;
    temp->Process_Data = *N;
    temp->Next = NULL;

    return temp;
}

Process *peekIntoQueue(Process_List *P_Queue)
{
    /*if list is already empty*/
    if (P_Queue->front == NULL)
    {
        return NULL;
    }
    return &P_Queue->front->Process_Data;
}

void popFromQueue(Process_List *P_Queue)
{
    /*if list is already empty*/
    if (P_Queue->front == NULL)
    {
        return;
    }
    struct Processes_Node *temp = P_Queue->front;
    P_Queue->front = P_Queue->front->Next;
    free(temp);
}

void pushIntoPriorityQueue(Process_List *P_Queue, Process *newProcess)
{
    // Create new Node
    struct Processes_Node *temp = newPriorityQueueNode(newProcess);
    /*if list is already empty*/
    if (P_Queue->front == NULL)
    {
        P_Queue->front = temp;
        temp->Next = NULL;
        return;
    }

    struct Processes_Node *start = P_Queue->front;
    struct Processes_Node *prev_start = P_Queue->front;
    if (P_Queue->front->priority > newProcess->Priority)
    {
        P_Queue->front->Process_Data.State = STOPPED;
        temp->Next = P_Queue->front;
        P_Queue->front = temp;
    }
    else
    {
        start = P_Queue->front->Next;
        while (start != NULL && start->priority < newProcess->Priority)
        {
            start = start->Next;
            prev_start = prev_start->Next;
        }
        temp->Next = prev_start->Next;
        prev_start->Next = temp;
    }
}

int isPriorityQueueEmpty(Process_List *P_Queue)
{
    return P_Queue->front == NULL;
}

void pushIntoQueue(Process_List *P_Queue, Process *newProcess)
{
    struct Processes_Node *temp = newPriorityQueueNode(newProcess);
    /*if list is already empty*/
    if (P_Queue->front == NULL)
    {
        P_Queue->front = temp;
        temp->Next = NULL;
        return;
    }
}
void pushIntoQueueForMultiLevel(Process_List *Queue, Process *N)
{
    // Allocate memory for new node
    struct Processes_Node *link = newPriorityQueueNode(N);
    // insert the data to the new node and make the next point to null

    // If head is empty, create new list
    if (Queue->front == NULL)
    {
        Queue->front = link;
        Queue->rear = link;
        return;
    }
    /* If not empty --> insert after Rear */
    link->Next = NULL;
    Queue->rear->Next = link;
    Queue->rear = link;
    return;
}

void pushIntoMultiLevel(MultiLevel *m, Process *newProcess)
{
    int priority = newProcess->Priority;
    Process_List *priorityList = &m->listOfQueues[priority];
    pushIntoQueueForMultiLevel(priorityList, newProcess);
}
void pushIntoNextLevel(int currLevel, Process *processToBePushedIntoNextLevel, MultiLevel *m)
{
    int priority = currLevel + 1;
    if (priority != 11)
    {
        Process_List *priorityList = &m->listOfQueues[priority];
        pushIntoQueueForMultiLevel(priorityList, processToBePushedIntoNextLevel);
    }
    else
    {
        printf("pushing into process to be pushed into next level\n");
        pushIntoQueueForMultiLevel(&m->toBeReturnedToItsLevel, processToBePushedIntoNextLevel);
    }
    printf("popping from currLevel: %d\n", currLevel);
    popFromQueue(&m->listOfQueues[currLevel]);
    printf(("popped\n"));
}
int AreAllLevelsEmpty(MultiLevel *m)
{
    for (int i = 0; i < 11; i++)
    {
        if (m->listOfQueues[i].front != NULL)
        {
            return 0;
        }
    }
    return 1;
}
int isMultiLevelEmpty(MultiLevel *m)
{
    if (m->toBeReturnedToItsLevel.front == NULL && AreAllLevelsEmpty(m))
        return 1;
    else
        return 0;
}
int pushAllProcessBackToItsLevel(MultiLevel *m)
{
    while (!isPriorityQueueEmpty(&m->toBeReturnedToItsLevel))
    {
        Process *processData = peekIntoQueue(&m->toBeReturnedToItsLevel);
        printf("proces id:%d process remaining time: %d prio: %d memsize: %d\n", processData->Process_ID, processData->Remaining_time, processData->Priority, processData->memsize);
        pushIntoMultiLevel(m, processData);
        popFromQueue(&m->toBeReturnedToItsLevel);
    }
}
Process *getNextProcessFromMultiLevel(MultiLevel *m, int *currentLevel)
{
    for (int i = 0; i < 11; i++)
    {
        if (!isPriorityQueueEmpty(&m->listOfQueues[i]))
        {
            printf("id:%d", peekIntoQueue(&m->listOfQueues[i])->Process_ID);

            printf("process head: %d and i = %d \n", peekIntoQueue(&m->listOfQueues[i])->Process_ID, i);
            *currentLevel = i;
            return peekIntoQueue(&m->listOfQueues[i]);
        }
    }
    return NULL;
}

void AddWaitingMultiLevel(MultiLevel *m, Process *currentProcess)
{
    for (int i = 0; i < 11; i++)
    {
        Add_waiting_SJF(&m->listOfQueues[i]);
    }
    Add_waiting_SJF(&m->toBeReturnedToItsLevel);
    --currentProcess->Waiting_time;
}

void runMultiLevelProcess(Process **currentProcess, int currentLevel, int *Process_Semaphore, MultiLevel *m, int Time, FILE *processess_file, int *finishedProcessCount, int *total_waiting_time, float *total_WTA_time, bool *memory, FILE *processess_mem_file)
{
    /*run_curr_process_logic*/
    printf("currProcess: %d, remaining time: %d\n", (*currentProcess)->Process_ID, (*currentProcess)->Remaining_time);
    printf("proces id:%d process remaining time: %d prio: %d memsize: %d\n", (*currentProcess)->Process_ID, (*currentProcess)->Remaining_time, (*currentProcess)->Priority, (*currentProcess)->memsize);
    int x = (*currentProcess)->Process_ID - 1;
    int y = (*currentProcess)->Remaining_time;
    AddWaitingMultiLevel(m, (*currentProcess)); /*Function to increase waiting time for not runing processes in ready queue*/
    --(*currentProcess)->Remaining_time;        /*decrease remaining time for the runing process*/
    /*check if this clock was process last_clock*/
    if ((*currentProcess)->Remaining_time == 0)
    {
        (*currentProcess)->State = FINISHED;

        /*Print finish process information*/
        PRINT_CURR_PROCESS((*currentProcess), Time + 1, processess_file);
        mem_dealloc(memory, (*currentProcess)->no_of_blocks, (*currentProcess)->memindex);
        PRINT_MEMORY_DEALLOC((*currentProcess), Time + 1, processess_mem_file);
        *total_waiting_time += (*currentProcess)->Waiting_time;
        *total_WTA_time += (*currentProcess)->W_TA;

        popFromQueue(&m->listOfQueues[currentLevel]);
        *currentProcess = NULL;
        ++*finishedProcessCount;
    }
    if (y > 1)
        up(Process_Semaphore[x]);
}

void setProcessState(Process *p)
{
    if (p->Remaining_time == p->Running_time)
        p->State = STARTED;
    else
        p->State = RESUMED;
}