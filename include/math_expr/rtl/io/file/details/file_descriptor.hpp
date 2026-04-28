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

#ifndef MATH_EXPR_RTL_IO_FILE_DETAILS_FILE_DESCRIPTOR_HPP
#define MATH_EXPR_RTL_IO_FILE_DETAILS_FILE_DESCRIPTOR_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"

namespace math_expr::rtl::io::file::details
   {
      using ::math_expr::details::char_ptr;
      using ::math_expr::details::char_cptr;

      enum file_mode
      {
         e_error = 0,
         e_read  = 1,
         e_write = 2,
         e_rdwrt = 4
      };

      struct file_descriptor
      {
         file_descriptor(const std::string& fname, const std::string& access)
         : stream_ptr(0)
         , mode(get_file_mode(access))
         , file_name(fname)
         {}

         void*       stream_ptr;
         file_mode   mode;
         std::string file_name;

         bool open()
         {
            if (e_read == mode)
            {
               std::ifstream* stream = new std::ifstream(file_name.c_str(),std::ios::binary);

               if (!(*stream))
               {
                  file_name.clear();
                  delete stream;

                  return false;
               }

               stream_ptr = stream;

               return true;
            }
            else if (e_write == mode)
            {
               std::ofstream* stream = new std::ofstream(file_name.c_str(),std::ios::binary);

               if (!(*stream))
               {
                  file_name.clear();
                  delete stream;

                  return false;
               }

               stream_ptr = stream;

               return true;
            }
            else if (e_rdwrt == mode)
            {
               std::fstream* stream = new std::fstream(file_name.c_str(),std::ios::binary);

               if (!(*stream))
               {
                  file_name.clear();
                  delete stream;

                  return false;
               }

               stream_ptr = stream;

               return true;
            }

            return false;
         }

         template <typename Stream, typename Ptr>
         void close(Ptr& p)
         {
            Stream* stream = reinterpret_cast<Stream*>(p);
            stream->close();
            delete stream;
            p = reinterpret_cast<Ptr>(0);
         }

         bool close()
         {
            switch (mode)
            {
               case e_read  : close<std::ifstream>(stream_ptr);
                              break;

               case e_write : close<std::ofstream>(stream_ptr);
                              break;

               case e_rdwrt : close<std::fstream> (stream_ptr);
                              break;

               default      : return false;
            }

            return true;
         }

         template <typename View>
         bool write(const View& view, const std::size_t amount, const std::size_t offset = 0)
         {
            switch (mode)
            {
               case e_write : reinterpret_cast<std::ofstream*>(stream_ptr)->
                                 write(reinterpret_cast<char_cptr>(view.begin() + offset), amount * sizeof(typename View::value_t));
                              break;

               case e_rdwrt : reinterpret_cast<std::fstream*>(stream_ptr)->
                                 write(reinterpret_cast<char_cptr>(view.begin() + offset) , amount * sizeof(typename View::value_t));
                              break;

               default      : return false;
            }

            return true;
         }

         template <typename View>
         bool read(View& view, const std::size_t amount, const std::size_t offset = 0)
         {
            switch (mode)
            {
               case e_read  : reinterpret_cast<std::ifstream*>(stream_ptr)->
                                 read(reinterpret_cast<char_ptr>(view.begin() + offset), amount * sizeof(typename View::value_t));
                              break;

               case e_rdwrt : reinterpret_cast<std::fstream*>(stream_ptr)->
                                 read(reinterpret_cast<char_ptr>(view.begin() + offset) , amount * sizeof(typename View::value_t));
                              break;

               default      : return false;
            }

            return true;
         }

         bool getline(std::string& s)
         {
            switch (mode)
            {
               case e_read  : return (!!std::getline(*reinterpret_cast<std::ifstream*>(stream_ptr),s));
               case e_rdwrt : return (!!std::getline(*reinterpret_cast<std::fstream* >(stream_ptr),s));
               default      : return false;
            }
         }

         bool eof() const
         {
            switch (mode)
            {
               case e_read  : return reinterpret_cast<std::ifstream*>(stream_ptr)->eof();
               case e_write : return reinterpret_cast<std::ofstream*>(stream_ptr)->eof();
               case e_rdwrt : return reinterpret_cast<std::fstream* >(stream_ptr)->eof();
               default      : return true;
            }
         }

         file_mode get_file_mode(const std::string& access) const
         {
            if (access.empty() || access.size() > 2)
               return e_error;

            std::size_t w_cnt = 0;
            std::size_t r_cnt = 0;

            for (std::size_t i = 0; i < access.size(); ++i)
            {
               switch (std::tolower(access[i]))
               {
                  case 'r' : r_cnt++; break;
                  case 'w' : w_cnt++; break;
                  default  : return e_error;
               }
            }

            if ((0 == r_cnt) && (0 == w_cnt))
               return e_error;
            else if ((r_cnt > 1) || (w_cnt > 1))
               return e_error;
            else if ((1 == r_cnt) && (1 == w_cnt))
               return e_rdwrt;
            else if (1 == r_cnt)
               return e_read;
            else
               return e_write;
         }
      };

      template <typename T>
      file_descriptor* make_handle(T v)
      {
         const std::size_t fd_size    = sizeof(details::file_descriptor*);
         details::file_descriptor* fd = reinterpret_cast<file_descriptor*>(0);

         std::memcpy(reinterpret_cast<char_ptr >(&fd),
                     reinterpret_cast<char_cptr>(&v ),
                     fd_size);
         return fd;
      }

      template <typename T>
      void perform_check()
      {
         #ifdef _MSC_VER
         #pragma warning(push)
         #pragma warning(disable: 4127)
         #endif
         if (sizeof(T) < sizeof(void*))
         {
            throw std::runtime_error("math_expr::rtl::io::file - Error - pointer size larger than holder.");
         }
         #ifdef _MSC_VER
         #pragma warning(pop)
         #endif
         assert(sizeof(T) <= sizeof(void*));
      }

   } // namespace math_expr::rtl::io::file::details

#endif
