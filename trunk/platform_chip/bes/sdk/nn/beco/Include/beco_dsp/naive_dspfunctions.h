/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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

#ifndef _NAIVE_DSPFUNCTIONS_H
#define _NAIVE_DSPFUNCTIONS_H

#include "arm_math.h"

void naive_mat_mult_i8xi8_i16(const int8_t *A, const int8_t *B, int16_t *C, int M, int N, int K)
{
    int i,j,p;
    int out;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            // Calculate the dot product between A.column and B.column:
            out = 0;
            for (p = 0; p < K; p++) {
                out += A[i+p*M] * B[p*N+j]; // A[M][K] * B[K][N]
            }
            C[i*N+j] = (int16_t)__SSAT(out, 16);
        }
    }
}

void naive_mat_mult_u8xu8_u16(const uint8_t *A, const uint8_t *B, uint16_t *C, int M, int N, int K)
{
    int i,j,p;
    int out;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            // Calculate the dot product between A.column and B.column:
            out = 0;
            for (p = 0; p < K; p++) {
                out += A[i+p*M] * B[p*N+j]; // A[M][K] * B[K][N]
            }
            C[i*N+j] = (uint16_t)__USAT(out, 16);
        }
    }
}

#endif
