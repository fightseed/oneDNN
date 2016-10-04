/*******************************************************************************
* Copyright 2016 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include <assert.h>
#include "mkldnn.h"

#include "c_types_map.hpp"
#include "utils.hpp"

using namespace mkldnn::impl;
using namespace mkldnn::impl::utils;
using namespace mkldnn::impl::status;
using namespace mkldnn::impl::prop_kind;
using namespace mkldnn::impl::alg_kind;

namespace {
status_t relu_desc_init(relu_desc_t *relu_desc, prop_kind_t prop_kind,
        const memory_desc_t *data_desc, const memory_desc_t *diff_data_desc,
        double negative_slope) {
    bool args_ok = true
        && !any_null(relu_desc, data_desc)
        && one_of(prop_kind, forward_training, forward_inference, backward_data)
        && implication(prop_kind == backward_data, diff_data_desc != nullptr);
    if (!args_ok) return invalid_arguments;

    relu_desc_t rd = {};
    rd.primitive_kind = primitive_kind::relu;
    rd.prop_kind = prop_kind;

    rd.data_desc = *data_desc;
    if (rd.prop_kind == backward_data)
        rd.diff_data_desc = *diff_data_desc;
    rd.negative_slope = negative_slope;

    bool consistency = true
        && rd.data_desc.ndims == 4;
    if (rd.prop_kind == backward_data)
        consistency = consistency
            && rd.diff_data_desc.ndims == 4
            && array_cmp(rd.diff_data_desc.dims, rd.data_desc.dims, 4);
    if (!consistency) return invalid_arguments;

    *relu_desc = rd;
    return success;
}
}

status_t mkldnn_relu_forward_desc_init(relu_desc_t *relu_desc,
        prop_kind_t prop_kind, const memory_desc_t *data_desc,
        double negative_slope) {
    if (!one_of(prop_kind, forward_training, forward_inference))
        return invalid_arguments;
    return relu_desc_init(relu_desc, prop_kind, data_desc, nullptr,
            negative_slope);
}

status_t mkldnn_relu_backward_desc_init(relu_desc_t *relu_desc,
        const memory_desc_t *diff_data_desc, const memory_desc_t *data_desc,
        double negative_slope) {
    return relu_desc_init(relu_desc, backward_data, data_desc, diff_data_desc,
            negative_slope);
}


// vim: et ts=4 sw=4 cindent cino^=l0,\:0,N-s
