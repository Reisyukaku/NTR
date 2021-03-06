#ifndef FUNC_H
#define FUNC_H


void setCpuClockLock(int v);
void lockCpuClock(void);
void clearDisp(u32 rgb);
void setExitFlag(void);
void checkExitFlag(void);

void dumpRemoteProcess(u32 pid, u8* fileName, u32 startAdd);
void dumpCode(u32 base, u32 size, u8* fileName);
void processManager() ;
u32 protectMemory(void* addr, u32 size);
u32 protectRemoteMemory(Handle hProcess, void* addr, u32 size);
u32 copyRemoteMemory(Handle hDst, void* ptrDst, Handle hSrc, void* ptrSrc, u32 size);
u32 getProcessInfo(u32 pid, u8* pname, u32 tid[], u32* kpobj);
u32 mapRemoteMemory(Handle hProcess, u32 addr, u32 size);
u32 controlMemoryInSysRegion(u32* outAddr, u32 addr0, u32 addr1, u32 size, u32 op, u32 perm);
u32 mapRemoteMemoryInSysRegion(Handle hProcess, u32 addr, u32 size);
u32 writeRemoteProcessMemory(int pid, u32 addr, u32 size, u32* buf);

u32 getProcessTIDByHandle(u32 hProcess, u32 tid[]);
u32 getCurrentProcessId(void);
u32 getCurrentProcessHandle(void);


void debounceKey(void);
void updateScreen(void);
s32 showMenu(u8* title, u32 entryCount, u8* captions[]);
s32 showMenuEx(u8* title, u32 entryCount, u8* captions[], u8* descriptions[], u32 selectOn);
void acquireVideo() ;
void releaseVideo(void);
u32 waitKey(void);
u32 getKey(void);
u32 initDirectScreenAccess(void);
void delayUi(void);
void drawString(const char* str, int x, int y, u32 rgbTxt);
void mdelay(u32 m);
void print(const char* s, int x, int y, u32 rgbTxt);

void ntrToolsMain(void);
void do_screen_shoot(void);

void setThemeCol(u32 col);

void mystrcat(char* a, char* b);
void myitoa(u32 a, char* b);

void kernelCallback(u32 msr);
void kmemcpy(void* dst, void* src, u32 size);
void kSetCurrentKProcess(u32 ptr);
u32 kGetCurrentKProcess(void);
u32 kGetKProcessByHandle(u32 handle);
u32 kSwapProcessPid(u32 kProcess, u32 newPid);
void kRemotePlayCallback(void);

void initFromInjectPM(void);
void initFromInjectGame(void);
void plgInitFromInjectHOME(void);


void nsInitBreakPoint(int id, u32 addr, int type);
void nsHandleQueryHandle(void);
void nsHandleBreakPoint(void);
void nsHandleReload(void);
void nsHandleListProcess(void);
void nsHandleMemLayout(void);
void nsHandleWriteMem(void);
void nsHandleReadMem(void);
u32 nsGetPCToAttachProcess(u32 hProcess);
void nsHandleListThread(void);
void nsHandleAttachProcess(void);
void nsPrintRegs(u32* regs);
void nsUpdateDebugStatus(void);
void nsHandlePacket(void);
void nsMainLoop(void);
void nsThreadStart(void);
void nsInitDebug(void);
void startDebugger(void);

void backdoorHandler(void);

typedef int(*drawStringTypeDef)  (const char* str, int x, int y, u32 rgbTxt, u32 rgbBg);
typedef char* (*translateTypeDef) (char* str);
char* plgTranslate(char* origText);

int plgTryUpdateConfig(void);
u32 plgRequestTempBuffer(u32 size);

extern Handle fsUserHandle;
extern FS_archive sdmcArchive;

extern u32 allowDirectScreenAccess;

extern u32 arm11BinStart;
extern u32 arm11BinSize;
extern u32 arm11BinProcess;

#define MAX_PLUGIN_COUNT 32

typedef struct _PLGLOADER_INFO {
	u32 plgCount;
	u32 plgBufferPtr[MAX_PLUGIN_COUNT];
	u32 plgSize[MAX_PLUGIN_COUNT];
	u32 arm11BinStart;
	u32 arm11BinSize;
	u32 tid[2];
	u32 gamePluginPid;
	u32 gamePluginMenuAddr;
	u32 currentLanguage;
	u32 nightShiftLevel;
} PLGLOADER_INFO;

#define MAX_GAME_PLUGIN_MENU_ENTRY 64
typedef struct _GAME_PLUGIN_MENU {
	u8 state[MAX_GAME_PLUGIN_MENU_ENTRY];
	u16 offsetInBuffer[MAX_GAME_PLUGIN_MENU_ENTRY];
	u16 bufOffset, count;
	u8 buf[3000];
} GAME_PLUGIN_MENU;

#define CURRENT_PROCESS_HANDLE	0xffff8001


#define REG(x)   (*(volatile u32*)(x))
#define REG8(x)  (*(volatile  u8*)(x))
#define REG16(x) (*(volatile u16*)(x))
#define SW(addr, data)  *(u32*)(addr) = data



#endif