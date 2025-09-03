cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

ifeq ($(VOC_ENCODE_OPUS),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)opus121/src/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)opus121/src/celt/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)opus121/src/silk/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)opus121/src/silk/fixed/*.c))

ccflags-y += \
	-Iservices/ai_voice/codec/opus121/include \
	-Iservices/ai_voice/codec/opus121/src/celt \
	-Iservices/ai_voice/codec/opus121/src/celt/fixed \
	-Iservices/ai_voice/codec/opus121/src/silk \
	-Iservices/ai_voice/codec/opus121/src/silk/fixed \
	-Iservices/ai_voice/codec/opus121/src/src \
	-Iservices/ai_voice/codec/opus121/src

ccflags-y += -O3 -DARM_MATH_CM4 -DOPUS_HAVE_CONFIG_H -DHAVE_CONFIG_H
endif

obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)sbc/*.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)compression/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

GEN_LIB_NAME := lib_codec
root_dir := $(abspath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))/..)
sinclude $(root_dir)/config/lib.mk

$(GEN_LIB_NAME)-y := $(src_obj)

obj-y += $(GEN_LIB_NAME).a

subdir-ccflags-y +=	\
    -Iservices/audioflinger \
    -Iapps/audioplayers \
    -Iapps/earbuds/conn \
    -Iservices/ai_voice/codec/sbc \
    -Iservices/ai_voice/codec/opus121/include \
	-Imultimedia/inc \
    -Imultimedia/inc/speech/inc \
    -Iservices/bt_app \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(BES_MULTIMEDIA_INCLUDES) \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iutils/lockcqueue \
    -Iutils/retention_ram \
    -Ithirdparty/binaural_lib \
    -Iservices/app_ibrt/inc \
    -Iservices/ibrt_core/inc
