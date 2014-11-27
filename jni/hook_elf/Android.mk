
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := hook_elf
LOCAL_SRC_FILES := hook_elf.c
LOCAL_C_INCLUDES += $(JNI_ROOT_PATH)/include
LOCAL_CFLAGS += -Wall -DTAG=\"HSDK:hook_elf\"
LOCAL_STATIC_LIBRARIES += hook_util
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_LDLIBS := -llog
include $(BUILD_STATIC_LIBRARY)
