
ANDROID_TREE := /Users/ming/Documents/source/Android-CAF
ANDROID_LIBS := /Users/ming/Documents/source/Android-LIB

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := remote_dex
LOCAL_SRC_FILES := remote_dex.c dex.cpp hook_elf.c hook_util.c
LOCAL_CFLAGS += -Wall
LOCAL_C_INCLUDES += $(ANDROID_TREE)/system/core/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/frameworks/native/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/frameworks/base/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/libnativehelper/include
LOCAL_LDLIBS += -L$(ANDROID_LIBS) -llog -landroid_runtime
include $(BUILD_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := injector
LOCAL_SRC_FILES := injector.c shellcode.S hook_elf.c hook_util.c
LOCAL_CFLAGS += -Wall
LOCAL_LDLIBS += -llog
include $(BUILD_EXECUTABLE)
include $(CLEAR_VARS)
LOCAL_MODULE := elf_hook_test
LOCAL_SRC_FILES := test_elf_hook.c hook_elf.c hook_util.c
LOCAL_CFLAGS += -Wall
LOCAL_LDLIBS += -llog
include $(BUILD_EXECUTABLE)
include $(CLEAR_VARS)
LOCAL_MODULE := remote_test
LOCAL_SRC_FILES := test_remote.c
LOCAL_CFLAGS += -Wall
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)
