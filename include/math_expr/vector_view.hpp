/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 02                                          *
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

#ifndef MATH_EXPR_VECTOR_VIEW_HPP
#define MATH_EXPR_VECTOR_VIEW_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/macros.hpp"

namespace math_expr
{
template <typename T> class vector_view
{
  public:
    using data_ptr_t = T*;

    vector_view(data_ptr_t data, const std::size_t& size)
        : base_size_(size), size_(size), data_(data), data_ref_(0)
    {
        assert(size_ > 0);
    }

    vector_view(const vector_view<T>& vv)
        : base_size_(vv.base_size_), size_(vv.size_), data_(vv.data_), data_ref_(0)
    {
        assert(size_ > 0);
    }

    inline void rebase(data_ptr_t data)
    {
        data_ = data;

        if (!data_ref_.empty())
        {
            for (std::size_t i = 0; i < data_ref_.size(); ++i)
            {
                (*data_ref_[i]) = data;
            }
        }
    }

    inline data_ptr_t data() const
    {
        return data_;
    }

    inline std::size_t base_size() const
    {
        return base_size_;
    }

    inline std::size_t size() const
    {
        return size_;
    }

    inline const T& operator[](const std::size_t index) const
    {
        assert(index < size_);
        return data_[index];
    }

    inline T& operator[](const std::size_t index)
    {
        assert(index < size_);
        return data_[index];
    }

    void set_ref(data_ptr_t* data_ref)
    {
        data_ref_.push_back(data_ref);
        math_expr_debug(("vector_view::set_ref() - data_ref: %p data_ref_.size(): %d\n",
                         reinterpret_cast<void*>(data_ref), static_cast<int>(data_ref_.size())));
    }

    void set_size_ref(std::size_t* size_ref)
    {
        size_ref_.push_back(size_ref);
        math_expr_debug(("vector_view::set_size_ref() - size_ref: %p size_ref_.size(): %d\n",
                         reinterpret_cast<void*>(size_ref), static_cast<int>(size_ref_.size())));
    }

    void remove_ref(data_ptr_t* data_ref)
    {
        data_ref_.erase(std::remove(data_ref_.begin(), data_ref_.end(), data_ref), data_ref_.end());
        math_expr_debug(("vector_view::remove_ref() - data_ref: %p data_ref_.size(): %d\n",
                         reinterpret_cast<void*>(data_ref), static_cast<int>(data_ref_.size())));
    }

    void remove_size_ref(std::size_t* size_ref)
    {
        size_ref_.erase(std::remove(size_ref_.begin(), size_ref_.end(), size_ref), size_ref_.end());
        math_expr_debug(("vector_view::remove_size_ref() - size_ref: %p size_ref_.size(): %d\n",
                         reinterpret_cast<void*>(size_ref), static_cast<int>(size_ref_.size())));
    }

    bool set_size(const std::size_t new_size)
    {
        if ((new_size > 0) && (new_size <= base_size_))
        {
            size_ = new_size;
            math_expr_debug(("vector_view::set_size() - data_: %p size: %lu\n",
                             reinterpret_cast<void*>(data_), size_));

            if (!size_ref_.empty())
            {
                for (std::size_t i = 0; i < size_ref_.size(); ++i)
                {
                    (*size_ref_[i]) = new_size;
                }
            }

            return true;
        }

        math_expr_debug(("vector_view::set_size() - error invalid new_size: %lu  base_size: %lu\n",
                         new_size, base_size_));
        return false;
    }

  private:
    const std::size_t base_size_;
    std::size_t size_;
    data_ptr_t data_;
    std::vector<data_ptr_t*> data_ref_;
    std::vector<std::size_t*> size_ref_;
};

template <typename T>
inline vector_view<T> make_vector_view(T* data, const std::size_t size,
                                       const std::size_t offset = 0)
{
    return vector_view<T>(data + offset, size);
}

template <typename T>
inline vector_view<T> make_vector_view(std::vector<T>& v, const std::size_t size,
                                       const std::size_t offset = 0)
{
    return vector_view<T>(v.data() + offset, size);
}

template <typename T> class results_context;
namespace details
{
template <typename T> class vector_interface;
}

} // namespace math_expr

#endif
