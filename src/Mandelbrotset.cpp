#include "Mandelbrotset.hpp"

#include <immintrin.h>

#include <cstdint>
#include <filesystem>

#include "Saves.hpp"

extern MandelbrotsetConfiguration mConfig;
extern TileConfiguration tConfig;

__m512d fmod(__m512d a, __m512d b) {
    return _mm512_sub_pd(a, _mm512_mul_pd(_mm512_div_round_pd(a, b, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC), b));
}

// Computes the result for 8 sequential pixels starting at pixel x and y, saves the results inside the output array outSamples.
// x and y and image coordinates
void computeIterationsVector(uint64_t x, uint64_t y, Sample outSamples[8]) noexcept {
    const __m512d m_ones = _mm512_set1_pd(1);
    const __m512d m_startReal = _mm512_set1_pd(mConfig.startReal);
    const __m512d m_endReal = _mm512_set1_pd(mConfig.endReal);
    const __m512d m_startImag = _mm512_set1_pd(mConfig.startImag);
    const __m512d m_endImag = _mm512_set1_pd(mConfig.endImag);
    const __m512d m_imageWidth = _mm512_set1_pd(tConfig.imageWidth - 1);
    const __m512d m_imageHeight = _mm512_set1_pd(tConfig.imageHeight - 1);
    const __m512d m_maxIterations = _mm512_set1_pd(mConfig.maxIterations);
    const __m512d m_bailoutRadius = _mm512_set1_pd(mConfig.bailoutRadius);
    const __m512d m_periodicityPrecision2 = _mm512_set1_pd(mConfig.periodicityPrecision2);
    const __m512d m_periodicitySavePeriod = _mm512_set1_pd(mConfig.periodicitySavePeriod);

    // Position vectors
    __m512d m_x =  _mm512_set_pd(x + 7, x + 6, x + 5, x + 4, x + 3, x + 2, x + 1, x + 0); // Vectors read from right to left when converted to native types
    __m512d m_y = _mm512_set1_pd(                           y                          );

    // Input variables
    // (((1 - (x / m)) * a) + ((x / m) * b)) = ((1 - (x / m)) * a + ((x / m) * b))
    __m512d m_cReal = _mm512_fmadd_pd(_mm512_sub_pd(m_ones, _mm512_div_pd(m_x, m_imageWidth)), m_startReal, _mm512_mul_pd(_mm512_div_pd(m_x, m_imageWidth), m_endReal));
    __m512d m_cImag = _mm512_fmadd_pd(_mm512_sub_pd(m_ones, _mm512_div_pd(m_y, m_imageHeight)), m_startImag, _mm512_mul_pd(_mm512_div_pd(m_y, m_imageHeight), m_endImag));

    // Initialization variables
    __m512d m_zReal = m_cReal;
    __m512d m_zImag = m_cImag;
    __m512d m_zReal2 = _mm512_set1_pd(0);
    __m512d m_zImag2 = _mm512_set1_pd(0);
    __m512d m_finalMagnitude2 = _mm512_set1_pd(0);
    // Periodicity checking
    __m512d m_oReal = _mm512_set1_pd(0);
    __m512d m_oImag = _mm512_set1_pd(0);

    // Compute for pixels (x + 0, y), (x + 1, y), (x + 2, y), (x + 3, y), (x + 4, y), (x + 5, y), (x + 6, y), (x + 7, y)
    __mmask8 m_iterating = 0b11111111;

    __m512d m_k = m_ones;
    for (int64_t k = 1; k < mConfig.maxIterations; k++) {
        m_zReal2 = _mm512_mul_pd(m_zReal, m_zReal);
        m_zImag2 = _mm512_mul_pd(m_zImag, m_zImag);

        if (mConfig.periodicitySavePeriod > 0) {
            __mmask8 m_savePeriod = m_iterating & _mm512_cmplt_pd_mask(fmod(_mm512_sub_pd(m_k, m_ones), m_periodicitySavePeriod), _mm512_set1_pd(0));
                     m_oReal = _mm512_mask_blend_pd(m_savePeriod, m_oReal, m_zReal);
                     m_oImag = _mm512_mask_blend_pd(m_savePeriod, m_oImag, m_zImag);
        }

        __m512d  m_magnitude2 = _mm512_add_pd(m_zReal2, m_zImag2);
                 m_finalMagnitude2 = _mm512_mask_blend_pd(m_iterating, m_finalMagnitude2, m_magnitude2);
        __mmask8 m_mask = _mm512_cmplt_pd_mask(m_magnitude2, m_bailoutRadius);
                 m_iterating = m_iterating & m_mask;

        // Pixels are done iterating
        if (m_iterating == 0b00000000) break;

        // Iterate
        // z_(n+1) = z ^ 2 + c
        // z_(n+1).i = (2 * z.r * z.i) + c.i = (z.r + z.r) * z.i + c.i
        // z_(n+1).r = (z.r * z.r) - (z.i * z.i) + c.r = (z.r + z.i) * (z.r - z.i) + c.r
        __m512d m_zImagNew = _mm512_maskz_fmadd_pd(m_iterating, _mm512_add_pd(m_zReal, m_zReal), m_zImag, m_cImag);
        m_zReal = _mm512_maskz_fmadd_pd(m_iterating, _mm512_add_pd(m_zReal, m_zImag), _mm512_sub_pd(m_zReal, m_zImag), m_cReal);
        m_zImag = m_zImagNew;

        if (mConfig.periodicitySavePeriod > 0) {
            __m512d  m_pReal    = _mm512_sub_pd(m_zReal, m_oReal),
                     m_pImag    = _mm512_sub_pd(m_zImag, m_oImag),
                     m_error    = _mm512_add_pd(_mm512_mul_pd(m_pReal, m_pReal), _mm512_mul_pd(m_pImag, m_pImag));
            __mmask8 m_inPeriod = _mm512_cmplt_pd_mask(m_error, m_periodicityPrecision2);

            // Setting m_k to maxIter when in a period
            m_k = _mm512_mask_blend_pd(m_inPeriod & m_iterating, m_k, m_maxIterations);
            // Set iterating to false when in a period
            m_iterating = m_iterating & ~m_inPeriod;
        }

        // Iterating m_k
        m_k = _mm512_mask_add_pd(m_k, m_iterating, m_k, m_ones);
    }
    __m512i m_iterations = _mm512_cvtpd_epi64(m_k);

    double  *cReal =           reinterpret_cast<double*> (&m_cReal);
    double  *cImag =           reinterpret_cast<double*> (&m_cImag);
    int64_t *iteration =       reinterpret_cast<int64_t*>(&m_iterations);
    double  *finalMagnitude2 = reinterpret_cast<double*> (&m_finalMagnitude2);
    for (int i = 0; i < 8; i++) {
        Sample &sample = outSamples[i];
        sample.cReal = cReal[i];
        sample.cImag = cImag[i];
        sample.iterations = iteration[i];
        sample.finalMagnitude2 = finalMagnitude2[i];
    }
}