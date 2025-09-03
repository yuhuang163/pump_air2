/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __COBUF_H__
#define __COBUF_H__
#include "bt_callback_func.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define cobuf_malloc(size) bt_callback_stack_buf_malloc((size), (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)
#define cobuf_free(buf) bt_callback_stack_buf_free((unsigned char *)buf, (uint32_t)(uintptr_t)__builtin_return_address(0), __LINE__)
#define cobuf_check_buf_ava(size) bt_callback_stack_buf_check_buf_ava(size)
#define cobuf_count_ava_buf(size) bt_callback_stack_buf_count_ava_buf(size)
#define cobuf_print_statistic() bt_callback_stack_buf_print_usage()

#if defined(__cplusplus)
}
#endif
#endif /* __COBUF_H__ */
