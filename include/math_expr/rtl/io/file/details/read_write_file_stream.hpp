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

#ifndef MATH_EXPR_RTL_IO_FILE_DETAILS_READ_WRITE_FILE_STREAM_HPP
#define MATH_EXPR_RTL_IO_FILE_DETAILS_READ_WRITE_FILE_STREAM_HPP

#include "math_expr/rtl/io/file/details/stream_base.hpp"

namespace math_expr::rtl::io::file::details
{
class read_write_file_stream final : public stream_base
{
   public:
    explicit read_write_file_stream(const std::string& file_name)
        : stream_(file_name.c_str(), std::ios::binary)
    {
    }

    bool is_open() const override
    {
        return stream_.is_open() && static_cast<bool>(stream_);
    }

    bool close() noexcept override
    {
        if (stream_.is_open())
        {
            stream_.close();
        }

        return !stream_.is_open();
    }

    bool write(char_cptr data, std::size_t size) override
    {
        stream_.write(data, static_cast<std::streamsize>(size));
        return true;
    }

    bool read(char_ptr data, std::size_t size) override
    {
        stream_.read(data, static_cast<std::streamsize>(size));
        return true;
    }

    bool getline(std::string& s) override
    {
        return !!std::getline(stream_, s);
    }

    bool eof() const override
    {
        return stream_.eof();
    }

   private:
    std::fstream stream_;
};
}  // namespace math_expr::rtl::io::file::details

#endif
