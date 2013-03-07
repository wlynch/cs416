 ** Add a queue of processes to the user struct
 ** When a process is created, it should be added to the queue of its owner user
 ** A queue of users should be added to scheduler
 ** Inside the schedule method, we need to properly dequeue a user and select
 the next process
 ** Implement system call shenanigans (TO BE DOCUMENTED LATER)



#Notes from Billy#

Ramblings from bouncing ideas off of Poppy...

###User queue###
-process queue
--"completed"- queue of processes that have ran the number of time = their weight.
--still need to run- queue of processes that still have to run because of their weight (we do this so these processes are still interleaved).

###Scheduling:###
1) Round robin the users
2) Dequeue process. Decrement weight. If > 0, move to completed queue. Else requeue in run queue.
4) Go to next user, repeat. 
5) Repeat until run queue is empty. The completed queue then becomes the run queue.

###Problem: How do we add things to the user queues?###
- There has to be some method already in place to add things to the run queue. 

##How to we get around this?##
###Do everything in the run queue.##
- User round robin: Always pick the next highest user id. If none, pick the lowest. Since the LL is circular, worst case we can do this at the same time. 
- Process queue: Keep a flag in the process struct that allows us to tell whether or not the process is "completed". Not set on the specifics on how to do this yet (counting down vs counting up, static vs dyamic). If we get to the end and all flags are set, just pick the next process (relative to the current process) and unset the flags for this iteration.
