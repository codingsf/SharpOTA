
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := hook_java
LOCAL_SRC_FILES := hook_java.cpp hook_dvm.cpp hook_art.cpp
LOCAL_C_INCLUDES += $(JNI_ROOT_PATH)/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/system/core/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/frameworks/native/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/frameworks/base/include
LOCAL_C_INCLUDES += $(ANDROID_TREE)/libnativehelper/include
LOCAL_CFLAGS += -Wall -DTAG=\"HSDK:$(LOCAL_MODULE)\"
LOCAL_STATIC_LIBRARIES += hook_java
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_LDLIBS := -L$(ANDROID_LIBS) -llog -lcutils -landroid_runtime -ldvm
include $(BUILD_STATIC_LIBRARY)

