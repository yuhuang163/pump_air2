cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_cpp:.cpp=.o)

subdir-ccflags-y += -DMODULE=SERVICE

ccflags-y += \
    -Iplatform/drivers/bt \
    -Ibthost/adapter/inc/bt/common
