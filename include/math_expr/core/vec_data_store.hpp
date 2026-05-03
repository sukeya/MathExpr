/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Author: Arash Partow (1999-2025)                           *
 * URL: https://www.partow.net/programming/exprtk/index.html  *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the MIT License.                   *
 * https://www.opensource.org/licenses/MIT                    *
 * SPDX-License-Identifier: MIT                               *
 *                                                            *
 **************************************************************


Copyright 2026 Yuya Asano <my_favorite_theory@yahoo.co.jp>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef MATH_EXPR_CORE_VEC_DATA_STORE_HPP
#define MATH_EXPR_CORE_VEC_DATA_STORE_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/config.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/core/debug.hpp"

namespace math_expr::core
{
template <typename T>
class vec_data_store
{
   public:
    using type = vec_data_store<T>;
    using data_t = T*;

   private:
    struct control_block
    {
        control_block() : size(0), data(nullptr), destruct(false) {}

        explicit control_block(const std::size_t& dsize)
            : size(dsize), data(nullptr), destruct(true)
        {
            create_data();
        }

        control_block(const std::size_t& dsize, data_t dptr, bool dstrct = false)
            : size(dsize), data(dptr), destruct(dstrct)
        {
        }

        std::size_t size;
        data_t data;
        bool destruct;

       private:
        control_block(const control_block&) = delete;
        control_block& operator=(const control_block&) = delete;

        std::unique_ptr<T[]> owned_data_;

        inline void create_data()
        {
            owned_data_ = std::make_unique<T[]>(size);
            data = owned_data_.get();
            std::fill_n(data, size, T(0));
            dump_ptr("control_block::create_data() - data", data, size);
        }
    };

   public:
    vec_data_store() : control_block_(std::make_shared<control_block>()) {}

    explicit vec_data_store(const std::size_t& size)
        : control_block_(std::make_shared<control_block>(size))
    {
    }

    vec_data_store(const std::size_t& size, data_t data, bool dstrct = false)
        : control_block_(std::make_shared<control_block>(size, data, dstrct))
    {
    }

    vec_data_store(const type& vds) : control_block_(vds.control_block_) {}

    ~vec_data_store() = default;

    type& operator=(const type& vds)
    {
        if (this != &vds)
        {
            const std::size_t final_size = min_size(control_block_, vds.control_block_);

            vds.control_block_->size = final_size;
            control_block_->size = final_size;

            if (control_block_->destruct || (nullptr == control_block_->data))
            {
                control_block_ = vds.control_block_;
            }
        }

        return (*this);
    }

    inline data_t data()
    {
        return control_block_->data;
    }

    inline data_t data() const
    {
        return control_block_->data;
    }

    inline std::size_t size() const
    {
        return control_block_->size;
    }

    inline data_t& ref()
    {
        return control_block_->data;
    }

    inline void dump() const
    {
        if constexpr (::math_expr::core::build_options::kEnableDebugging)
        {
            MATH_EXPR_DEBUG(("size: %d\taddress:%p\tdestruct:%c\n", size(), data(),
                             (control_block_->destruct ? 'T' : 'F')));

            for (std::size_t i = 0; i < size(); ++i)
            {
                if (5 == i)
                    MATH_EXPR_DEBUG(("\n"));

                MATH_EXPR_DEBUG(("%15.10f ", data()[i]));
            }
            MATH_EXPR_DEBUG(("\n"));
        }
    }

    static inline void match_sizes(type& vds0, type& vds1)
    {
        const std::size_t size = min_size(vds0.control_block_, vds1.control_block_);
        vds0.control_block_->size = size;
        vds1.control_block_->size = size;
    }

   private:
    static inline std::size_t min_size(const std::shared_ptr<control_block>& cb0,
                                       const std::shared_ptr<control_block>& cb1)
    {
        const std::size_t size0 = cb0->size;
        const std::size_t size1 = cb1->size;

        if (size0 && size1)
            return std::min(size0, size1);
        else
            return (size0) ? size0 : size1;
    }

    std::shared_ptr<control_block> control_block_;
};

}  // namespace math_expr::core

#endif
