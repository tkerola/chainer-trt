/*
 * Copyright (c) 2018 Preferred Networks, Inc. All rights reserved.
 */

#include <fstream>

#include <cuda_runtime_api.h>

#include "chainer_trt/chainer_trt.hpp"
#include "include/chainer_trt_impl.hpp"
#include "include/cuda/cuda_kernels.hpp"
#include "test_helper.hpp"

using chainer_trt::internal::make_dims;

/*
 * Test cases are generated by this kind of code:
 * test_case_input_shape_list = [
 *     (1, 3, 1, 1), (1, 1, 2, 1), (1, 1, 1, 4),
 *     (1, 1, 2, 4), (1, 3, 1, 4), (1, 3, 2, 1),
 * ]
 * for i in range(len(test_case_input_shape_list)):
 *     input_shape = test_case_input_shape_list[i]
 *     x1 = chainer.Variable(np.random.randn(*input_shape).astype(np.float32))
 *     print('x1', x1.shape, x1)
 *     with chainer.using_config('train', False):
 *         with RetainHook():
 *             y1 = chainer.functions.broadcast_to(x1, (1, 3, 2, 4))
 *     out = "test/fixtures/plugins/broadcast_to/case{}".format(i)
 *     if not os.path.exists(out):
 *         os.mkdir(out)
 *     ModelRetriever(out)(y1).save()
 *     open(out + "/in.csv", 'wt').write(atos(x1))
 *     open(out + "/out.csv", 'wt').write(atos(y1))
 */

using TestParams = std::tuple<int, nvinfer1::DataType, nvinfer1::Dims,
                              nvinfer1::Dims, std::string>;

class BroadcastToKernelParameterizedTest
  : public ::testing::TestWithParam<TestParams> {};

TEST_P(BroadcastToKernelParameterizedTest, CheckValues) {
    cudaSetDevice(0);

    auto param = GetParam();
    const int batch_size = std::get<0>(param);
    const nvinfer1::DataType data_type = std::get<1>(param);
    const nvinfer1::Dims in_dims = std::get<2>(param);
    const nvinfer1::Dims out_dims = std::get<3>(param);
    const auto dir = "test/fixtures/plugins/broadcast_to/" + std::get<4>(param);

    chainer_trt::plugin::broadcast_to broadcast_src(in_dims, out_dims);
    run_plugin_assert(broadcast_src, batch_size, data_type, in_dims, dir,
                      0.001);
}

static const nvinfer1::DataType kFLOAT = nvinfer1::DataType::kFLOAT;
static const nvinfer1::DataType kHALF = nvinfer1::DataType::kHALF;
INSTANTIATE_TEST_CASE_P(
  CheckOutputValue, BroadcastToKernelParameterizedTest,
  ::testing::Values(
    TestParams(1, kFLOAT, make_dims(3, 1, 1), make_dims(3, 2, 4), "case0"),
    TestParams(1, kFLOAT, make_dims(1, 2, 1), make_dims(3, 2, 4), "case1"),
    TestParams(1, kFLOAT, make_dims(1, 1, 4), make_dims(3, 2, 4), "case2"),
    TestParams(1, kFLOAT, make_dims(1, 2, 4), make_dims(3, 2, 4), "case3"),
    TestParams(1, kFLOAT, make_dims(3, 1, 4), make_dims(3, 2, 4), "case4"),
    TestParams(1, kFLOAT, make_dims(3, 2, 1), make_dims(3, 2, 4), "case5"),
    TestParams(1, kHALF, make_dims(3, 1, 1), make_dims(3, 2, 4), "case0"),
    TestParams(1, kHALF, make_dims(1, 2, 1), make_dims(3, 2, 4), "case1"),
    TestParams(1, kHALF, make_dims(1, 1, 4), make_dims(3, 2, 4), "case2"),
    TestParams(1, kHALF, make_dims(1, 2, 4), make_dims(3, 2, 4), "case3"),
    TestParams(1, kHALF, make_dims(3, 1, 4), make_dims(3, 2, 4), "case4"),
    TestParams(1, kHALF, make_dims(3, 2, 1), make_dims(3, 2, 4), "case5"),

    TestParams(65535, kFLOAT, make_dims(3, 1, 1), make_dims(3, 2, 4), "case0"),
    TestParams(65535, kFLOAT, make_dims(1, 2, 1), make_dims(3, 2, 4), "case1"),
    TestParams(65535, kFLOAT, make_dims(1, 1, 4), make_dims(3, 2, 4), "case2"),
    TestParams(65535, kFLOAT, make_dims(1, 2, 4), make_dims(3, 2, 4), "case3"),
    TestParams(65535, kFLOAT, make_dims(3, 1, 4), make_dims(3, 2, 4), "case4"),
    TestParams(65535, kFLOAT, make_dims(3, 2, 1), make_dims(3, 2, 4), "case5"),
    TestParams(65535, kHALF, make_dims(3, 1, 1), make_dims(3, 2, 4), "case0"),
    TestParams(65535, kHALF, make_dims(1, 2, 1), make_dims(3, 2, 4), "case1"),
    TestParams(65535, kHALF, make_dims(1, 1, 4), make_dims(3, 2, 4), "case2"),
    TestParams(65535, kHALF, make_dims(1, 2, 4), make_dims(3, 2, 4), "case3"),
    TestParams(65535, kHALF, make_dims(3, 1, 4), make_dims(3, 2, 4), "case4"),
    TestParams(65535, kHALF, make_dims(3, 2, 1), make_dims(3, 2, 4), "case5")));