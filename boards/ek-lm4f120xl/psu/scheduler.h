#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "task_def.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"

// General OS data
extern tOSCoreUtils OSCoreUtils;

//Create heap memory for new tasks
tTaskProto* taskAllocateMemory(const char cCount);
bool deleteTaskMemory(void);

bool initScheduler(const char cTaskCount);
bool stopScheduler(void);

//Push task to task pool created in heap
bool registerTask(  const char* taskName,
                    const int taskId, 
                    tTaskResult (*tCallMethod)(const char*),
                    volatile tTaskResultPrio taskPrio,
                    const int argc, 
                    const char* argv);


bool disableInterrupts(void);
void launchExecutionPipe(void);
#endif
