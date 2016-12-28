#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

//This it the status of the currently running task in the system
typedef enum
{
    eTaskRunning = 1,
    eTaskReady,
    eTaskCompleted,
    eTaskError
    
}tTaskStatus;

//This is the return result from the call method attached to the running task
typedef enum
{
    eTaskExecOK = 1,
    eTaskExecFailed,
    eTaskExecIsBlocked
}tTaskResult;

//Task prios
typedef enum
{
    eTaskPrioError,
    eTaskPrioUltra,
    eTaskPrioHigh,
    eTaskPrioModerate,
    eTaskPrioLow
    
}tTaskResultPrio;

//Task prototype for initialisation and data tranfer
typedef struct task
{
    //1.Explicite name for the new created task
    const char* taskName;
    
    //2.Task id is the value used by scheduler 
    int taskId;
    
    //3.Task priority used by the schedule to plan execution
    volatile tTaskResultPrio taskPrio;
    
    //4.Task execution function pointer
    tTaskResult (*tCallMethod)(const char*);
    
    //5.Task exec unit param counter in bytes
    int argc;
    
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
    
}tTaskProto;

// Mutex statuses
typedef enum
{
    eMutexLock = 1,
    eMutexClear,
    eMutexPendClear
}eMutexState;

//Mutex type
typedef struct 
{
    const char* mutex_name;
    const int mutex_id;
    const tTaskProto* taskBinded; 
    volatile eMutexState mutexState;

}tMutex;

//OS container status flag 
typedef enum
{
    eContaninerUnlocked = 1,
    eContainerBlocked
    
}eContainerState;

// Global task collection
typedef struct
{
    eContainerState containerStatus;
    tTaskProto* gTskPtr;
    int taskSlotCounter;
    int freeTaskSlots;
    tMutex* aMutexDescriptor;
    tTaskResult (**taskMethodArray)(const char*);   
}tOSCoreUtils;


extern tOSCoreUtils OSCoreUtils;


#endif
