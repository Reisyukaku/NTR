#include "global.h"

u32 kernelArgs[32];
u32 KProcessHandleDataOffset;
u32 KProcessPIDOffset;
u32 KProcessCodesetOffset;

int cpuClockLockValue = -1;

void* currentBackdoorHandler = backdoorHandler;

u32 keRefHandle(u32 pHandleTable, u32 handle) {
	u32 handleLow = handle & 0x7fff;
	u32 ptr = *(u32*)(*((u32*)pHandleTable) + (handleLow * 8) + 4);
	return ptr;
}

u32* translateAddress(u32 addr) {
	if (addr < 0x1ff00000) {
		return addr - 0x1f3f8000 + 0xfffdc000;
	}
	return (u32*)(addr - 0x1ff00000 + 0xdff00000);
}

void set_kmmu_rw(int cpu, u32 addr, u32 size){
	int i, j;
	u32 mmu_p;
	u32 p1, p2;
	u32 v1, v2;
	u32 end;

	if (cpu == 0){
		mmu_p = 0x1fff8000;
	}
	if (cpu == 1) {
		mmu_p = 0x1fffc000;
	}
	if (cpu == 2) {
		mmu_p = 0x1f3f8000;
	}

	end = addr + size;

	v1 = 0x20000000;
	for (i = 512; i<4096; i++){
		p1 = *translateAddress(mmu_p + i * 4);
		if ((p1 & 3) == 2){
			if (v1 >= addr && v1<end){
				p1 &= 0xffff73ff;
				p1 |= 0x00000c00;
				*translateAddress(mmu_p + i * 4) = p1;
			}
		}
		else if ((p1 & 3) == 1){
			p1 &= 0xfffffc00;
			for (j = 0; j<256; j++){
				v2 = v1 + j * 0x1000;
				if ((v2 >= addr) && (v2<end)){
					p2 = *translateAddress(p1 + j * 4);
					if ((p2 & 3) == 1){
						p2 &= 0xffff7dcf;
						p2 |= 0x00000030;
						*translateAddress(p1 + j * 4) = p2;
					}
					else if ((p2 & 3)>1){
						p2 &= 0xfffffdce;
						p2 |= 0x00000030;
						*translateAddress(p1 + j * 4) = p2;
					}
				}
			}
			
		}
		v1 += 0x00100000;
	}
}

void set_remoteplay_mmu(u32 addr, u32 size) {
	int i, j;
	u32 mmu_p;
	u32 p1, p2;
	u32 v1, v2;
	u32 end;
	
	mmu_p = 0x1f3f8000;
	end = addr + size;

	v1 = 0x20000000;
	for (i = 0; i<4096; i++){
		p1 = *translateAddress(mmu_p + i * 4);
		if ((p1 & 3) == 2){
			if (v1 >= addr && v1<end){
				p1 &= 0xffff73ff;
				p1 |= 0x00000c00;
				p1 &= 0xfffffff3;
				*translateAddress(mmu_p + i * 4) = p1;
			}
		}
		v1 += 0x00100000;
	}
}

void keDoKernelHax() {
	//set mmu
	set_kmmu_rw(0, ntrConfig->KMMUHaxAddr, ntrConfig->KMMUHaxSize);
	set_kmmu_rw(1, ntrConfig->KMMUHaxAddr, ntrConfig->KMMUHaxSize);
	if (ntrConfig->isNew3DS) {
		set_kmmu_rw(2, ntrConfig->KMMUHaxAddr, ntrConfig->KMMUHaxSize);
	}

	//patch controlmemory to disable address boundary check
	*(u32*)(ntrConfig->ControlMemoryPatchAddr1) = 0;
	*(u32*)(ntrConfig->ControlMemoryPatchAddr2) = 0;

    //Set new backdoor to disable IRQ/FIQ interupts before hand
	if (ntrConfig->KernelFreeSpaceAddr_Optional) {
		u32* addr = ntrConfig->KernelFreeSpaceAddr_Optional;
		addr[0] = 0xe10f0000;   //mrs r0, CPSR
		addr[1] = 0xe38000c0;   //orr r0, r0, #0xC0
		addr[2] = 0xe129f000;   //msr CPSR_fc, r0
		rtGenerateJumpCode(kernelCallback, &addr[3]);
		currentBackdoorHandler = (void*)(addr);
	}

    //Invalidate cache
	InvalidateEntireInstructionCache();
	InvalidateEntireDataCache();
}

void kernelCallback(u32 msr) {
	typedef (*keRefHandleType)(u32, u32);
	
	u32 i = 0;
    switch(kernelArgs[0]){
        case 1:
        {
            u32 size = kernelArgs[3];
            u32 dst = kernelArgs[1];
            u32 src = kernelArgs[2];
            for (i = 0; i < size; i += 4)
                *(vu32*)(dst + i) = *(vu32*)(src + i);
                
            break;
        }
        case 2:
        {
            //refKProcessByHandle
            u32 hProcess = kernelArgs[1];
            u32 kProcess = keRefHandle(*(u32*)0xFFFF9004 + KProcessHandleDataOffset, hProcess);
            kernelArgs[1] = kProcess;
            break;
        }
        case 3:
        {
            //getCurrentKProcess
            kernelArgs[1] = *(u32*)0xFFFF9004;
            break;
        }
        case 4:
        {
            //setCurrentKProcess
            *(u32*)0xFFFF9004 = kernelArgs[1];
            break;
        }
        case 5:
        {
            //swapPid
            u32 kProcess = kernelArgs[1];
            u32 newPid = kernelArgs[2];
            kernelArgs[2] = *(u32*)(kProcess + KProcessPIDOffset);
            *(u32*)(kProcess + KProcessPIDOffset) = newPid;
            break;
        }
        case 6:
        {
            keDoKernelHax();
            break;
        }
        case 7:
        {
            remotePlayKernelCallback();
            break;
        }
    }
}

/*
*   Functions to run in kernel mode
*/
u32 kSwapProcessPid(u32 kProcess, u32 newPid) {
	kernelArgs[0] = 5;
	kernelArgs[1] = kProcess;
	kernelArgs[2] = newPid;
	svc_backDoor(currentBackdoorHandler);
	return kernelArgs[2];
}

void kSetCurrentKProcess(u32 ptr) {
	kernelArgs[0] = 4;
	kernelArgs[1] = ptr;
	svc_backDoor(currentBackdoorHandler);
}

u32 kGetCurrentKProcess() {
	kernelArgs[0] = 3;
	svc_backDoor(currentBackdoorHandler);
	return kernelArgs[1];
}

u32 kGetKProcessByHandle(u32 handle) {
	kernelArgs[0] = 2;
	kernelArgs[1] = handle;
	svc_backDoor(currentBackdoorHandler);
	return kernelArgs[1];
}

void kmemcpy(void* dst, void* src, u32 size) {
	kernelArgs[0] = 1;
	kernelArgs[1] = (u32)dst;
	kernelArgs[2] = (u32)src;
	kernelArgs[3] = (u32)size;
	svc_backDoor(currentBackdoorHandler);
}

void kDoKernelHax() {
	kernelArgs[0] = 6;
	svc_backDoor(currentBackdoorHandler);
}

void kRemotePlayCallback() {
	kernelArgs[0] = 7;
	svc_backDoor(currentBackdoorHandler);
}

//----------------------

void lockCpuClock() {
	if (cpuClockLockValue == -1) return;
	svc_kernelSetState(10, cpuClockLockValue, 0, 0);
}

void setCpuClockLock(int v) {
	cpuClockLockValue = v;
}