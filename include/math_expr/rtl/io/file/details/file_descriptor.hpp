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

#ifndef MATH_EXPR_RTL_IO_FILE_DETAILS_FILE_DESCRIPTOR_HPP
#define MATH_EXPR_RTL_IO_FILE_DETAILS_FILE_DESCRIPTOR_HPP

#include <memory>

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"
#include "math_expr/rtl/io/file/details/input_file_stream.hpp"
#include "math_expr/rtl/io/file/details/output_file_stream.hpp"
#include "math_expr/rtl/io/file/details/read_write_file_stream.hpp"

namespace math_expr::rtl::io::file::details
{
using ::math_expr::core::char_cptr;
using ::math_expr::core::char_ptr;

enum class file_mode : int
{
    error = 0,
    read = 1,
    write = 2,
    read_write = 4
};

struct file_descriptor
{
    file_descriptor(const std::string& fname, const std::string& access)
        : stream_(nullptr), mode(get_file_mode(access)), file_name(fname)
    {
    }

    ~file_descriptor() noexcept
    {
        close();
    }

    bool open()
    {
        stream_.reset();

        switch (mode)
        {
            case file_mode::read:
                stream_ = std::make_unique<input_file_stream>(file_name);
                break;

            case file_mode::write:
                stream_ = std::make_unique<output_file_stream>(file_name);
                break;

            case file_mode::read_write:
                stream_ = std::make_unique<read_write_file_stream>(file_name);
                break;

            default:
                return false;
        }

        if (!stream_ || !stream_->is_open())
        {
            file_name.clear();
            stream_.reset();
            return false;
        }

        return true;
    }

    bool close() noexcept
    {
        if (!stream_)
        {
            return true;
        }

        const bool result = stream_->close();
        stream_.reset();
        return result;
    }

    template <typename View>
    bool write(const View& view, const std::size_t amount, const std::size_t offset = 0)
    {
        if (!stream_)
        {
            return false;
        }

        return stream_->write(reinterpret_cast<char_cptr>(view.begin() + offset),
                              amount * sizeof(typename View::value_t));
    }

    template <typename View>
    bool read(View& view, const std::size_t amount, const std::size_t offset = 0)
    {
        if (!stream_)
        {
            return false;
        }

        return stream_->read(reinterpret_cast<char_ptr>(view.begin() + offset),
                             amount * sizeof(typename View::value_t));
    }

    bool getline(std::string& s)
    {
        if (!stream_)
        {
            return false;
        }

        return stream_->getline(s);
    }

    bool eof() const
    {
        if (!stream_)
        {
            return true;
        }

        return stream_->eof();
    }

    file_mode get_file_mode(const std::string& access) const
    {
        if (access.empty() || access.size() > 2)
            return file_mode::error;

        std::size_t w_cnt = 0;
        std::size_t r_cnt = 0;

        for (std::size_t i = 0; i < access.size(); ++i)
        {
            switch (std::tolower(access[i]))
            {
                case 'r':
                    r_cnt++;
                    break;
                case 'w':
                    w_cnt++;
                    break;
                default:
                    return file_mode::error;
            }
        }

        if ((0 == r_cnt) && (0 == w_cnt))
            return file_mode::error;
        else if ((r_cnt > 1) || (w_cnt > 1))
            return file_mode::error;
        else if ((1 == r_cnt) && (1 == w_cnt))
            return file_mode::read_write;
        else if (1 == r_cnt)
            return file_mode::read;
        else
            return file_mode::write;
    }

    std::unique_ptr<stream_base> stream_;
    file_mode mode;
    std::string file_name;
};
}  // namespace math_expr::rtl::io::file::details

#endif
