
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := remotejarldr
LOCAL_SRC_FILES := main.c dex.cpp
LOCAL_C_INCLUDES += $(JNI_ROOT_PATH)/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/system/core/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/frameworks/native/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/frameworks/base/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/libnativehelper/include
LOCAL_CFLAGS += -Wall -DTAG=\"HSDK:remotejarldr\"
LOCAL_STATIC_LIBRARIES += hook_elf
LOCAL_LDLIBS := -L$(ANDROID_LIBS) -llog -landroid_runtime
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
include $(BUILD_SHARED_LIBRARY)

