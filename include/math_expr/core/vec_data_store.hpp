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
        control_block() : ref_count(1), size(0), data(0), destruct(true) {}

        explicit control_block(const std::size_t& dsize)
            : ref_count(1), size(dsize), data(0), destruct(true)
        {
            create_data();
        }

        control_block(const std::size_t& dsize, data_t dptr, bool dstrct = false)
            : ref_count(1), size(dsize), data(dptr), destruct(dstrct)
        {
        }

        ~control_block()
        {
            if (data && destruct && (0 == ref_count))
            {
                dump_ptr("~vec_data_store::control_block() data", data);
                delete[] data;
                data = reinterpret_cast<data_t>(0);
            }
        }

        static inline control_block* create(const std::size_t& dsize, data_t data_ptr = data_t(0),
                                            bool dstrct = false)
        {
            if (dsize)
            {
                if (0 == data_ptr)
                    return (new control_block(dsize));
                else
                    return (new control_block(dsize, data_ptr, dstrct));
            }
            else
                return (new control_block);
        }

        static inline void destroy(control_block*& cntrl_blck)
        {
            if (cntrl_blck)
            {
                if ((0 != cntrl_blck->ref_count) && (0 == --cntrl_blck->ref_count))
                {
                    delete cntrl_blck;
                }

                cntrl_blck = 0;
            }
        }

        std::size_t ref_count;
        std::size_t size;
        data_t data;
        bool destruct;

       private:
        control_block(const control_block&) = delete;
        control_block& operator=(const control_block&) = delete;

        inline void create_data()
        {
            destruct = true;
            data = new T[size];
            std::fill_n(data, size, T(0));
            dump_ptr("control_block::create_data() - data", data, size);
        }
    };

   public:
    vec_data_store() : control_block_(control_block::create(0)) {}

    explicit vec_data_store(const std::size_t& size)
        : control_block_(control_block::create(size, reinterpret_cast<data_t>(0), true))
    {
    }

    vec_data_store(const std::size_t& size, data_t data, bool dstrct = false)
        : control_block_(control_block::create(size, data, dstrct))
    {
    }

    vec_data_store(const type& vds)
    {
        control_block_ = vds.control_block_;
        control_block_->ref_count++;
    }

    ~vec_data_store()
    {
        control_block::destroy(control_block_);
    }

    type& operator=(const type& vds)
    {
        if (this != &vds)
        {
            const std::size_t final_size = min_size(control_block_, vds.control_block_);

            vds.control_block_->size = final_size;
            control_block_->size = final_size;

            if (control_block_->destruct || (0 == control_block_->data))
            {
                control_block::destroy(control_block_);

                control_block_ = vds.control_block_;
                control_block_->ref_count++;
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
            math_expr_debug(("size: %d\taddress:%p\tdestruct:%c\n", size(), data(),
                             (control_block_->destruct ? 'T' : 'F')));

            for (std::size_t i = 0; i < size(); ++i)
            {
                if (5 == i)
                    math_expr_debug(("\n"));

                math_expr_debug(("%15.10f ", data()[i]));
            }
            math_expr_debug(("\n"));
        }
    }

    static inline void match_sizes(type& vds0, type& vds1)
    {
        const std::size_t size = min_size(vds0.control_block_, vds1.control_block_);
        vds0.control_block_->size = size;
        vds1.control_block_->size = size;
    }

   private:
    static inline std::size_t min_size(const control_block* cb0, const control_block* cb1)
    {
        const std::size_t size0 = cb0->size;
        const std::size_t size1 = cb1->size;

        if (size0 && size1)
            return std::min(size0, size1);
        else
            return (size0) ? size0 : size1;
    }

    control_block* control_block_;
};

}  // namespace math_expr::core

#endif
