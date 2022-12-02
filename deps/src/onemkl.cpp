#include "onemkl.h"
#include "sycl.hpp"

#include <oneapi/mkl.hpp>

// This is a workaround to flush MKL submissions into Level-zero queue, 
// using unspecified but guaranteed behavior of intel-sycl runtime. 
// Once SYCL standard committee approves sycl::queue::flush() we will change the macro to use the same 
#define __FORCE_MKL_FLUSH__(cmd) \
            get_native<sycl::backend::ext_oneapi_level_zero>(cmd)

// gemm

// https://spec.oneapi.io/versions/1.0-rev-1/elements/oneMKL/source/domains/blas/gemm.html

oneapi::mkl::transpose convert(onemklTranspose val) {
    switch (val) {
    case ONEMKL_TRANSPOSE_NONTRANS:
        return oneapi::mkl::transpose::nontrans;
    case ONEMKL_TRANSPOSE_TRANS:
        return oneapi::mkl::transpose::trans;
    case ONEMLK_TRANSPOSE_CONJTRANS:
        return oneapi::mkl::transpose::conjtrans;
    }
}

oneapi::mkl::side convert(onemklSide val) {
    switch (val) {
    case ONEMKL_SIDE_LEFT:
        return oneapi::mkl::side::left;
    case ONEMKL_SIDE_RIGHT:
        return oneapi::mkl::side::right;
    }
}

oneapi::mkl::uplo convert(onemklUplo val) {
    switch(val) {
    case ONEMKL_UPLO_UPPER:
        return oneapi::mkl::uplo::upper;
    case ONEMKL_UPLO_LOWER:
        return oneapi::mkl::uplo::lower;
    }
}

oneapi::mkl::diag convert(onemklDiag val) {
    switch(val) {
        case ONEMKL_DIAG_NONUNIT:
            return oneapi::mkl::diag::nonunit;
        case ONEMKL_DIAG_UNIT:
            return oneapi::mkl::diag::unit;
    }
}

extern "C" int onemklHgemm(syclQueue_t device_queue, onemklTranspose transA,
                           onemklTranspose transB, int64_t m, int64_t n,
                           int64_t k, sycl::half alpha, const sycl::half *A, int64_t lda,
                           const sycl::half *B, int64_t ldb, sycl::half beta, sycl::half *C,
                           int64_t ldc) {
    oneapi::mkl::blas::column_major::gemm(device_queue->val, convert(transA),
                                          convert(transB), m, n, k, alpha, A,
                                          lda, B, ldb, beta, C, ldc);
    return 0;
}

extern "C" int onemklSgemm(syclQueue_t device_queue, onemklTranspose transA,
                           onemklTranspose transB, int64_t m, int64_t n,
                           int64_t k, float alpha, const float *A, int64_t lda,
                           const float *B, int64_t ldb, float beta, float *C,
                           int64_t ldc) {
    oneapi::mkl::blas::column_major::gemm(device_queue->val, convert(transA),
                                          convert(transB), m, n, k, alpha, A,
                                          lda, B, ldb, beta, C, ldc);
    return 0;
}

extern "C" int onemklDgemm(syclQueue_t device_queue, onemklTranspose transA,
                           onemklTranspose transB, int64_t m, int64_t n,
                           int64_t k, double alpha, const double *A,
                           int64_t lda, const double *B, int64_t ldb,
                           double beta, double *C, int64_t ldc) {
    oneapi::mkl::blas::column_major::gemm(device_queue->val, convert(transA),
                                          convert(transB), m, n, k, alpha, A,
                                          lda, B, ldb, beta, C, ldc);
    return 0;
}

extern "C" int onemklCgemm(syclQueue_t device_queue, onemklTranspose transA,
                           onemklTranspose transB, int64_t m, int64_t n,
                           int64_t k, float _Complex alpha,
                           const float _Complex *A, int64_t lda,
                           const float _Complex *B, int64_t ldb,
                           float _Complex beta, float _Complex *C,
                           int64_t ldc) {
    oneapi::mkl::blas::column_major::gemm(
        device_queue->val, convert(transA), convert(transB), m, n, k, alpha,
        reinterpret_cast<const std::complex<float> *>(A), lda,
        reinterpret_cast<const std::complex<float> *>(B), ldb, beta,
        reinterpret_cast<std::complex<float> *>(C), ldc);
    return 0;
}

extern "C" int onemklZgemm(syclQueue_t device_queue, onemklTranspose transA,
                           onemklTranspose transB, int64_t m, int64_t n,
                           int64_t k, double _Complex alpha,
                           const double _Complex *A, int64_t lda,
                           const double _Complex *B, int64_t ldb,
                           double _Complex beta, double _Complex *C,
                           int64_t ldc) {
    oneapi::mkl::blas::column_major::gemm(
        device_queue->val, convert(transA), convert(transB), m, n, k, alpha,
        reinterpret_cast<const std::complex<double> *>(A), lda,
        reinterpret_cast<const std::complex<double> *>(B), ldb, beta,
        reinterpret_cast<std::complex<double> *>(C), ldc);
    return 0;
}

extern "C" void onemklSasum(syclQueue_t device_queue, int64_t n, 
                            const float *x, int64_t incx,
                            float *result) {
    auto status = oneapi::mkl::blas::column_major::asum(device_queue->val, n, x,
                                                        incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDasum(syclQueue_t device_queue, int64_t n,
                            const double *x, int64_t incx,
                            double *result) {
    auto status = oneapi::mkl::blas::column_major::asum(device_queue->val, n, x,
                                                        incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCasum(syclQueue_t device_queue, int64_t n,
                            const float _Complex *x, int64_t incx,
                            float *result) {
    auto status = oneapi::mkl::blas::column_major::asum(device_queue->val, n, 
                                        reinterpret_cast<const std::complex<float> *>(x),
                                        incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZasum(syclQueue_t device_queue, int64_t n,
                            const double _Complex *x, int64_t incx,
                            double *result) {
    auto status = oneapi::mkl::blas::column_major::asum(device_queue->val, n, 
                                        reinterpret_cast<const std::complex<double> *>(x),
                                        incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklSaxpy(syclQueue_t device_queue, int64_t n, float alpha,
                            const float *x, std::int64_t incx, float *y, int64_t incy) {
    auto status = oneapi::mkl::blas::column_major::axpy(device_queue->val, n, alpha, x,
                                                incx, y, incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDaxpy(syclQueue_t device_queue, int64_t n, double alpha, 
                            const double *x, std::int64_t incx, double *y, int64_t incy) {
    auto status = oneapi::mkl::blas::column_major::axpy(device_queue->val, n, alpha, x,
                                                incx, y, incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCaxpy(syclQueue_t device_queue, int64_t n, float _Complex alpha,
                        const float _Complex *x, std::int64_t incx, float _Complex *y, int64_t incy) {
    auto status = oneapi::mkl::blas::column_major::axpy(device_queue->val, n, alpha,
                            reinterpret_cast<const std::complex<float> *>(x), incx,
                            reinterpret_cast<std::complex<float> *>(y), incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZaxpy(syclQueue_t device_queue, int64_t n, double _Complex alpha,
                        const double _Complex *x, std::int64_t incx, double _Complex *y, int64_t incy) {
    auto status = oneapi::mkl::blas::column_major::axpy(device_queue->val, n, alpha,
                            reinterpret_cast<const std::complex<double> *>(x), incx,
                            reinterpret_cast<std::complex<double> *>(y), incy);
    __FORCE_MKL_FLUSH__(status);
}
// Support Level-1: SCAL primitive
extern "C" void onemklDscal(syclQueue_t device_queue, int64_t n, double alpha,
                            double *x, int64_t incx) {
    auto status = oneapi::mkl::blas::column_major::scal(device_queue->val, n, alpha,
                                                    x, incx);
    __FORCE_MKL_FLUSH__(status);

}

extern "C" void onemklSscal(syclQueue_t device_queue, int64_t n, float alpha,
                            float *x, int64_t incx) {
    auto status = oneapi::mkl::blas::column_major::scal(device_queue->val, n, alpha,
                                                         x, incx);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCscal(syclQueue_t device_queue, int64_t n,
                            float _Complex alpha, float _Complex *x,
                            int64_t incx) {
    auto status = oneapi::mkl::blas::column_major::scal(device_queue->val, n,
                                        static_cast<std::complex<float> >(alpha),
                                        reinterpret_cast<std::complex<float> *>(x),incx);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCsscal(syclQueue_t device_queue, int64_t n,
                            float alpha, float _Complex *x,
                            int64_t incx) {
    auto status = oneapi::mkl::blas::column_major::scal(device_queue->val, n, alpha,
                                        reinterpret_cast<std::complex<float> *>(x),incx);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZscal(syclQueue_t device_queue, int64_t n,
                            double _Complex alpha, double _Complex *x,
                            int64_t incx) {
    auto status = oneapi::mkl::blas::column_major::scal(device_queue->val, n,
                                        static_cast<std::complex<double> >(alpha),
                                        reinterpret_cast<std::complex<double> *>(x),incx);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZdscal(syclQueue_t device_queue, int64_t n,
                            double alpha, double _Complex *x,
                            int64_t incx) {
    auto status = oneapi::mkl::blas::column_major::scal(device_queue->val, n, alpha,
                                        reinterpret_cast<std::complex<double> *>(x),incx);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklChemm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo upper_lower, int64_t m, int64_t n, 
                            float _Complex alpha, const float _Complex *a,
                            int64_t lda, const float _Complex *b, int64_t ldb,
                            float _Complex beta, float _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::hemm(device_queue->val, convert(left_right),
                                          convert(upper_lower), m, n, 
                                          static_cast<std::complex<float> >(alpha),
                                          reinterpret_cast<const std::complex<float> *>(a),
                                          lda, reinterpret_cast<const std::complex<float> *>(b),
                                          ldb, static_cast<std::complex<float> >(beta),
                                          reinterpret_cast<std::complex<float> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZhemm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo upper_lower, int64_t m, int64_t n, 
                            double _Complex alpha, const double _Complex *a,
                            int64_t lda, const double _Complex *b, int64_t ldb,
                            double _Complex beta, double _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::hemm(device_queue->val, convert(left_right),
                                          convert(upper_lower), m, n, 
                                          static_cast<std::complex<double> >(alpha),
                                          reinterpret_cast<const std::complex<double> *>(a),
                                          lda, reinterpret_cast<const std::complex<double> *>(b),
                                          ldb, static_cast<std::complex<double> >(beta),
                                          reinterpret_cast<std::complex<double> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCherk(syclQueue_t device_queue, onemklUplo upper_lower,
                            onemklTranspose trans, int64_t n, int64_t k, float alpha,
                            const float _Complex *a, int64_t lda, float beta,
                            float _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::herk(device_queue->val, convert(upper_lower),
                                          convert(trans), n, k, alpha,
                                          reinterpret_cast<const std::complex<float> *>(a),
                                          lda, beta, reinterpret_cast<std::complex<float> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZherk(syclQueue_t device_queue, onemklUplo upper_lower,
                            onemklTranspose trans, int64_t n, int64_t k, double alpha,
                            const double _Complex *a, int64_t lda, double beta,
                            double _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::herk(device_queue->val, convert(upper_lower),
                                          convert(trans), n, k, alpha,
                                          reinterpret_cast<const std::complex<double> *>(a),
                                          lda, beta, reinterpret_cast<std::complex<double> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCher2k(syclQueue_t device_queue, onemklUplo upper_lower,
                             onemklTranspose trans, int64_t n, int64_t k, 
                             float _Complex alpha, const float _Complex *a,
                             int64_t lda, const float _Complex *b, int64_t ldb, 
                             float beta, float _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::her2k(device_queue->val, convert(upper_lower),
                                           convert(trans), n, k, static_cast<std::complex<float> >(alpha),
                                           reinterpret_cast<const std::complex<float> *>(a), lda,
                                           reinterpret_cast<const std::complex<float> *>(b), ldb,
                                           beta, reinterpret_cast<std::complex<float> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZher2k(syclQueue_t device_queue, onemklUplo upper_lower,
                             onemklTranspose trans, int64_t n, int64_t k, 
                             double _Complex alpha, const double _Complex *a,
                             int64_t lda, const double _Complex *b, int64_t ldb, 
                             double beta, double _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::her2k(device_queue->val, convert(upper_lower),
                                           convert(trans), n, k, static_cast<std::complex<double> >(alpha),
                                           reinterpret_cast<const std::complex<double> *>(a), lda,
                                           reinterpret_cast<const std::complex<double> *>(b), ldb,
                                           beta, reinterpret_cast<std::complex<double> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklStrmm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo uppler_lower, onemklTranspose trans,
                            onemklDiag diag, int64_t m, int64_t n, float alpha,
                            const float *a, int64_t lda, float *b, int64_t ldb) {
    auto status = oneapi::mkl::blas::column_major::trmm(device_queue->val, convert(left_right),
                                          convert(uppler_lower), convert(trans),
                                          convert(diag), m, n, alpha, a, lda, b, ldb);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDtrmm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo uppler_lower, onemklTranspose trans,
                            onemklDiag diag, int64_t m, int64_t n, double alpha,
                            const double *a, int64_t lda, double *b, int64_t ldb) {
    auto status = oneapi::mkl::blas::column_major::trmm(device_queue->val, convert(left_right),
                                          convert(uppler_lower), convert(trans),
                                          convert(diag), m, n, alpha, a, lda, b, ldb);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCtrmm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo uppler_lower, onemklTranspose trans,
                            onemklDiag diag, int64_t m, int64_t n, float _Complex alpha,
                            const float _Complex *a, int64_t lda, float _Complex *b,
                            int64_t ldb) {
    auto status = oneapi::mkl::blas::column_major::trmm(device_queue->val, convert(left_right),
                                          convert(uppler_lower), convert(trans),
                                          convert(diag), m, n, static_cast<std::complex<float> >(alpha),
                                          reinterpret_cast<const std::complex<float> *>(a), lda,
                                          reinterpret_cast<std::complex<float> *>(b), ldb);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZtrmm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo uppler_lower, onemklTranspose trans,
                            onemklDiag diag, int64_t m, int64_t n, double _Complex alpha,
                            const double _Complex *a, int64_t lda, double _Complex *b, int64_t ldb) {
    auto status = oneapi::mkl::blas::column_major::trmm(device_queue->val, convert(left_right),
                                          convert(uppler_lower), convert(trans),
                                          convert(diag), m, n, static_cast<std::complex<double> >(alpha),
                                          reinterpret_cast<const std::complex<double> *>(a), lda,
                                          reinterpret_cast<std::complex<double> *>(b), ldb);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklStrsm(syclQueue_t device_queue, onemklSide left_right, onemklUplo upper_lower,
                            onemklTranspose transa, onemklDiag unit_diag, int64_t m, int64_t n,
                            float alpha, const float *a, int64_t lda, float *b, int64_t ldb) {
    auto status = oneapi::mkl::blas::column_major::trsm(device_queue->val, convert(left_right),
                                        convert(upper_lower), convert(transa), convert(unit_diag),
                                        m, n, alpha, a, lda, b, ldb);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDtrsm(syclQueue_t device_queue, onemklSide left_right, onemklUplo upper_lower,
                            onemklTranspose transa, onemklDiag unit_diag, int64_t m, int64_t n,
                            double alpha, const double *a, int64_t lda, double *b, int64_t ldb) {
    auto status = oneapi::mkl::blas::column_major::trsm(device_queue->val, convert(left_right),
                                        convert(upper_lower), convert(transa), convert(unit_diag),
                                        m, n, alpha, a, lda, b, ldb);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCtrsm(syclQueue_t device_queue, onemklSide left_right, onemklUplo upper_lower,
                            onemklTranspose transa, onemklDiag unit_diag, int64_t m, int64_t n,
                            float _Complex alpha, const float _Complex *a, int64_t lda, float _Complex *b,
                            int64_t ldb) {
    auto status = oneapi::mkl::blas::column_major::trsm(device_queue->val, convert(left_right),
                                        convert(upper_lower), convert(transa), convert(unit_diag),
                                        m, n, static_cast<std::complex<float> >(alpha),
                                        reinterpret_cast<const std::complex<float> *>(a), lda,
                                        reinterpret_cast<std::complex<float> *>(b), ldb);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZtrsm(syclQueue_t device_queue, onemklSide left_right, onemklUplo upper_lower,
                            onemklTranspose transa, onemklDiag unit_diag, int64_t m, int64_t n,
                            double _Complex alpha, const double _Complex *a, int64_t lda,
                            double _Complex *b, int64_t ldb) {
    auto status = oneapi::mkl::blas::column_major::trsm(device_queue->val, convert(left_right),
                                        convert(upper_lower), convert(transa), convert(unit_diag),
                                        m, n, static_cast<std::complex<double> >(alpha),
                                        reinterpret_cast<const std::complex<double> *>(a), lda,
                                        reinterpret_cast<std::complex<double> *>(b), ldb);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklSsymm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo upper_lower, int64_t m, int64_t n,
                            float alpha, const float *a, int64_t lda, const float *b,
                            int64_t ldb, float beta, float *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::symm(device_queue->val, convert(left_right),
                                        convert(upper_lower), m, n, alpha, a, lda, b,
                                        ldb, beta, c, ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDsymm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo upper_lower, int64_t m, int64_t n,
                            double alpha, const double *a, int64_t lda, const double *b,
                            int64_t ldb, double beta, double *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::symm(device_queue->val, convert(left_right),
                                        convert(upper_lower), m, n, alpha, a, lda, b,
                                        ldb, beta, c, ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCsymm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo upper_lower, int64_t m, int64_t n,
                            float _Complex alpha, const float _Complex *a, int64_t lda,
                            const float _Complex *b, int64_t ldb, float _Complex beta,
                            float _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::symm(device_queue->val, convert(left_right),
                                          convert(upper_lower), m, n,
                                          static_cast<std::complex<float> >(alpha),
                                          reinterpret_cast<const std::complex<float> *>(a),
                                          lda, reinterpret_cast<const std::complex<float> *>(b),
                                          ldb, beta, reinterpret_cast<std::complex<float> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZsymm(syclQueue_t device_queue, onemklSide left_right,
                            onemklUplo upper_lower, int64_t m, int64_t n,
                            double _Complex alpha, const double _Complex *a, int64_t lda,
                            const double _Complex *b, int64_t ldb, double _Complex beta,
                            double _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::symm(device_queue->val, convert(left_right),
                                          convert(upper_lower), m, n,
                                          static_cast<std::complex<double> >(alpha),
                                          reinterpret_cast<const std::complex<double> *>(a), lda,
                                          reinterpret_cast<const std::complex<double> *>(b), ldb,
                                          static_cast<std::complex<double> >(beta),
                                          reinterpret_cast<std::complex<double> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklSsyrk(syclQueue_t device_queue, onemklUplo upper_lower,
                            onemklTranspose trans, int64_t n, int64_t k, float alpha,
                            const float *a, int64_t lda, float beta, float *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::syrk(device_queue->val, convert(upper_lower),
                                        convert(trans), n, k, alpha, a, lda, beta, c, ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDsyrk(syclQueue_t device_queue, onemklUplo upper_lower,
                            onemklTranspose trans, int64_t n, int64_t k, double alpha,
                            const double *a, int64_t lda, double beta, double *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::syrk(device_queue->val, convert(upper_lower),
                                        convert(trans), n, k, alpha, a, lda, beta, c, ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCsyrk(syclQueue_t device_queue, onemklUplo upper_lower,
                            onemklTranspose trans, int64_t n, int64_t k, float _Complex alpha,
                            const float _Complex *a, int64_t lda, float _Complex beta, float _Complex *c,
                            int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::syrk(device_queue->val, convert(upper_lower),
                                        convert(trans), n, k, static_cast<std::complex<float> >(alpha),
                                        reinterpret_cast<const std::complex<float> *>(a), lda,
                                        static_cast<std::complex<float> >(beta),
                                        reinterpret_cast<std::complex<float> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZsyrk(syclQueue_t device_queue, onemklUplo upper_lower,
                            onemklTranspose trans, int64_t n, int64_t k, double _Complex alpha,
                            const double _Complex *a, int64_t lda, double _Complex beta, double _Complex *c,
                            int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::syrk(device_queue->val, convert(upper_lower), convert(trans),
                                          n, k, static_cast<std::complex<float> >(alpha),
                                          reinterpret_cast<const std::complex<double> *>(a), lda,
                                          static_cast<std::complex<double> >(beta),
                                          reinterpret_cast<std::complex<double> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklSsyr2k(syclQueue_t device_queue, onemklUplo upper_lower, onemklTranspose trans,
                             int64_t n, int64_t k, float alpha, const float *a, int64_t lda,
                             const float *b, int64_t ldb, float beta, float *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::syr2k(device_queue->val, convert(upper_lower),
                                        convert(trans), n, k, alpha, a, lda, b, ldb, beta, c, ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDsyr2k(syclQueue_t device_queue, onemklUplo upper_lower, onemklTranspose trans,
                             int64_t n, int64_t k, double alpha, const double *a, int64_t lda,
                             const double *b, int64_t ldb, float beta, double *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::syr2k(device_queue->val, convert(upper_lower),
                                        convert(trans), n, k, alpha, a, lda, b, ldb, beta, c, ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCsyr2k(syclQueue_t device_queue, onemklUplo upper_lower, onemklTranspose trans,
                             int64_t n, int64_t k, float _Complex alpha, const float _Complex *a,
                             int64_t lda, const float _Complex *b, int64_t ldb, float _Complex beta,
                             float _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::syr2k(device_queue->val, convert(upper_lower),
                                        convert(trans), n, k, static_cast<std::complex<float> >(alpha),
                                           reinterpret_cast<const std::complex<float> *>(a), lda,
                                           reinterpret_cast<const std::complex<float> *>(b), ldb,
                                           static_cast<std::complex<float> >(beta),
                                           reinterpret_cast<std::complex<float> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZsyr2k(syclQueue_t device_queue, onemklUplo upper_lower, onemklTranspose trans,
                             int64_t n, int64_t k, double _Complex alpha, const double _Complex *a,
                             int64_t lda, const double _Complex *b, int64_t ldb, double _Complex beta,
                             float _Complex *c, int64_t ldc) {
    auto status = oneapi::mkl::blas::column_major::syr2k(device_queue->val, convert(upper_lower),
                                        convert(trans), n, k, static_cast<std::complex<double> >(alpha),
                                        reinterpret_cast<const std::complex<double> *>(a), lda,
                                        reinterpret_cast<const std::complex<double> *>(b), ldb,
                                        static_cast<std::complex<double> >(beta),
                                        reinterpret_cast<std::complex<double> *>(c), ldc);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDnrm2(syclQueue_t device_queue, int64_t n, const double *x, 
                            int64_t incx, double *result) {
    auto status = oneapi::mkl::blas::column_major::nrm2(device_queue->val, n, x, incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklSnrm2(syclQueue_t device_queue, int64_t n, const float *x, 
                            int64_t incx, float *result) {
    auto status = oneapi::mkl::blas::column_major::nrm2(device_queue->val, n, x, incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCnrm2(syclQueue_t device_queue, int64_t n, const float _Complex *x, 
                            int64_t incx, float *result) {   
    auto status = oneapi::mkl::blas::column_major::nrm2(device_queue->val, n, 
                    reinterpret_cast<const std::complex<float> *>(x), incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZnrm2(syclQueue_t device_queue, int64_t n, const double _Complex *x, 
                            int64_t incx, double *result) {
    auto status = oneapi::mkl::blas::column_major::nrm2(device_queue->val, n, 
                    reinterpret_cast<const std::complex<double> *>(x), incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDcopy(syclQueue_t device_queue, int64_t n, const double *x,
                            int64_t incx, double *y, int64_t incy) {
    auto status = oneapi::mkl::blas::column_major::copy(device_queue->val, n, x, incx, y, incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklScopy(syclQueue_t device_queue, int64_t n, const float *x,
                            int64_t incx, float *y, int64_t incy) {
    auto status = oneapi::mkl::blas::column_major::copy(device_queue->val, n, x, incx, y, incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZcopy(syclQueue_t device_queue, int64_t n, const double _Complex *x,
                            int64_t incx, double _Complex *y, int64_t incy) {
    auto status = oneapi::mkl::blas::column_major::copy(device_queue->val, n,
        reinterpret_cast<const std::complex<double> *>(x), incx,
        reinterpret_cast<std::complex<double> *>(y), incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCcopy(syclQueue_t device_queue, int64_t n, const float _Complex *x,
                            int64_t incx, float _Complex *y, int64_t incy) {
    auto status = oneapi::mkl::blas::column_major::copy(device_queue->val, n, 
        reinterpret_cast<const std::complex<float> *>(x), incx, 
        reinterpret_cast<std::complex<float> *>(y), incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDamax(syclQueue_t device_queue, int64_t n, const double *x,
                            int64_t incx, int64_t *result){
    auto status = oneapi::mkl::blas::column_major::iamax(device_queue->val, n, x, incx, result);
    __FORCE_MKL_FLUSH__(status);
}
extern "C" void onemklSamax(syclQueue_t device_queue, int64_t n, const float  *x,
                            int64_t incx, int64_t *result){
    auto status = oneapi::mkl::blas::column_major::iamax(device_queue->val, n, x, incx, result);
    __FORCE_MKL_FLUSH__(status);
}
extern "C" void onemklZamax(syclQueue_t device_queue, int64_t n, const double _Complex *x,
                            int64_t incx, int64_t *result){
    auto status = oneapi::mkl::blas::column_major::iamax(device_queue->val, n,
                            reinterpret_cast<const std::complex<double> *>(x), incx, result);
    __FORCE_MKL_FLUSH__(status);
}
extern "C" void onemklCamax(syclQueue_t device_queue, int64_t n, const float _Complex *x,
                            int64_t incx, int64_t *result){
    auto status = oneapi::mkl::blas::column_major::iamax(device_queue->val, n,
                            reinterpret_cast<const std::complex<float> *>(x), incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDamin(syclQueue_t device_queue, int64_t n, const double *x,
                            int64_t incx, int64_t *result){
    auto status = oneapi::mkl::blas::column_major::iamin(device_queue->val, n, x, incx, result);
    __FORCE_MKL_FLUSH__(status);
}
extern "C" void onemklSamin(syclQueue_t device_queue, int64_t n, const float  *x,
                            int64_t incx, int64_t *result){
    auto status = oneapi::mkl::blas::column_major::iamin(device_queue->val, n, x, incx, result);
    __FORCE_MKL_FLUSH__(status);
}
extern "C" void onemklZamin(syclQueue_t device_queue, int64_t n, const double _Complex *x,
                            int64_t incx, int64_t *result){
    auto status = oneapi::mkl::blas::column_major::iamin(device_queue->val, n,
                            reinterpret_cast<const std::complex<double> *>(x), incx, result);
    __FORCE_MKL_FLUSH__(status);
}
extern "C" void onemklCamin(syclQueue_t device_queue, int64_t n, const float _Complex *x,
                            int64_t incx, int64_t *result){
    auto status = oneapi::mkl::blas::column_major::iamin(device_queue->val, n,
                            reinterpret_cast<const std::complex<float> *>(x), incx, result);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklSswap(syclQueue_t device_queue, int64_t n, float *x, int64_t incx,\
                            float *y, int64_t incy){
    auto status = oneapi::mkl::blas::column_major::swap(device_queue->val, n, x, incx, y, incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklDswap(syclQueue_t device_queue, int64_t n, double *x, int64_t incx,
                            double *y, int64_t incy){
    auto status = oneapi::mkl::blas::column_major::swap(device_queue->val, n, x, incx, y, incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklCswap(syclQueue_t device_queue, int64_t n, float _Complex *x, int64_t incx,
                            float _Complex *y, int64_t incy){
    auto status = oneapi::mkl::blas::column_major::swap(device_queue->val, n,
                            reinterpret_cast<std::complex<float> *>(x), incx,
                            reinterpret_cast<std::complex<float> *>(y), incy);
    __FORCE_MKL_FLUSH__(status);
}

extern "C" void onemklZswap(syclQueue_t device_queue, int64_t n, double _Complex *x, int64_t incx,
                            double _Complex *y, int64_t incy){
    auto status = oneapi::mkl::blas::column_major::swap(device_queue->val, n,
                            reinterpret_cast<std::complex<double> *>(x), incx,
                            reinterpret_cast<std::complex<double> *>(y), incy);
    __FORCE_MKL_FLUSH__(status);
}

// other

// oneMKL keeps a cache of SYCL queues and tries to destroy them when unloading the library.
// that is incompatible with oneAPI.jl destroying queues before that, so expose a function
// to manually wipe the device cache when we're destroying queues.

namespace oneapi {
namespace mkl {
namespace gpu {
int clean_gpu_caches();
}
}
}

extern "C" void onemklDestroy() {
    oneapi::mkl::gpu::clean_gpu_caches();
}
