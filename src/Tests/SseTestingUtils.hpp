// Author: David Alexander

#pragma once

#define COMPARE4(sse_fn, simple_fn, i, j)                                                 \
    {                                                                                     \
        float sse_score[4], simple_score[4];                                              \
        __m128 score4 = sse_fn(i, j);                                                     \
        _mm_storeu_ps(sse_score, score4);                                                 \
        for (int ii = 0; ii < 4; ii++) {                                                  \
            simple_score[ii] = simple_fn(i + ii, j);                                      \
            ASSERT_FLOAT_EQ(simple_score[ii], sse_score[ii]) << "(at row " << i + ii      \
                                                             << ", column  " << j << ")"; \
        }                                                                                 \
    }
