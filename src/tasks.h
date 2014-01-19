/*
 * tasks.h
 *
 *  Created on: Dec 30, 2013
 *      Author: felipeneves
 */

#ifndef TASKS_H_
#define TASKS_H_

os_stack_t TaskTryStack[64];
os_taskname_t Try[7];


extern os_stack_t TaskTryStack2[64];
extern os_taskname_t Try2[8];

extern os_stack_t TaskTryStack3[64];
extern os_taskname_t Try3[8];

extern os_stack_t TaskTryStack4[64];
extern os_taskname_t Try4[8];

extern os_stack_t TaskTryStack5[64];
extern os_taskname_t Try5[8];

extern os_stack_t TaskTryStack6[64];
extern os_taskname_t Try6[8];

extern os_stack_t TaskTryStack7[64];
extern os_taskname_t Try7[8];

extern os_stack_t TaskTryStack8[64];
extern os_taskname_t Try8[8];


//uma simples tarefa
taskptr_t TaskTry(void *TaskArgs);


//outra simples tarefa
taskptr_t TaskTry2(void *TaskArgs);


//outra simples tarefa
taskptr_t TaskTry3(void *TaskArgs);


//outra simples tarefa
taskptr_t TaskTry4(void *TaskArgs);


//outra simples tarefa
taskptr_t TaskTry5(void *TaskArgs);


//outra simples tarefa
taskptr_t TaskTry6(void *TaskArgs);


//outra simples tarefa
taskptr_t TaskTry7(void *TaskArgs);



//outra simples tarefa
taskptr_t TaskTry8(void *TaskArgs);





#endif /* TASKS_H_ */
