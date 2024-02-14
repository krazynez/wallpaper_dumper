#include <pspsdk.h>
#include <pspkernel.h>
#include <psprtc.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdio.h>

#define printf pspDebugScreenPrintf

#include "libpspexploit.h"

PSP_MODULE_INFO("Wallpaper_Dumper", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(4096);

static KernelFunctions _ktbl; KernelFunctions* k_tbl = &_ktbl;

void dump() {
	int k1 = pspSdkSetK1(0);
	int userLevel = pspXploitSetUserLevel(8);
	pspXploitRepairKernel();
	pspXploitScanKernelFunctions(k_tbl);


	sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, NULL, 0);
	SceUID wallpaper = sceIoOpen("flash1:/vsh/theme/wallpaper.bmp", PSP_O_RDONLY, 0777);
	if(wallpaper<0) {
		pspDebugScreenClear();
		pspDebugScreenSetTextColor(0x0000FF);
		pspDebugScreenSetXY(2, 2);
		printf("No wallpaper found quitting ....");	
		sceIoUnassign("flash1:");
		sceKernelDelayThread(5000000);
		return;
	}

	SceUID photo = sceIoDopen("ms0:/psp/photo");
	if(photo)
		sceIoDclose(photo);
	else 
		sceIoMkdir(photo, 0777);

	u8 buf[512];
	int read;
	pspDebugScreenClear();
	pspDebugScreenSetXY(2, 2);
	printf("Dumping wallpaper.bmp to ms0:/psp/photo/wallpaper.bmp ....");	
	sceKernelDelayThread(4000000);
	SceUID fd = sceIoOpen("ms0:/psp/photo/wallpaper.bmp", PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
	while((read = sceIoRead(wallpaper, buf, 512))>0)
		sceIoWrite(fd, buf, sizeof(buf));

	sceIoClose(wallpaper);
	sceIoClose(fd);
	sceIoUnassign("flash1:");

	pspSdkSetK1(k1);
	pspXploitSetUserLevel(userLevel);
	pspDebugScreenClear();
	pspDebugScreenSetXY(2,2);
	printf("Succesfully dumped wallapaper ...");
	sceKernelDelayThread(2000000);
	pspDebugScreenClear();
	pspDebugScreenSetXY(2,2);
	printf("Quiting back to XMB ...");
	sceKernelDelayThread(4000000);

}

int main(int argc, char *args[]) {

	SceCtrlData pad;
	pspDebugScreenInit();
	pspDebugScreenSetTextColor(0x00BFFF);
	pspDebugScreenSetXY(2,2);
	printf("Press X to dump");
	for(;;) {
		sceCtrlReadBufferPositive(&pad, 1);
		if((pad.Buttons & PSP_CTRL_CROSS) == PSP_CTRL_CROSS) {
			int res = pspXploitInitKernelExploit();
			if(res == 0) {
				res = pspXploitDoKernelExploit();
				if (res == 0)
					pspXploitExecuteKernel(dump);
			}
			else {
				pspDebugScreenClear();
				printf("ERR!");
				sceKernelDelayThread(500000);
			}
			sceKernelExitGame();
		}
	}


			

	return 0;
}

