LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
#2 自定义了一个all_cpp_files_recursively函数，递归遍历返回给定目录下所有C++源文件。
all_cpp_files_recursively = \
 $(eval src_files = $(wildcard $1/*.cpp)) \
 $(eval src_files = $(src_files:$(LOCAL_PATH)/%=%))$(src_files) \
 $(eval item_all = $(wildcard $1/*)) \
 $(foreach item, $(item_all) $(),\
  $(eval item := $(item:%.cpp=%)) \
  $(call all_cpp_files_recursively, $(item))\
 )
 
#3 自定义了一个all_c_files_recursively 函数，递归遍历返回给定目录下所有C源文件。
all_cc_files_recursively = \
 $(eval src_files = $(wildcard $1/*.cc)) \
 $(eval src_files = $(src_files:$(LOCAL_PATH)/%=%))$(src_files) \
 $(eval item_all = $(wildcard $1/*)) \
 $(foreach item, $(item_all) $(),\
  $(eval item := $(item:%.cc=%)) \
  $(call all_c_files_recursively, $(item))\
 )

LOCAL_MODULE := FaceTracker
LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/../include
LOCAL_LDLIBS := -llog

SRC_PATH := $(LOCAL_PATH)/../src
LOCAL_SRC_FILES := $(SRC_PATH)/objdetect/cascadedetect.cpp $(SRC_PATH)/objdetect/haar.cpp
LOCAL_SRC_FILES += $(call all_cpp_files_recursively,$(SRC_PATH)/core)
LOCAL_SRC_FILES += $(call all_cpp_files_recursively,$(SRC_PATH)/imgproc)
LOCAL_SRC_FILES += $(call all_cc_files_recursively,$(SRC_PATH)/lib)
include $(BUILD_SHARED_LIBRARY)