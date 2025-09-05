# Project Makefile
# 用法示例：
#   make T=best1306p -j16             # 调用 SDK 编译，产物复制到 project/bin
#   make clean                         # 清理本项目及 SDK 输出
#   make EXTRA_SRCS="/abs/a.c /abs/b.c"  # 参与最终链接的本地源码
#   make EXTRA_INCLUDE="-I/abs/dir"      # 附加头文件路径

# ============= 可配置区域（按需覆盖） =============
# 使用相对路径，便于移植
PROJECT_ROOT := $(CURDIR)
SDK_DIR      := $(PROJECT_ROOT)/trunk/platform_chip/bes/sdk
PROJECT_DIR  := $(PROJECT_ROOT)/project
BUILD_DIR    := $(PROJECT_DIR)/build
BIN_DIR      := $(PROJECT_DIR)/bin

# SDK 目标板（可在命令行覆盖）：make T=best1306p
T ?= best1306p

# 交叉工具链（建议与 SDK 一致），例如：arm-none-eabi-
# 直接默认配置为 arm-none-eabi-，可在命令行覆盖：make CONFIG_CROSS_COMPILE=xxx-
CONFIG_CROSS_COMPILE ?= arm-none-eabi-
# 本地与 SDK 一致使用该前缀
CROSS_COMPILE ?= $(CONFIG_CROSS_COMPILE)

# 本地扩展源码（包含 root_source 目录下的所有 .c 文件）
EXTRA_SRCS ?= \
	$(PROJECT_ROOT)/trunk/root_source/base/root_list.c \
	$(PROJECT_ROOT)/trunk/root_source/algorithm/algorithm_manager.c \
	$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/cmsis_os_queue.c \
	$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/cmsis_os_mutex.c \
	$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/cmsis_os_memery.c \
	$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/cmsis_os_semaphore.c \
	$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/cmsis_os_thread.c \
	$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/cmsis_os_base.c \
	$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/cmsis_misc.c \
	$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/cmsis_rtos_timer.c \
	$(PROJECT_ROOT)/trunk/root_source/base/root_main.c \
	$(PROJECT_ROOT)/trunk/root_source/utils/log_utils.c

# C 标准与通用编译参数
CSTD      ?= gnu11
CFLAGS    ?= -O2 -g -std=$(CSTD)

# 芯片定义 - 根据T参数设置对应的芯片宏
ifeq ($(T),best1306p)
CHIP_DEFINES := -DCHIP_BEST1306P
else ifeq ($(T),best1306)
CHIP_DEFINES := -DCHIP_BEST1306
else ifeq ($(T),best1305)
CHIP_DEFINES := -DCHIP_BEST1305
else ifeq ($(T),best1000)
CHIP_DEFINES := -DCHIP_BEST1000
else ifeq ($(T),best1400)
CHIP_DEFINES := -DCHIP_BEST1400
else ifeq ($(T),best1501)
CHIP_DEFINES := -DCHIP_BEST1501
else ifeq ($(T),best1502p)
CHIP_DEFINES := -DCHIP_BEST1502P
else ifeq ($(T),best1502x)
CHIP_DEFINES := -DCHIP_BEST1502X
else ifeq ($(T),best1503)
CHIP_DEFINES := -DCHIP_BEST1503
else ifeq ($(T),best1600)
CHIP_DEFINES := -DCHIP_BEST1600
else ifeq ($(T),best2000)
CHIP_DEFINES := -DCHIP_BEST2000
else ifeq ($(T),best2001)
CHIP_DEFINES := -DCHIP_BEST2001
else ifeq ($(T),best2003)
CHIP_DEFINES := -DCHIP_BEST2003
else ifeq ($(T),best2300)
CHIP_DEFINES := -DCHIP_BEST2300
else ifeq ($(T),best2300a)
CHIP_DEFINES := -DCHIP_BEST2300A
else ifeq ($(T),best2300p)
CHIP_DEFINES := -DCHIP_BEST2300P
else ifeq ($(T),best3001)
CHIP_DEFINES := -DCHIP_BEST3001
else ifeq ($(T),best3003)
CHIP_DEFINES := -DCHIP_BEST3003
else ifeq ($(T),best3005)
CHIP_DEFINES := -DCHIP_BEST3005
else ifeq ($(T),fpga1000)
CHIP_DEFINES := -DCHIP_FPGA1000
else
CHIP_DEFINES := -DCHIP_$(shell echo $(T) | tr '[:lower:]' '[:upper:]')
endif
# SDK 包含路径配置
SDK_INCLUDE_PATHS := \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/include \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/include/rtos/rtx5 \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/include/rtos/rtx \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/platform/hal \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/platform/cmsis/inc \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/platform/drivers/ana \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/services/nv_section/aud_section \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/services/nv_section/include \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/multimedia/inc \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/multimedia/inc/speech/inc \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/utils/heap \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/utils/libc/inc \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/os_base/include \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/rtos/rtx5 \
	-I$(PROJECT_ROOT)/trunk/platform_chip/bes/sdk/rtos/rtx5/rtx_config \
	-I$(PROJECT_ROOT)/boards \
	-I$(PROJECT_ROOT)/trunk/include

CPPFLAGS  ?= -I$(PROJECT_ROOT) -I$(PROJECT_ROOT)/trunk $(SDK_INCLUDE_PATHS) $(CHIP_DEFINES) $(EXTRA_INCLUDE)

# 与 SDK 对齐的目标架构参数，确保与最终链接兼容
EXTRA_SDK_ARCH_FLAGS := -mthumb -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
EXTRA_CFLAGS ?= -std=gnu99 $(EXTRA_SDK_ARCH_FLAGS)

# 从 CROSS_COMPILE 推导工具
ifeq ($(strip $(CROSS_COMPILE)),)
CC := gcc
AR := ar
else
CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
endif

# ============= 本地扩展库：libextra.a =============
# 仅编译 EXTRA_SRCS，为了保证和 SDK 链接参数兼容，使用绝对路径生成 .o
EXTRA_SRCS_ABS := $(foreach f,$(EXTRA_SRCS),$(if $(wildcard $(f)),$(f),))

# 针对不在 PROJECT_ROOT 树内的绝对路径，放入 build/extra/abs/<escaped_path>.o
define escape_abs_path
$(subst /,_,$(subst :,_,$(1))).o
endef

EXTRA_OBJS := $(foreach f,$(EXTRA_SRCS_ABS),$(if $(filter $(PROJECT_ROOT)/%,$(f)),$(patsubst $(PROJECT_ROOT)/%.c,$(BUILD_DIR)/extra/%.o,$(f)),$(BUILD_DIR)/extra/abs/$(call escape_abs_path,$(f))))

EXTRA_LIB     := $(BUILD_DIR)/lib/libextra.a

.PHONY: all sdk build_extra clean sdk_clean print-vars cursor-config

all: sdk

# ---------- 编译本地扩展为静态库 ----------
build_extra: $(EXTRA_LIB)

$(EXTRA_LIB): $(EXTRA_OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(EXTRA_OBJS)
	@echo "[OK] Built extra lib: $@"

# 在 PROJECT_ROOT 树内的 .c 源文件

$(BUILD_DIR)/extra/%.o: $(PROJECT_ROOT)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

# 对于任意绝对路径的 .c 源文件（树外），放到 abs 目录，文件名转义
$(BUILD_DIR)/extra/abs/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

# ---------- 调用 SDK 编译 ----------
sdk: build_extra | $(BIN_DIR)
	@# 若本地扩展库比 SDK 的 elf 新，则先删除 elf 以强制重新链接
	@set -e; \
	  out_dir="$(BUILD_DIR)/sdk/$(T)"; \
	  base="$(T)"; \
	  elf="$$out_dir/$$base.elf"; \
	  if [ -f "$(EXTRA_LIB)" ] && [ -f "$$elf" ] && [ "$(EXTRA_LIB)" -nt "$$elf" ]; then \
	    echo "[INFO] $(EXTRA_LIB) 更新，移除旧的 ELF 以触发重新链接"; \
	    rm -f "$$elf"; \
	  fi
	$(MAKE) -C $(SDK_DIR) T=$(T) O=$(BUILD_DIR)/sdk CONFIG_CROSS_COMPILE=$(CONFIG_CROSS_COMPILE) \
	  LIB_LDFLAGS="$(EXTRA_LIB)" \
	  KCPPFLAGS+=" -DREL_TRACE_ENABLE" \
	  C++_ONLY_FLAGS+=" -fno-threadsafe-statics" \
	  -j
	@# 复制常见产物（elf/bin/map/lst）到 BIN_DIR
	@set -e; \
	  out_dir="$(BUILD_DIR)/sdk/$(T)"; \
	  base="$(T)"; \
	  for ext in elf bin map lst; do \
	    f="$$out_dir/$$base.$$ext"; \
	    if [ -f "$$f" ]; then cp -f "$$f" "$(BIN_DIR)/"; fi; \
	  done; \
	  echo "[OK] Output copied to $(BIN_DIR)"

$(BIN_DIR):
	@mkdir -p $@

clean: sdk_clean
	@rm -rf "$(BUILD_DIR)" "$(BIN_DIR)"
	@echo "[CLEAN] Removed $(BUILD_DIR) and $(BIN_DIR)"

sdk_clean:
	@$(MAKE) -C $(SDK_DIR) T=$(T) O=$(BUILD_DIR)/sdk allclean >/dev/null 2>&1 || true

print-vars:
	@echo PROJECT_ROOT=$(PROJECT_ROOT)
	@echo SDK_DIR=$(SDK_DIR)
	@echo PROJECT_DIR=$(PROJECT_DIR)
	@echo BUILD_DIR=$(BUILD_DIR)
	@echo BIN_DIR=$(BIN_DIR)
	@echo T=$(T)
	@echo CROSS_COMPILE=$(CROSS_COMPILE)
	@echo CC=$(CC)
	@echo AR=$(AR)
	@echo CFLAGS=$(CFLAGS)
	@echo CPPFLAGS=$(CPPFLAGS)
	@echo EXTRA_SRCS=$(EXTRA_SRCS)
	@echo EXTRA_LIB=$(EXTRA_LIB)

# ---------- 生成 Cursor 配置 ----------
cursor-config:
	@echo "[INFO] 正在生成 Cursor 配置文件..."
	@$(PROJECT_ROOT)/generate_compile_commands.sh
	@echo "[OK] Cursor 配置已更新，请重启 Cursor"

