#android root dir and Toolchain prefix
ANDROID_BASE = /work_tool/prebuilts
SYSROOT = $(ANDROID_BASE)/ndk/9/platforms/android-21/arch-arm/
ANDROID_TOOL_PREFIX = $(ANDROID_BASE)/gcc/linux-x86/arm/arm-linux-androideabi-4.8/bin/arm-linux-androideabi-

# executable file name
RECORDEXE = obj/bin/testdeltarecord_s
PLAYEREXE = obj/bin/testdeltaplayer_s

# local libs
LOCAL_LIBS = -L./libs \
	-lcutils \
	-lutils \
	-lbinder \
	-lmedia  \
	-lmediapolicy

# local include
LOCAL_INCLUDE = -I./include/ -I./stlport/stlport/ -I./bionic/

#local defines
LOCAL_DEFINES =  -Dxxxxx

# all src files
CPP_SRCS = $(wildcard src/*.cpp)
C_SRCS = $(wildcard *.c)
ASM_SRCS = $(wildcard *.S)

# all obj files
CPP_OBJS = $(CPP_SRCS:%.cpp=obj/cpp/%.o)
C_OBJS = $(C_SRCS:%.c=obj/c/%.o)
ASM_OBJS = $(ASM_SRCS:%.s=obj/s/%.o)
OBJS = $(C_OBJS) $(ASM_OBJS) $(CPP_OBJS)
RECORDOBJS = obj/cpp/record_main.o 
PLAYEROBJS = obj/cpp/player_main.o 

CFLAGS_EXTRA += -fno-strict-aliasing -O2 -DNDEBUG  --sysroot=$(SYSROOT)  -mthumb-interwork -mthumb -march=armv7-a \
		-mfloat-abi=hard -mfpu=vfp3 -Wl,--no-warn-mismatch -std=gnu99 -fPIC 
# Float optimazation flags
FLOAT_OPTIMAZATION_FLAGS = -ftree-vectorize -ftree-vectorizer-verbose=6 \
			-mvectorize-with-neon-quad -ffast-math -fsingle-precision-constant

ANDROID_LIBS := \
	-lm -llog \
	$(LOCAL_LIBS)


ANDROID_CFLAGS =   $(FLOAT_OPTIMAZATION_FLAGS)\
	$(CFLAGS_EXTRA) \
	$(LOCAL_INCLUDE) \
	${LOCAL_DEFINES}


ANDROID_LDFLAGS = $(CFLAGS_EXTRA) \

# Default CDT target is all, build the native library
all: $(RECORDEXE) ${PLAYEREXE}
	@echo "Build $(RECORDEXE) Succ!!!"
	@echo "Build $(PLAYEREXE) Succ!!!"

record: $(RECORDEXE)
	@echo "Build $(RECORDEXE) Succ!!!"
player: ${PLAYEREXE}
	@echo "Build $(PLAYEREXE) Succ!!!"

# recordexe
$(RECORDEXE): $(OBJS) ${RECORDOBJS}
	@mkdir -p $(dir $@)
	$(ANDROID_TOOL_PREFIX)g++ -Wl,--gc-sections -pie -fPIE $(ANDROID_LDFLAGS) $^ -o $@ $(ANDROID_LIBS) 

# playerexe
$(PLAYEREXE): $(OBJS) ${PLAYEROBJS}
	@mkdir -p $(dir $@)
	$(ANDROID_TOOL_PREFIX)g++ -Wl,--gc-sections -pie -fPIE $(ANDROID_LDFLAGS) $^ -o $@ $(ANDROID_LIBS) 

# Compile also generating dependencies with -MD
obj/c/%.o: %.c
	@mkdir -p $(dir $@)
	$(ANDROID_TOOL_PREFIX)gcc $(ANDROID_CFLAGS)  -O3 -o $@ -c $<

# Compile also generating dependencies with -MD
obj/s/%.o: %.s 
	@mkdir -p $(dir $@)
	$(ANDROID_TOOL_PREFIX)gcc $(ANDROID_CFLAGS) -O3 -o $@ -c $<

# Compile also generating dependencies with -MD
obj/cpp/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(ANDROID_TOOL_PREFIX)g++ $(ANDROID_CFLAGS) -o $@ -c $<


# Clean by deleting all the objs and the lib
clean:
	rm -fr obj

install:
	adb push -p $(RECORDEXE) /system/bin
	adb push -p $(PLAYEREXE) /system/bin
	adb push -p libs/libmedia.so /system/lib
	adb push -p libs/libmediapolicy.so /system/lib
