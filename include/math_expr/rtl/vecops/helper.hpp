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

#ifndef MATH_EXPR_RTL_VECOPS_HELPER_HPP
#define MATH_EXPR_RTL_VECOPS_HELPER_HPP

#ifndef math_expr_disable_rtl_vecops
namespace math_expr
{
   namespace rtl { namespace vecops {

   namespace helper
   {
      template <typename Vector>
      inline bool invalid_range(const Vector& v, const std::size_t r0, const std::size_t r1)
      {
         if (r0 > (v.size() - 1))
            return true;
         else if (r1 > (v.size() - 1))
            return true;
         else if (r1 < r0)
            return true;
         else
            return false;
      }

      template <typename T>
      struct load_vector_range
      {
         typedef typename math_expr::igeneric_function<T> igfun_t;
         typedef typename igfun_t::parameter_list_t    parameter_list_t;
         typedef typename igfun_t::generic_type        generic_type;
         typedef typename generic_type::scalar_view    scalar_t;
         typedef typename generic_type::vector_view    vector_t;

         static inline bool process(parameter_list_t& parameters,
                                    std::size_t& r0, std::size_t& r1,
                                    const std::size_t& r0_prmidx,
                                    const std::size_t& r1_prmidx,
                                    const std::size_t vec_idx = 0)
         {
            if (r0_prmidx >= parameters.size())
               return false;

            if (r1_prmidx >= parameters.size())
               return false;

            if (!scalar_t(parameters[r0_prmidx]).to_uint(r0))
               return false;

            if (!scalar_t(parameters[r1_prmidx]).to_uint(r1))
               return false;

            return !invalid_range(vector_t(parameters[vec_idx]), r0, r1);
         }
      };
   }

   namespace details
   {
      template <typename T>
      inline void kahan_sum(T& sum, T& error, const T v)
      {
         const T x = v - error;
         const T y = sum + x;
         error = (y - sum) - x;
         sum = y;
      }

   } // namespace math_expr::rtl::details

   template <typename T>
   class all_true final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      all_true()
      : math_expr::igeneric_function<T>("V|VTT|T*")
        /*
           Overloads:
           0. V   - vector
           1. VTT - vector, r0, r1
           2. T*  - T....T
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         if (2 == ps_index)
         {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
               if (scalar_t(parameters[i])() == T(0))
               {
                  return T(0);
               }
            }
         }
         else
         {
            const vector_t vec(parameters[0]);

            std::size_t r0 = 0;
            std::size_t r1 = vec.size() - 1;

            if (
                 (1 == ps_index) &&
                 !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
               )
            {
               return std::numeric_limits<T>::quiet_NaN();
            }

            for (std::size_t i = r0; i <= r1; ++i)
            {
               if (vec[i] == T(0))
               {
                  return T(0);
               }
            }
         }

         return T(1);
      }
   };

   template <typename T>
   class all_false final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      all_false()
      : math_expr::igeneric_function<T>("V|VTT|T*")
        /*
           Overloads:
           0. V   - vector
           1. VTT - vector, r0, r1
           2. T*  - T....T
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         if (2 == ps_index)
         {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
               if (scalar_t(parameters[i])() != T(0))
               {
                  return T(0);
               }
            }
         }
         else
         {
            const vector_t vec(parameters[0]);

            std::size_t r0 = 0;
            std::size_t r1 = vec.size() - 1;

            if (
                 (1 == ps_index) &&
                 !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
               )
            {
               return std::numeric_limits<T>::quiet_NaN();
            }

            for (std::size_t i = r0; i <= r1; ++i)
            {
               if (vec[i] != T(0))
               {
                  return T(0);
               }
            }
         }

         return T(1);
      }
   };

   template <typename T>
   class any_true final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      any_true()
      : math_expr::igeneric_function<T>("V|VTT|T*")
        /*
           Overloads:
           0. V   - vector
           1. VTT - vector, r0, r1
           2. T*  - T....T
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         if (2 == ps_index)
         {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
               if (scalar_t(parameters[i])() != T(0))
               {
                  return T(1);
               }
            }
         }
         else
         {
            const vector_t vec(parameters[0]);

            std::size_t r0 = 0;
            std::size_t r1 = vec.size() - 1;

            if (
                 (1 == ps_index) &&
                 !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
               )
            {
               return std::numeric_limits<T>::quiet_NaN();
            }

            for (std::size_t i = r0; i <= r1; ++i)
            {
               if (vec[i] != T(0))
               {
                  return T(1);
               }
            }
         }

         return T(0);
      }
   };

   template <typename T>
   class any_false final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      any_false()
      : math_expr::igeneric_function<T>("V|VTT|T*")
        /*
           Overloads:
           0. V   - vector
           1. VTT - vector, r0, r1
           2. T*  - T....T
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         if (2 == ps_index)
         {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
               if (scalar_t(parameters[i])() == T(0))
               {
                  return T(1);
               }
            }
         }
         else
         {
            const vector_t vec(parameters[0]);

            std::size_t r0 = 0;
            std::size_t r1 = vec.size() - 1;

            if (
                 (1 == ps_index) &&
                 !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
               )
            {
               return std::numeric_limits<T>::quiet_NaN();
            }

            for (std::size_t i = r0; i <= r1; ++i)
            {
               if (vec[i] == T(0))
               {
                  return T(1);
               }
            }
         }

         return T(0);
      }
   };

   template <typename T>
   class count final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      count()
      : math_expr::igeneric_function<T>("V|VTT|T*")
        /*
           Overloads:
           0. V   - vector
           1. VTT - vector, r0, r1
           2. T*  - T....T
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         std::size_t cnt = 0;

         if (2 == ps_index)
         {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
               if (scalar_t(parameters[i])() != T(0)) ++cnt;
            }
         }
         else
         {
            const vector_t vec(parameters[0]);

            std::size_t r0 = 0;
            std::size_t r1 = vec.size() - 1;

            if (
                 (1 == ps_index) &&
                 !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
               )
            {
               return std::numeric_limits<T>::quiet_NaN();
            }

            for (std::size_t i = r0; i <= r1; ++i)
            {
               if (vec[i] != T(0)) ++cnt;
            }
         }

         return T(cnt);
      }
   };

   template <typename T>
   class copy final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      copy()
      : math_expr::igeneric_function<T>("VV|VTTVTT")
        /*
           Overloads:
           0. VV     - x(vector), y(vector)
           1. VTTVTT - x(vector), xr0, xr1, y(vector), yr0, yr1,
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[0]);
               vector_t y(parameters[(0 == ps_index) ? 1 : 3]);

         std::size_t xr0 = 0;
         std::size_t xr1 = x.size() - 1;

         std::size_t yr0 = 0;
         std::size_t yr1 = y.size() - 1;

         if (1 == ps_index)
         {
            if (
                 !helper::load_vector_range<T>::process(parameters, xr0, xr1, 1, 2, 0) ||
                 !helper::load_vector_range<T>::process(parameters, yr0, yr1, 4, 5, 3)
               )
               return T(0);
         }

         const std::size_t n = std::min(xr1 - xr0 + 1, yr1 - yr0 + 1);

         std::copy(
            x.begin() + xr0,
            x.begin() + xr0 + n,
            y.begin() + yr0);

         return T(n);
      }
   };

   template <typename T>
   class rol final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      rol()
      : math_expr::igeneric_function<T>("VT|VTTT")
        /*
           Overloads:
           0. VT   - vector, N
           1. VTTT - vector, N, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         std::size_t n  = 0;
         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (!scalar_t(parameters[1]).to_uint(n))
            return T(0);

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0)
            )
            return T(0);

         const std::size_t dist  = r1 - r0 + 1;
         const std::size_t shift = n % dist;

         std::rotate(
            vec.begin() + r0,
            vec.begin() + r0 + shift,
            vec.begin() + r1 + 1);

         return T(1);
      }
   };

   template <typename T>
   class ror final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      ror()
      : math_expr::igeneric_function<T>("VT|VTTT")
        /*
           Overloads:
           0. VT   - vector, N
           1. VTTT - vector, N, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         std::size_t n  = 0;
         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (!scalar_t(parameters[1]).to_uint(n))
            return T(0);

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0)
            )
            return T(0);

         const std::size_t dist  = r1 - r0 + 1;
         const std::size_t shift = (dist - (n % dist)) % dist;

         std::rotate(
            vec.begin() + r0,
            vec.begin() + r0 + shift,
            vec.begin() + r1 + 1);

         return T(1);
      }
   };

   template <typename T>
   class reverse final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      reverse()
      : math_expr::igeneric_function<T>("V|VTT")
        /*
           Overloads:
           0. V   - vector
           1. VTT - vector, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
            return T(0);

         std::reverse(vec.begin() + r0, vec.begin() + r1 + 1);

         return T(1);
      }
   };

   template <typename T>
   class shift_left final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      shift_left()
      : math_expr::igeneric_function<T>("VT|VTTT")
        /*
           Overloads:
           0. VT   - vector, N
           1. VTTT - vector, N, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         std::size_t n  = 0;
         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (!scalar_t(parameters[1]).to_uint(n))
            return T(0);

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0)
            )
            return T(0);

         const std::size_t dist = r1 - r0 + 1;

         if (n > dist)
            return T(0);

         std::rotate(
            vec.begin() + r0,
            vec.begin() + r0 + n,
            vec.begin() + r1 + 1);

         for (std::size_t i = r1 - n + 1ULL; i <= r1; ++i)
         {
            vec[i] = T(0);
         }

         return T(1);
      }
   };

   template <typename T>
   class shift_right final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      shift_right()
      : math_expr::igeneric_function<T>("VT|VTTT")
        /*
           Overloads:
           0. VT   - vector, N
           1. VTTT - vector, N, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         std::size_t n  = 0;
         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (!scalar_t(parameters[1]).to_uint(n))
            return T(0);

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0)
            )
            return T(0);

         const std::size_t dist = r1 - r0 + 1;

         if (n > dist)
            return T(0);

         const std::size_t shift = (dist - (n % dist)) % dist;

         std::rotate(
            vec.begin() + r0,
            vec.begin() + r0 + shift,
            vec.begin() + r1 + 1);

         for (std::size_t i = r0; i < r0 + n; ++i)
         {
            vec[i] = T(0);
         }

         return T(1);
      }
   };

   template <typename T>
   class sort final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      sort()
      : math_expr::igeneric_function<T>("V|VTT|VS|VSTT")
        /*
           Overloads:
           0. V    - vector
           1. VTT  - vector, r0, r1
           2. VS   - vector, string
           3. VSTT - vector, string, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0))
            return T(0);
         if ((3 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0))
            return T(0);

         bool ascending = true;

         if ((2 == ps_index) || (3 == ps_index))
         {
            if (math_expr::details::imatch(to_str(string_t(parameters[1])),"ascending"))
               ascending = true;
            else if (math_expr::details::imatch(to_str(string_t(parameters[1])),"descending"))
               ascending = false;
            else
               return T(0);
         }

         if (ascending)
            std::sort(
               vec.begin() + r0,
               vec.begin() + r1 + 1,
               std::less<T>());
         else
            std::sort(
               vec.begin() + r0,
               vec.begin() + r1 + 1,
               std::greater<T>());

         return T(1);
      }
   };

   template <typename T>
   class nthelement final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      nthelement()
      : math_expr::igeneric_function<T>("VT|VTTT")
        /*
           Overloads:
           0. VT   - vector, nth-element
           1. VTTT - vector, nth-element, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         std::size_t n  = 0;
         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (!scalar_t(parameters[1]).to_uint(n))
            return T(0);

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0))
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         std::nth_element(
            vec.begin() + r0,
            vec.begin() + r0 + n ,
            vec.begin() + r1 + 1);

         return T(1);
      }
   };

   template <typename T>
   class assign final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      assign()
      : math_expr::igeneric_function<T>("VT|VTTT|VTTTT")
        /*
           Overloads:
           0. VT    - vector, V
           1. VTTT  - vector, V, r0, r1
           2. VTTTT - vector, V, r0, r1, SS
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         const T assign_value = scalar_t(parameters[1]);

         const std::size_t step_size = (2 != ps_index) ? 1 :
                                       static_cast<std::size_t>(scalar_t(parameters.back())());

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              ((ps_index == 1) || (ps_index == 2)) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0)
            )
         {
            return T(0);
         }

         for (std::size_t i = r0; i <= r1; i += step_size)
         {
            vec[i] = assign_value;
         }

         return T(1);
      }
   };

   template <typename T>
   class iota final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      iota()
      : math_expr::igeneric_function<T>("VTT|VT|VTTTT|VTTT")
        /*
           Overloads:
           0. VTT  - vector, SV, SS
           1. VT   - vector, SV, SS (+1)
           2. VTTT - vector, r0, r1, SV, SS
           3. VTT  - vector, r0, r1, SV, SS (+1)

           Where:
           1. SV - Start value
           2. SS - Step size
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         const T start_value = (ps_index <= 1) ?
                               scalar_t(parameters[1]) :
                               scalar_t(parameters[3]) ;

         const T step_size = ((0 == ps_index) || (2 == ps_index)) ?
                             scalar_t(parameters.back())() :
                             T(1) ;

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              ((ps_index == 2) || (ps_index == 3)) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
         {
            return T(0);
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            vec[i] = start_value + ((i - r0) * step_size);
         }

         return T(1);
      }
   };

   template <typename T>
   class sumk final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      sumk()
      : math_expr::igeneric_function<T>("V|VTT|VTTT")
        /*
           Overloads:
           0. V    - vector
           1. VTT  - vector, r0, r1
           2. VTTT - vector, r0, r1, stride
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t vec(parameters[0]);

         const std::size_t stride = (2 != ps_index) ? 1 :
                                    static_cast<std::size_t>(scalar_t(parameters[3])());

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              ((1 == ps_index) || (2 == ps_index)) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         T result = T(0);
         T error  = T(0);

         for (std::size_t i = r0; i <= r1; i += stride)
         {
            details::kahan_sum(result, error, vec[i]);
         }

         return result;
      }
   };

   template <typename T>
   class axpy final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpy()
      : math_expr::igeneric_function<T>("TVV|TVVTT")
        /*
           y <- ax + y
           Overloads:
           0. TVV   - a, x(vector), y(vector)
           1. TVVTT - a, x(vector), y(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
               vector_t y(parameters[2]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 3, 4, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();

         for (std::size_t i = r0; i <= r1; ++i)
         {
            y[i] = (a * x[i]) + y[i];
         }

         return T(1);
      }
   };

   template <typename T>
   class axpby final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpby()
      : math_expr::igeneric_function<T>("TVTV|TVTVTT")
        /*
           y <- ax + by
           Overloads:
           0. TVTV   - a, x(vector), b, y(vector)
           1. TVTVTT - a, x(vector), b, y(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
               vector_t y(parameters[3]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 4, 5, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();
         const T b = scalar_t(parameters[2])();

         for (std::size_t i = r0; i <= r1; ++i)
         {
            y[i] = (a * x[i]) + (b * y[i]);
         }

         return T(1);
      }
   };

   template <typename T>
   class axpyz final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpyz()
      : math_expr::igeneric_function<T>("TVVV|TVVVTT")
        /*
           z <- ax + y
           Overloads:
           0. TVVV   - a, x(vector), y(vector), z(vector)
           1. TVVVTT - a, x(vector), y(vector), z(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
         const vector_t y(parameters[2]);
               vector_t z(parameters[3]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 4, 5, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(z, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();

         for (std::size_t i = r0; i <= r1; ++i)
         {
            z[i] = (a * x[i]) + y[i];
         }

         return T(1);
      }
   };

   template <typename T>
   class axpbyz final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpbyz()
      : math_expr::igeneric_function<T>("TVTVV|TVTVVTT")
        /*
           z <- ax + by
           Overloads:
           0. TVTVV   - a, x(vector), b, y(vector), z(vector)
           1. TVTVVTT - a, x(vector), b, y(vector), z(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
         const vector_t y(parameters[3]);
               vector_t z(parameters[4]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 5, 6, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(z, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();
         const T b = scalar_t(parameters[2])();

         for (std::size_t i = r0; i <= r1; ++i)
         {
            z[i] = (a * x[i]) + (b * y[i]);
         }

         return T(1);
      }
   };

   template <typename T>
   class axpbsy final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpbsy()
      : math_expr::igeneric_function<T>("TVTTV|TVTTVTT")
        /*
           y <- ax + by
           Overloads:
           0. TVTVV   - a, x(vector), b, shift, y(vector), z(vector)
           1. TVTVVTT - a, x(vector), b, shift, y(vector), z(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
               vector_t y(parameters[4]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 5, 6, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();
         const T b = scalar_t(parameters[2])();

         const std::size_t s = static_cast<std::size_t>(scalar_t(parameters[3])());

         for (std::size_t i = r0; i <= r1; ++i)
         {
            y[i] = (a * x[i]) + (b * y[i + s]);
         }

         return T(1);
      }
   };

   template <typename T>
   class axpbsyz final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpbsyz()
      : math_expr::igeneric_function<T>("TVTTVV|TVTTVVTT")
        /*
           z <- ax + by
           Overloads:
           0. TVTVV   - a, x(vector), b, shift, y(vector), z(vector)
           1. TVTVVTT - a, x(vector), b, shift, y(vector), z(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
         const vector_t y(parameters[4]);
               vector_t z(parameters[5]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 6, 7, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(z, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();
         const T b = scalar_t(parameters[2])();

         const std::size_t s = static_cast<std::size_t>(scalar_t(parameters[3])());

         for (std::size_t i = r0; i <= r1; ++i)
         {
            z[i] = (a * x[i]) + (b * y[i + s]);
         }

         return T(1);
      }
   };

   template <typename T>
   class axpbz final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpbz()
      : math_expr::igeneric_function<T>("TVTV|TVTVTT")
        /*
           z <- ax + b
           Overloads:
           0. TVTV   - a, x(vector), b, z(vector)
           1. TVTVTT - a, x(vector), b, z(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
               vector_t z(parameters[3]);

         std::size_t r0 = 0;
         std::size_t r1 = x.size() - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 4, 5, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(z, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();
         const T b = scalar_t(parameters[2])();

         for (std::size_t i = r0; i <= r1; ++i)
         {
            z[i] = (a * x[i]) + b;
         }

         return T(1);
      }
   };

   template <typename T>
   class diff final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      diff()
      : math_expr::igeneric_function<T>("VV|VVT")
        /*
           x_(i - stride) - x_i
           Overloads:
           0. VV  - x(vector), y(vector)
           1. VVT - x(vector), y(vector), stride
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[0]);
               vector_t y(parameters[1]);

         const std::size_t r0 = 0;
         const std::size_t r1 = std::min(x.size(),y.size()) - 1;

         const std::size_t stride = (1 != ps_index) ? 1 :
                                    std::min(r1,static_cast<std::size_t>(scalar_t(parameters[2])()));

         for (std::size_t i = 0; i < stride; ++i)
         {
            y[i] = std::numeric_limits<T>::quiet_NaN();
         }

         for (std::size_t i = (r0 + stride); i <= r1; ++i)
         {
            y[i] = x[i] - x[i - stride];
         }

         return T(1);
      }
   };

   template <typename T>
   class dot final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      dot()
      : math_expr::igeneric_function<T>("VV|VVTT")
        /*
           Overloads:
           0. VV   - x(vector), y(vector)
           1. VVTT - x(vector), y(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[0]);
         const vector_t y(parameters[1]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         T result = T(0);

         for (std::size_t i = r0; i <= r1; ++i)
         {
            result += (x[i] * y[i]);
         }

         return result;
      }
   };

   template <typename T>
   class dotk final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      dotk()
      : math_expr::igeneric_function<T>("VV|VVTT")
        /*
           Overloads:
           0. VV   - x(vector), y(vector)
           1. VVTT - x(vector), y(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[0]);
         const vector_t y(parameters[1]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         T result = T(0);
         T error  = T(0);

         for (std::size_t i = r0; i <= r1; ++i)
         {
            details::kahan_sum(result, error, (x[i] * y[i]));
         }

         return result;
      }
   };

   template <typename T>
   class threshold_below final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      threshold_below()
      : math_expr::igeneric_function<T>("VTT|VTTTT")
      /*
         Overloads:
         0. VTT   - vector, TV, SV
         1. VTTTT - vector, r0, r1, TV, SV

         Where:
         TV - Threshold value
         SV - Snap-to value
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         const T threshold_value = (0 == ps_index) ?
                                   scalar_t(parameters[1]) :
                                   scalar_t(parameters[3]) ;

         const T snap_value = scalar_t(parameters.back());

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
         {
            return T(0);
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            if (vec[i] < threshold_value)
            {
               vec[i] = snap_value;
            }
         }

         return T(1);
      }
   };

   template <typename T>
   class threshold_above final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      threshold_above()
      : math_expr::igeneric_function<T>("VTT|VTTTT")
      /*
         Overloads:
         0. VTT   - vector, TV, SV
         1. VTTTT - vector, r0, r1, TV, SV

         Where:
         TV - Threshold value
         SV - Snap-to value
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         const T threshold_value = (0 == ps_index) ?
                                   scalar_t(parameters[1]) :
                                   scalar_t(parameters[3]) ;

         const T snap_value = scalar_t(parameters.back());

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
         {
            return T(0);
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            if (vec[i] > threshold_value)
            {
               vec[i] = snap_value;
            }
         }

         return T(1);
      }
   };

   template <typename T>
   class min_elemwise final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      min_elemwise()
      : math_expr::igeneric_function<T>("VT|VVT|VTTT|VVTTT")
      /*
         Overloads:
         0. VT    - vector, T
         0. VVT   - vector, vector, T
         0. VTTT  - vector, r0, r1, T
         0. VVTTT - vector, vector, r0, r1, T
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         std::size_t out_vec_index = 0;
         std::size_t in_vec_index  = (ps_index & 1) ? 1 : 0;
         std::size_t scalar_index  = parameters.size() - 1;

         vector_t out_vec(parameters[out_vec_index]);
         vector_t in_vec (parameters[in_vec_index ]);

         const T s = scalar_t(parameters[scalar_index ])();

         std::size_t r0 = 0;
         std::size_t r1 = in_vec.size() - 1;

         if ((2 == ps_index) || (3 == ps_index))
         {
            std::size_t rng_idx0 = 0;
            std::size_t rng_idx1 = 0;

            switch (ps_index)
            {
               case 2 : { rng_idx0 = 1; rng_idx1 = 2; }; break;
               case 3 : { rng_idx0 = 2; rng_idx1 = 3; }; break;
            }

            if (!helper::load_vector_range<T>::process(parameters, r0, r1, rng_idx0, rng_idx1, 0))
            {
               return T(0);
            }
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            out_vec[i] = math_expr::details::numeric::min(in_vec[i], s);
         }

         return T(1);
      }
   };

   template <typename T>
   class max_elemwise final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      max_elemwise()
      : math_expr::igeneric_function<T>("VT|VVT|VTTT|VVTTT")
      /*
         Overloads:
         0. VT    - vector, T
         1. VVT   - vector, vector, T
         2. VTTT  - vector, r0, r1, T
         3. VVTTT - vector, vector, r0, r1, T
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         std::size_t out_vec_index = 0;
         std::size_t in_vec_index  = (ps_index & 1) ? 1 : 0;
         std::size_t scalar_index  = parameters.size() - 1;

         vector_t out_vec(parameters[out_vec_index]);
         vector_t in_vec (parameters[in_vec_index ]);

         const T s = scalar_t(parameters[scalar_index ])();

         std::size_t r0 = 0;
         std::size_t r1 = in_vec.size() - 1;

         if ((2 == ps_index) || (3 == ps_index))
         {
            std::size_t rng_idx0 = 0;
            std::size_t rng_idx1 = 0;

            switch (ps_index)
            {
               case 2 : { rng_idx0 = 1; rng_idx1 = 2; }; break;
               case 3 : { rng_idx0 = 2; rng_idx1 = 3; }; break;
            }

            if (!helper::load_vector_range<T>::process(parameters, r0, r1, rng_idx0, rng_idx1, 0))
            {
               return T(0);
            }
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            out_vec[i] = math_expr::details::numeric::max(in_vec[i], s);
         }

         return T(1);
      }
   };

   template <typename T>
   class select final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      select()
      : math_expr::igeneric_function<T>("VVVV|VVVVTT")
      /*
         Overloads:
         0. VVVV  - out vector, mask, vector 0, vector 1
         1. VVVV  - out vector, mask, vector 0, vector 1, r0, r1
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
               vector_t out (parameters[0]);
         const vector_t mask(parameters[1]);
         const vector_t vec0(parameters[2]);
         const vector_t vec1(parameters[3]);

         std::size_t r0 = 0;
         std::size_t r1 = out.size() - 1;

         if (1 == ps_index)
         {
            std::size_t rng_idx0 = 4;
            std::size_t rng_idx1 = 5;

            if (!helper::load_vector_range<T>::process(parameters, r0, r1, rng_idx0, rng_idx1, 0))
            {
               return T(0);
            }

            if (helper::invalid_range(out , r0, r1)) return T(0);
            if (helper::invalid_range(mask, r0, r1)) return T(0);
            if (helper::invalid_range(vec0, r0, r1)) return T(0);
            if (helper::invalid_range(vec1, r0, r1)) return T(0);
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            out[i] = (mask[i] != T(0)) ? vec0[i] : vec1[i];
         }

         return T(1);
      }
   };

   template <typename T>
   struct package
   {
      all_true       <T> at;
      all_false      <T> af;
      any_true       <T> nt;
      any_false      <T> nf;
      count          <T>  c;
      copy           <T> cp;
      rol            <T> rl;
      ror            <T> rr;
      reverse        <T> rev;
      shift_left     <T> sl;
      shift_right    <T> sr;
      sort           <T> st;
      nthelement     <T> ne;
      assign         <T> an;
      iota           <T> ia;
      sumk           <T> sk;
      axpy           <T> b1_axpy;
      axpby          <T> b1_axpby;
      axpyz          <T> b1_axpyz;
      axpbyz         <T> b1_axpbyz;
      axpbsy         <T> b1_axpbsy;
      axpbsyz        <T> b1_axpbsyz;
      axpbz          <T> b1_axpbz;
      diff           <T> df;
      dot            <T> dt;
      dotk           <T> dtk;
      threshold_above<T> ta;
      threshold_below<T> tb;
      min_elemwise<T>    miew;
      max_elemwise<T>    maew;
      select<T>          slct;

      bool register_package(math_expr::symbol_table<T>& symtab)
      {
         #define math_expr_register_function(FunctionName, FunctionType)                 \
         if (!symtab.add_function(FunctionName,FunctionType))                         \
         {                                                                            \
            math_expr_debug((                                                            \
              "math_expr::rtl::vecops::register_package - Failed to add function: %s\n", \
              FunctionName));                                                         \
            return false;                                                             \
         }                                                                            \

         math_expr_register_function("all_true"        , at        )
         math_expr_register_function("all_false"       , af        )
         math_expr_register_function("any_true"        , nt        )
         math_expr_register_function("any_false"       , nf        )
         math_expr_register_function("count"           , c         )
         math_expr_register_function("copy"            , cp        )
         math_expr_register_function("rotate_left"     , rl        )
         math_expr_register_function("rol"             , rl        )
         math_expr_register_function("rotate_right"    , rr        )
         math_expr_register_function("ror"             , rr        )
         math_expr_register_function("reverse"         , rev       )
         math_expr_register_function("shftl"           , sl        )
         math_expr_register_function("shftr"           , sr        )
         math_expr_register_function("sort"            , st        )
         math_expr_register_function("nth_element"     , ne        )
         math_expr_register_function("assign"          , an        )
         math_expr_register_function("iota"            , ia        )
         math_expr_register_function("sumk"            , sk        )
         math_expr_register_function("axpy"            , b1_axpy   )
         math_expr_register_function("axpby"           , b1_axpby  )
         math_expr_register_function("axpyz"           , b1_axpyz  )
         math_expr_register_function("axpbyz"          , b1_axpbyz )
         math_expr_register_function("axpbsy"          , b1_axpbsy )
         math_expr_register_function("axpbsyz"         , b1_axpbsyz)
         math_expr_register_function("axpbz"           , b1_axpbz  )
         math_expr_register_function("diff"            , df        )
         math_expr_register_function("dot"             , dt        )
         math_expr_register_function("dotk"            , dtk       )
         math_expr_register_function("threshold_above" , ta        )
         math_expr_register_function("threshold_below" , tb        )
         math_expr_register_function("min_elemwise"    , miew      )
         math_expr_register_function("max_elemwise"    , maew      )
         math_expr_register_function("select"          , slct      )

         #undef math_expr_register_function

         return true;
      }
   };

   } // namespace math_expr::rtl::vecops
   } // namespace math_expr::rtl
}    // namespace math_expr
#endif

#endif
