//================================================================================================================
// Copyright (c) 2023-present Anne Sakitin (Tianwan Ayana).                                                      =
//                                                                                                               =
// Part of the RK project.                                                                                      =
// Licensed under the F2DLPR License.                                                                            =
//                                                                                                               =
// YOU MAY NOT USE THIS FILE EXCEPT IN COMPLIANCE WITH THE LICENSE.                                              =
// Provided "AS IS", WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,                                               =
// unless required by applicable law or agreed to in writing.                                                    =
//                                                                                                               =
// For details about the F2DLPR License terms and conditions, visit: http://license.fileto.download.             =
//================================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define RKSTDC_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define RKSTDC_INLINE __forceinline
#else
#define RKSTDC_INLINE inline
#endif

RKSTDC_INLINE static void *_rkstdc_memcpy(void *dst, const void *src, size_t size) {
#ifdef __GNUC__
    return __builtin_memcpy(dst, src, size);
#endif
    return memcpy(dst, src, size);
}

RKSTDC_INLINE ssize_t rkstdc_rkrle_encode(const uint8_t *in, size_t in_len, uint8_t **out) {
    if (in == NULL || in_len == 0) return -1;
    size_t in_idx = 0, out_idx = 0;
    do *out = (uint8_t *)malloc(in_len);
    while (*out == NULL);
    while (in_idx < in_len) {
        uint8_t rpt_num = 1;
        while (in_idx + rpt_num < in_len && in[in_idx] == in[in_idx + rpt_num] && rpt_num < UINT8_MAX) ++rpt_num;
        if (rpt_num > 2) {
            if (out_idx + 3 > in_len) {
                do *out = (uint8_t *)realloc(*out, out_idx + 3);
                while (*out == NULL);
            }
            (*out)[out_idx++] = (uint8_t)'Y';
            (*out)[out_idx++] = rpt_num;
            (*out)[out_idx++] = in[in_idx];
            in_idx += rpt_num;
        } else {
            uint8_t nrpt_num = rpt_num;
            while (in_idx + nrpt_num + 2 < in_len && nrpt_num < UINT8_MAX &&
                   ((in[in_idx + nrpt_num] != in[in_idx + nrpt_num + 1] &&
                     in[in_idx + nrpt_num] != in[in_idx + nrpt_num + 2]) ||
                    (in[in_idx + nrpt_num] != in[in_idx + nrpt_num + 1] &&
                     in[in_idx + nrpt_num] == in[in_idx + nrpt_num + 2]) ||
                    (in[in_idx + nrpt_num] == in[in_idx + nrpt_num + 1] &&
                     in[in_idx + nrpt_num] != in[in_idx + nrpt_num + 2])))
                ++nrpt_num;
            if (in_idx + nrpt_num + 2 == in_len && nrpt_num + 2 < UINT8_MAX)
                nrpt_num += 2;
            else if (in_idx + nrpt_num + 1 == in_len && nrpt_num + 1 < UINT8_MAX)
                ++nrpt_num;
            if (out_idx + 2 + nrpt_num > in_len) do
                    *out = (uint8_t *)realloc(*out, out_idx + 2 + nrpt_num);
                while (*out == NULL);
            (*out)[out_idx++] = (uint8_t)'N';
            (*out)[out_idx++] = nrpt_num;
            _rkstdc_memcpy(*out + out_idx, in + in_idx, nrpt_num);
            out_idx += nrpt_num;
            in_idx += nrpt_num;
        }
    }
    do *out = (uint8_t *)realloc(*out, out_idx);
    while (*out == NULL);
    return out_idx;
}

RKSTDC_INLINE ssize_t rkstdc_rkrle_decode(const uint8_t *in, size_t in_len, uint8_t **out) {
    if (in == NULL || in_len == 0) return -1;
    size_t in_idx = 0, out_idx = 0;
    do *out = (uint8_t *)malloc(in_len);
    while (*out == NULL);
    while (in_idx < in_len) {
        char    method = (char)in[in_idx++];
        uint8_t length = in[in_idx++];
        if (out_idx + length > in_len) do
                *out = (uint8_t *)realloc(*out, out_idx + length);
            while (*out == NULL);
        switch (method) {
            case 'y':
            case 'Y': {
                uint8_t value = in[in_idx++];
                for (uint8_t nrpt_idx = 0; nrpt_idx < length; ++nrpt_idx) (*out)[out_idx++] = value;
                break;
            }
            case 'n':
            case 'N':
                _rkstdc_memcpy(*out + out_idx, in + in_idx, length);
                in_idx += length;
                out_idx += length;
                break;
            default:
                return -1;
        }
    }
    do *out = (uint8_t *)realloc(*out, out_idx);
    while (*out == NULL);
    return out_idx;
}

#ifdef __cplusplus
}
#endif
