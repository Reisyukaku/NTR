#ifndef SVC_H
#define SVC_H

typedef enum{
	MEMOP_FREE = 1,
	MEMOP_RESERVE = 2,
	MEMOP_COMMIT = 3,
	MEMOP_MAP = 4,
	MEMOP_UNMAP = 5,
	MEMOP_PROTECT = 6,
	MEMOP_REGION_APP = 0x100,
	MEMOP_REGION_SYSTEM = 0x200,
	MEMOP_REGION_BASE = 0x300,
	MEMOP_LINEAR = 0x1000,
}MEMORY_OPERATION;

	u32* getThreadCommandBuffer(void);
	
	Result svc_backDoor(void* callback);
	Result svc_getThreadList(u32* threadCount, u32* threadIds, s32 threadIdMaxCount, Handle hProcess);
	Result svc_getDmaState(u32* state, Handle dma);
	Result svc_startInterProcessDma(Handle* hdma, Handle dstProcess, void* dst, Handle srcProcess, const void* src, u32 size, u32* config);
	
	Result svc_writeProcessMemory(Handle debug, void const* buffer, u32 addr, u32 size);
	Result svc_readProcessMemory(void* buffer, Handle debug, u32 addr, u32 size);
	Result svc_debugActiveProcess(s32* handle_out, u32 pid);
	Result svc_getProcessList(s32* processCount, u32* processIds, s32 processIdMaxCount);
	
	Result svc_controlProcessMemory(Handle hProcess, void* Addr0, void* Addr1, u32 size, u32 Type, u32 Permissions);
	
	Result svc_openProcess(Handle* process, u32 processId);
	Result svc_addCodeSegment(u32 addr, u32 size);
	Result svc_flushProcessDataCache(Handle handle, u32 addr, u32 size);
	Result svc_invalidateProcessDataCache(Handle handle, u32 addr, u32 size);
	Result svc_controlMemory(u32* outaddr, u32 addr0, u32 addr1, u32 size, u32 operation, u32 permissions); //(outaddr is usually the same as the input addr0)
	void svc_exitProcess(void);
	Result svc_createThread(Handle* thread, void* entrypoint, u32 arg, u32* stacktop, s32 threadpriority, s32 processorid);
	void svc_exitThread();
	void svc_sleepThread(s64 ns);
	Result svc_createMutex(Handle* mutex, bool initialLocked);
	Result svc_releaseMutex(Handle handle);
	Result svc_releaseSemaphore(s32* count, Handle semaphore, s32 releaseCount);
	Result svc_createEvent(Handle* event, u8 resettype);
	Result svc_signalEvent(Handle handle);
	Result svc_clearEvent(Handle handle);
	Result svc_createMemoryBlock(Handle* memblock, u32 addr, u32 size, u32 mypermission, u32 otherpermission);
	Result svc_mapMemoryBlock(Handle memblock, u32 addr, u32 mypermissions, u32 otherpermission);
	Result svc_unmapMemoryBlock(Handle memblock, u32 addr);
	Result svc_waitSynchronization1(Handle handle, s64 nanoseconds);
	Result svc_waitSynchronizationN(s32* out, Handle* handles, s32 handlecount, bool waitAll, s64 nanoseconds);
	Result svc_arbitrateAddress(Handle arbiter, u32 addr, u8 type, s32 value, s64 nanoseconds);
	Result svc_closeHandle(Handle handle);
	u64 svc_getSystemTick();
	Result svc_getSystemInfo(s64* out, u32 type, s32 param);
	Result svc_connectToPort(volatile Handle* out, const char* portName);
	Result svc_sendSyncRequest(Handle session);
	Result svc_getProcessId(u32 *out, Handle handle);
	Result svc_getThreadId(u32 *out, Handle handle);
	Result svc_setThreadIdealProcessor(Handle handle, u32 processorid);
	Result svc_restartDma(Handle h, void * dst, void const* src, unsigned int size, signed char flag);
	Result svc_kernelSetState(unsigned int Type, unsigned int Param0, unsigned int Param1, unsigned int Param2);
    Result svc_openThread(Handle* thread,Handle process, u32 threadId);
	/**
	* @brief Maps a block of process memory.
	* @param process Handle of the process.
	* @param destAddress Address of the mapped block in the current process.
	* @param srcAddress Address of the mapped block in the source process.
	* @param size Size of the block of the memory to map (truncated to a multiple of 0x1000 bytes).
	*/
	Result svcMapProcessMemoryEx(Handle process, u32 destAddr, u32 srcAddr, u32 size);
#endif
