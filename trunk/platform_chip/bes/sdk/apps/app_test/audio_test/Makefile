
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += \
    -Iservices/audio_bt \
    -Iservices/bone_sensor \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iservices/resources \
    -Iservices/overlay \
    -Iutils/cqueue \
    -Iservices/mcpp \
	-Imultimedia/inc \
    -Imultimedia/inc/speech/inc \
    -Imultimedia/inc/audio/process/anc/include\
    -Imultimedia/inc/audio/process/drc/include \
    -Imultimedia/inc/audio/process/limiter/include \
    -Imultimedia/inc/audio/process/dynamic_eq/include \
    -Iservices/audio_process \
    -Iutils/kfifo \
    -Iapps/anc/inc \
    -Iservices/nv_section/aud_section \
    -Iservices/nv_section/include


ifeq ($(AUDIO_DYNAMIC_BOOST),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_DYNAMIC_BOOST__
endif

ifeq ($(A2DP_KARAOKE),1)
CFLAGS_audio_test_cmd.o += -DA2DP_KARAOKE
endif

ifeq ($(HW_FIR_EQ_PROCESS),1)
CFLAGS_audio_test_cmd.o += -D__HW_FIR_EQ_PROCESS__
CFLAGS_audio_test_debug_tool.o += -D__HW_FIR_EQ_PROCESS__
endif

ifeq ($(HW_IIR_EQ_PROCESS),1)
CFLAGS_audio_test_cmd.o += -D__HW_IIR_EQ_PROCESS__
CFLAGS_audio_test_debug_tool.o += -D__HW_IIR_EQ_PROCESS__
endif

ifeq ($(SW_IIR_EQ_PROCESS),1)
CFLAGS_audio_test_cmd.o += -D__SW_IIR_EQ_PROCESS__
CFLAGS_audio_test_debug_tool.o += -D__SW_IIR_EQ_PROCESS__
endif

ifeq ($(HW_DAC_IIR_EQ_PROCESS),1)
CFLAGS_audio_test_cmd.o += -D__HW_DAC_IIR_EQ_PROCESS__
CFLAGS_audio_test_debug_tool.o += -D__HW_DAC_IIR_EQ_PROCESS__
endif

ifeq ($(AUDIO_DRC),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_DRC__
CFLAGS_audio_test_debug_tool.o += -D__AUDIO_DRC__
endif

ifeq ($(AUDIO_LIMITER),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_LIMITER__
CFLAGS_audio_test_debug_tool.o += -D__AUDIO_LIMITER__
endif

ifeq ($(AUDIO_DYNAMIC_EQ),1)
CFLAGS_audio_test_cmd.o += -D__AUDIO_DYNAMIC_EQ__
CFLAGS_audio_test_debug_tool.o += -D__AUDIO_DYNAMIC_EQ__
endif

ifeq ($(NN_KWS),1)
CFLAGS_audio_test_cmd.o += -DNN_KWS
CFLAGS_audio_kws_test.o += -DNN_KWS_$(NN_KWS_NAME)
ifeq ($(BECO),1)
CFLAGS_audio_kws_test.o += -DUSE_BECO_NN
endif
endif
