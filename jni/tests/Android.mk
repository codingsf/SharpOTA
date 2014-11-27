
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_hook
LOCAL_SRC_FILES := test_elf_hook.c
LOCAL_C_INCLUDES += $(JNI_ROOT_PATH)/include
LOCAL_CFLAGS += -Wall -DTAG=\"HSDK:$(LOCAL_MODULE)\"
LOCAL_STATIC_LIBRARIES += hook_elf
LOCAL_LDLIBS := -llog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := test_inject
LOCAL_SRC_FILES := test_inject.c
LOCAL_C_INCLUDES += $(JNI_ROOT_PATH)/include
LOCAL_CFLAGS += -Wall -DTAG=\"HSDK:$(LOCAL_MODULE)\"
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := test_java_hook
LOCAL_SRC_FILES := test_java_hook.c
LOCAL_C_INCLUDES += $(JNI_ROOT_PATH)/include
LOCAL_CFLAGS += -Wall -DTAG=\"HSDK:$(LOCAL_MODULE)\"
LOCAL_STATIC_LIBRARIES += hook_java
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)

