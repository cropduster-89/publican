#include "win32.h"
#include "../utils/utils.h"

static DWORD GetThreadID(void)
{
	BYTE *ThreadLocalStorage = (BYTE *)__readgsqword(0x30);
	DWORD ThreadID = *(DWORD *)(ThreadLocalStorage + 0x48);

	return(ThreadID);
}

static struct win32_work_queue *PlatformToWin32(struct platform_work_queue *queue)
{
 	return (struct win32_work_queue *)queue->platform;
}

extern void Win32_AddEntry(struct platform_work_queue *queue,
						   platform_work_queue_callback *callback,
						   void *data)
{
	DWORD newNextEntryToWrite = (queue->nextToWrite + 1) % ARRAY_COUNT(queue->entries);
	assert(newNextEntryToWrite != queue->nextToRead);
	struct platform_work_queue_entry *entry = queue->entries + queue->nextToWrite;
	entry->callback = callback;
	entry->data = data;
	++queue->completionGoal;

	__asm__ volatile("":::"memory");

	queue->nextToWrite = newNextEntryToWrite;
	ReleaseSemaphore(PlatformToWin32(queue)->semaphoreHandle, 1, 0);
}

extern bool DoNextQueueEntry(struct platform_work_queue *queue)
{
	BOOL needSleep = false;

	DWORD originalNextToRead = queue->nextToRead;
	DWORD newNextToRead = (originalNextToRead + 1) % ARRAY_COUNT(queue->entries);
	if(originalNextToRead != queue->nextToWrite) {
		DWORD index = InterlockedCompareExchange((LONG volatile *)&queue->nextToRead,
			newNextToRead, originalNextToRead);
		if(index == originalNextToRead) {
			struct platform_work_queue_entry entry = queue->entries[index];
			entry.callback(queue, entry.data);
			InterlockedIncrement((LONG volatile *)&queue->completionCount);
		}
	} else {needSleep = true;}

	return(needSleep);
}

extern DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	struct win32_thread_startup *thread = (struct win32_thread_startup *)lpParam;
	struct platform_work_queue *queue = thread->queue;

	DWORD testThreadID = GetThreadID();
	assert(testThreadID == GetCurrentThreadId());

	for(;;) {
		if(DoNextQueueEntry(queue)) {
			WaitForSingleObjectEx(PlatformToWin32(queue)->semaphoreHandle, INFINITE, FALSE);
		}
	}
}

extern void Win32_CompleteAllWork(struct platform_work_queue *queue)
{
	while(queue->completionGoal != queue->completionCount) {
		DoNextQueueEntry(queue);
	}
	queue->completionGoal = 0;
	queue->completionCount = 0;
}

/*
*	Initialises a new thread queue
*/
extern void MakeQueue(struct platform_work_queue *queue,
					  DWORD threadCount,
					  struct win32_thread_startup *startups)
{
	queue->completionGoal = 0;
	queue->completionCount = 0;
	queue->nextToWrite = 0;
	queue->nextToRead = 0;

	DWORD initialCount = 0;
	PlatformToWin32(queue)->semaphoreHandle = CreateSemaphoreEx(0, initialCount,
		threadCount, 0, 0, SEMAPHORE_ALL_ACCESS);

	for(uint32_t i = 0; i < threadCount; ++i) {
		struct win32_thread_startup *startup = startups + i;
		startup->queue = queue;

		DWORD threadID;
		HANDLE threadHandle = CreateThread(0, 0, ThreadProc,
			startup, 0, &threadID);
		CloseHandle(threadHandle);
	}
}