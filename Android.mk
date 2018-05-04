LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	record_main.cpp \

LOCAL_SHARED_LIBRARIES := \
	libbinder	\
	libutils	\
	libcutils	\
	libmedia	\
	liblog		\

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := testdeltarecord_s

ifeq ($(AV_BUILD_WITH_DELTAII_MODE),true)
LOCAL_C_INCLUDES :=    \
        $(TOP)/vendor/libratone/sdk/luci/include/ \
    $(call include-path-for, audio-utils)
endif

ifeq ($(AV_BUILD_WITH_DELTAI_MODE),true)
LOCAL_C_INCLUDES :=    \
        $(TOP)/vendor/libratone/luci/include/ \
    $(call include-path-for, audio-utils)
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/
include $(BUILD_EXECUTABLE)


