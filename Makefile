TARGET = eboot
OBJS = main.o

UNAME := $(shell uname)

EXTRA_TARGETS = EBOOT.PBP

PSP_EBOOT_TITLE = Wallaper Dumper

all:
ifeq ($(UNAME), Linux)
	WINEPREFIX="$(shell pwd)/prefix/" wine bin/prxEncrypter.exe $(TARGET).prx
else
	$(shell pwd)\bin\prxEncrypter $(TARGET).prx
endif
	#pack-pbp $(EXTRA_TARGETS) PARAM.SFO icon0.png NULL pic0.png NULL NULL data.psp NULL
	pack-pbp $(EXTRA_TARGETS) PARAM.SFO NULL NULL NULL NULL NULL data.psp NULL

INCDIR = ./inc
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ./lib 
#LIBS = 
LDFLAGS = -L.

LIBS = -lpspexploit -lpsprtc -lpspdebug


PSP_FW_VERSION = 280

BUILD_PRX = 1


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

release: all
	@mkdir -p PSP/GAME/Wallpaper_Dumper
	@cp EBOOT.PBP PSP/GAME/Wallpaper_Dumper/
	@zip -r Wallpaper_Dumper.zip PSP/

clean:
	@rm -rf $(TARGET)* prefix data.psp PSP *.zip *.o *.PBP *.SFO
	@echo Cleaned


