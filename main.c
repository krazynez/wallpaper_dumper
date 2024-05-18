#include <pspsdk.h>
#include <pspkernel.h>
#include <psprtc.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdio.h>

#define printf pspDebugScreenPrintf

#include "libpspexploit.h"

PSP_MODULE_INFO("Wallpaper_Dumper", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(20480);

static KernelFunctions _ktbl; KernelFunctions* k_tbl = &_ktbl;

void dump() {
	int k1 = pspSdkSetK1(0);
	int userLevel = pspXploitSetUserLevel(8);
	pspXploitRepairKernel();
	pspXploitScanKernelFunctions(k_tbl);


	k_tbl->IoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, NULL, 0);
	SceUID wallpaper = k_tbl->KernelIOOpen("flash1:/vsh/theme/wallpaper.bmp", PSP_O_RDONLY, 0777);
	if(wallpaper<0) {
		pspDebugScreenClear();
		pspDebugScreenSetTextColor(0x0000FF);
		pspDebugScreenSetXY(2, 2);
		printf("No wallpaper found quitting ....");	
		k_tbl->IoUnassign("flash1:");
		k_tbl->KernelDelayThread(5000000);
		return;
	}

	SceUID photo = k_tbl->KernelIODopen("ms0:/psp/photo");
	if(photo)
		k_tbl->KernelIODclose(photo);
	else 
		k_tbl->KernelIOMkdir(photo, 0777);

	u8 buf[512];
	int read;
	pspDebugScreenClear();
	pspDebugScreenSetXY(2, 2);
	printf("Dumping wallpaper.bmp to ms0:/psp/photo/wallpaper.bmp ....");	
	k_tbl->KernelDelayThread(4000000);
	SceUID fd = k_tbl->KernelIOOpen("ms0:/psp/photo/wallpaper.bmp", PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
	if(fd<0) {
		k_tbl->KernelIOMkdir("ms0:/psp/photo", 0777);
		fd = k_tbl->KernelIOOpen("ms0:/psp/photo/wallpaper.bmp", PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
	}
	while((read = k_tbl->KernelIORead(wallpaper, buf, 512))>0)
		k_tbl->KernelIOWrite(fd, buf, sizeof(buf));

	k_tbl->KernelIOClose(wallpaper);
	k_tbl->KernelIOClose(fd);
	k_tbl->IoUnassign("flash1:");

	pspSdkSetK1(k1);
	pspXploitSetUserLevel(userLevel);
	pspDebugScreenClear();
	pspDebugScreenSetXY(2,2);
	printf("Succesfully dumped wallapaper ...");
	k_tbl->KernelDelayThread(2000000);
	pspDebugScreenClear();
	pspDebugScreenSetXY(2,2);
	printf("Quiting back to XMB ...");
	k_tbl->KernelDelayThread(4000000);

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

