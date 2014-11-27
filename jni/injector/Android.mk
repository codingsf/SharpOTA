
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := injector
LOCAL_SRC_FILES := injector.c shellcode.S
LOCAL_C_INCLUDES += $(JNI_ROOT_PATH)/include
LOCAL_CFLAGS += -Wall -DTAG=\"HSDK:injector\"
LOCAL_STATIC_LIBRARIES += hook_util
LOCAL_LDLIBS += -llog
include $(BUILD_EXECUTABLE)

