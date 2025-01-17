#pragma once
#include "cuda_runtime.h"

namespace EasyRender::UniUtils
{

template<typename T, typename U>
__host__ __device__ __forceinline__ T Barycentric(T v1, T v2, T v3, U coord)
{
    return v1 * (1 - coord.x - coord.y) + v2 * coord.x + v3 * coord.y;
}

template<typename T, typename U, typename V>
__host__ __device__ __forceinline__ T BarycentricByIndices(T *v, U indices,
                                                           V coord)
{
    return Barycentric(v[indices.x], v[indices.y], v[indices.z], coord);
}

} // namespace EasyRender::UniUtils
