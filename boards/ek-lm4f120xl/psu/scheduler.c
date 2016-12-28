#include "scheduler.h"
#include "inc/utils.h"

tOSCoreUtils OSCoreUtils;

//Allocate memory for the new tasks
tTaskProto* allocateTaskMemory(const char cCount)
{   
    int size = cCount * sizeof(tTaskProto);
    OSCoreUtils.gTskPtr = (tTaskProto*)malloc(size);
    OSCoreUtils.taskMethodArray = malloc(size);
    return OSCoreUtils.gTskPtr;
}

// Clear all heap allocations
bool deleteTaskMemory(void)
{
    if(OSCoreUtils.gTskPtr != NULL)
    {
        free(OSCoreUtils.gTskPtr);
        return true;
    }else
    {
        return false;
    }
}

//Allocate memory for tasks and set proper running flags
bool initScheduler(const char cTaskCount)
{
    bool status = disableInterrupts();
    if ( allocateTaskMemory(cTaskCount) != NULL)
    {
        OSCoreUtils.containerStatus = eContainerBlocked;
        OSCoreUtils.freeTaskSlots = cTaskCount;
        return true;
    }
    else
    {
        return false;
    }
}

bool startScheduler(void)
{
    if (IntMasterEnable())
    {
        OSCoreUtils.containerStatus = eContaninerUnlocked;
        launchExecutionPipe();
        return true;
    }
    else
    {
        return false;
    }
}

// Launch tasks execution 
void launchExecutionPipe(void)
{
    int i=0;
    for (; i < OSCoreUtils.taskSlotCounter; i++)
    {
        //Run execution pipe with no arguments in method
        if(OSCoreUtils.gTskPtr[i].argc == 0)
        {
            OSCoreUtils.taskMethodArray[i](NULL);
        }
        else
        {
            //Arguments are provided for this method.... count is OSCoreUtils.gTskPtr[i].argc
            OSCoreUtils.taskMethodArray[i]((const char *)OSCoreUtils.gTskPtr[i].argv);
        }
        
    }

}

bool stopScheduler(void)
{
    if (deleteTaskMemory())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*

    //1.Explicite name for the new created task
    const char* taskName;
    
    //2.Task id is the value used by scheduler 
    const int taskId;
    
    //3.Task priority used by the schedule to plan execution
    volatile tTaskResultPrio taskPrio;
    
    //4.Task execution function pointer
    tTaskResult (*tCallMethod)(void*);
    
    //5.Task exec unit param counter in bytes
    const int argc;
    
    //6.Task exec unit param array; 
    const char* argv;
    
    //7.Task exec unit result counter in bytes
    int resc;
    
    //8.Task exec unit result array;
    char* resv;
    
    //9.Current status of the running task
    volatile tTaskStatus taskStatus;
    
    //10.Task running time
    long tTaskExecutionTime;
    */
//Push task to task pool created in heap
bool registerTask(  const char* taskName,
                    const int taskId, 
                    tTaskResult (*tCallMethod)(const char*),
                    volatile tTaskResultPrio taskPrio,
                    const int argc, 
                    const char* argv)
{
    
    if(OSCoreUtils.freeTaskSlots)
    {
        int i = taskId;
        
        //Lock resource acces
        OSCoreUtils.containerStatus = eContainerBlocked;
        //
        
        //Initial task parameters
        OSCoreUtils.gTskPtr[i].taskName = taskName;
        OSCoreUtils.gTskPtr[i].taskId = taskId;
        OSCoreUtils.gTskPtr[i].taskPrio = taskPrio;
        OSCoreUtils.gTskPtr[i].tCallMethod = tCallMethod;
        OSCoreUtils.taskMethodArray[i] = tCallMethod;
        OSCoreUtils.gTskPtr[i].argc = argc;
        OSCoreUtils.gTskPtr[i].argv = argv;
        
        //Place task in ready to run state
        OSCoreUtils.gTskPtr[i].taskStatus = eTaskReady;
        
        //Release a free slot
        OSCoreUtils.freeTaskSlots--;
        return true;
    
    }
    else
    {
        //No task entries are available
        return false;
    }
   
}


//Disable global interrupts to do exclusive os operations
bool disableInterrupts(void)
{
    //Disable all global interrupts to avoid garbage data/ wrong flag set
    return IntMasterDisable();
}
