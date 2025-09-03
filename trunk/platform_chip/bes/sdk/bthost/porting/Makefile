cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/buf/*.*))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/os/*.*))

obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)

subdir-ccflags-y += -DMODULE=SERVICE

ccflags-y += \
	-Iinclude/cmsis_dsp \
    -Iutils/heap \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	-Ibthost/porting/api
