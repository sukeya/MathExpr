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

#ifndef MATH_EXPR_CORE_OPERATOR_TYPES_HPP
#define MATH_EXPR_CORE_OPERATOR_TYPES_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/config.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/core/numeric.hpp"
#include <stdexcept>
#include <type_traits>

namespace math_expr::details
{
      enum class operator_type : int
      {
         default_op , null    , add     , sub     ,
         mul     , div     , mod     , pow     ,
         atan2   , min     , max     , avg     ,
         sum     , prod    , lt      , lte     ,
         eq      , equal   , ne      , nequal  ,
         gte     , gt      , logical_and     , nand    ,
         logical_or      , nor     , logical_xor     , xnor    ,
         mand    , mor     , scand   , scor    ,
         shr     , shl     , abs     , acos    ,
         acosh   , asin    , asinh   , atan    ,
         atanh   , ceil    , cos     , cosh    ,
         exp     , expm1   , floor   , log     ,
         log10   , log2    , log1p   , logn    ,
         neg     , pos     , round   , roundn  ,
         root    , sqrt    , sin     , sinc    ,
         sinh    , sec     , csc     , tan     ,
         tanh    , cot     , clamp   , iclamp  ,
         inrange , sgn     , r2d     , d2r     ,
         d2g     , g2d     , hypot   , notl    ,
         erf     , erfc    , ncdf    , frac    ,
         trunc   , assign  , addass  , subass  ,
         mulass  , divass  , modass  , in      ,
         like    , ilike   , multi   , smulti  ,
         swap    ,

         // Do not add new functions/operators after this point.
         sf00 = 1000, sf01 = 1001, sf02 = 1002, sf03 = 1003,
         sf04 = 1004, sf05 = 1005, sf06 = 1006, sf07 = 1007,
         sf08 = 1008, sf09 = 1009, sf10 = 1010, sf11 = 1011,
         sf12 = 1012, sf13 = 1013, sf14 = 1014, sf15 = 1015,
         sf16 = 1016, sf17 = 1017, sf18 = 1018, sf19 = 1019,
         sf20 = 1020, sf21 = 1021, sf22 = 1022, sf23 = 1023,
         sf24 = 1024, sf25 = 1025, sf26 = 1026, sf27 = 1027,
         sf28 = 1028, sf29 = 1029, sf30 = 1030, sf31 = 1031,
         sf32 = 1032, sf33 = 1033, sf34 = 1034, sf35 = 1035,
         sf36 = 1036, sf37 = 1037, sf38 = 1038, sf39 = 1039,
         sf40 = 1040, sf41 = 1041, sf42 = 1042, sf43 = 1043,
         sf44 = 1044, sf45 = 1045, sf46 = 1046, sf47 = 1047,
         sf48 = 1048, sf49 = 1049, sf50 = 1050, sf51 = 1051,
         sf52 = 1052, sf53 = 1053, sf54 = 1054, sf55 = 1055,
         sf56 = 1056, sf57 = 1057, sf58 = 1058, sf59 = 1059,
         sf60 = 1060, sf61 = 1061, sf62 = 1062, sf63 = 1063,
         sf64 = 1064, sf65 = 1065, sf66 = 1066, sf67 = 1067,
         sf68 = 1068, sf69 = 1069, sf70 = 1070, sf71 = 1071,
         sf72 = 1072, sf73 = 1073, sf74 = 1074, sf75 = 1075,
         sf76 = 1076, sf77 = 1077, sf78 = 1078, sf79 = 1079,
         sf80 = 1080, sf81 = 1081, sf82 = 1082, sf83 = 1083,
         sf84 = 1084, sf85 = 1085, sf86 = 1086, sf87 = 1087,
         sf88 = 1088, sf89 = 1089, sf90 = 1090, sf91 = 1091,
         sf92 = 1092, sf93 = 1093, sf94 = 1094, sf95 = 1095,
         sf96 = 1096, sf97 = 1097, sf98 = 1098, sf99 = 1099,
         sffinal  = 1100,
         sf4ext00 = 2000, sf4ext01 = 2001, sf4ext02 = 2002, sf4ext03 = 2003,
         sf4ext04 = 2004, sf4ext05 = 2005, sf4ext06 = 2006, sf4ext07 = 2007,
         sf4ext08 = 2008, sf4ext09 = 2009, sf4ext10 = 2010, sf4ext11 = 2011,
         sf4ext12 = 2012, sf4ext13 = 2013, sf4ext14 = 2014, sf4ext15 = 2015,
         sf4ext16 = 2016, sf4ext17 = 2017, sf4ext18 = 2018, sf4ext19 = 2019,
         sf4ext20 = 2020, sf4ext21 = 2021, sf4ext22 = 2022, sf4ext23 = 2023,
         sf4ext24 = 2024, sf4ext25 = 2025, sf4ext26 = 2026, sf4ext27 = 2027,
         sf4ext28 = 2028, sf4ext29 = 2029, sf4ext30 = 2030, sf4ext31 = 2031,
         sf4ext32 = 2032, sf4ext33 = 2033, sf4ext34 = 2034, sf4ext35 = 2035,
         sf4ext36 = 2036, sf4ext37 = 2037, sf4ext38 = 2038, sf4ext39 = 2039,
         sf4ext40 = 2040, sf4ext41 = 2041, sf4ext42 = 2042, sf4ext43 = 2043,
         sf4ext44 = 2044, sf4ext45 = 2045, sf4ext46 = 2046, sf4ext47 = 2047,
         sf4ext48 = 2048, sf4ext49 = 2049, sf4ext50 = 2050, sf4ext51 = 2051,
         sf4ext52 = 2052, sf4ext53 = 2053, sf4ext54 = 2054, sf4ext55 = 2055,
         sf4ext56 = 2056, sf4ext57 = 2057, sf4ext58 = 2058, sf4ext59 = 2059,
         sf4ext60 = 2060, sf4ext61 = 2061
      };

      inline std::string to_str(const operator_type opr)
      {
         switch (opr)
         {
            case operator_type::add    : return  "+"  ;
            case operator_type::sub    : return  "-"  ;
            case operator_type::mul    : return  "*"  ;
            case operator_type::div    : return  "/"  ;
            case operator_type::mod    : return  "%"  ;
            case operator_type::pow    : return  "^"  ;
            case operator_type::assign : return ":="  ;
            case operator_type::addass : return "+="  ;
            case operator_type::subass : return "-="  ;
            case operator_type::mulass : return "*="  ;
            case operator_type::divass : return "/="  ;
            case operator_type::modass : return "%="  ;
            case operator_type::lt     : return  "<"  ;
            case operator_type::lte    : return "<="  ;
            case operator_type::eq     : return "=="  ;
            case operator_type::equal  : return  "="  ;
            case operator_type::ne     : return "!="  ;
            case operator_type::nequal : return "<>"  ;
            case operator_type::gte    : return ">="  ;
            case operator_type::gt     : return  ">"  ;
            case operator_type::logical_and    : return "and" ;
            case operator_type::logical_or     : return "or"  ;
            case operator_type::logical_xor    : return "xor" ;
            case operator_type::nand   : return "nand";
            case operator_type::nor    : return "nor" ;
            case operator_type::xnor   : return "xnor";
            default       : return "N/A" ;
         }
      }

      struct base_operation_t
      {
         base_operation_t(const operator_type t, const unsigned int& np)
         : type(t)
         , num_params(np)
         {}

         operator_type type;
         unsigned int num_params;
      };

      struct loop_unroll
      {
         static constexpr unsigned int global_loop_batch_size =
            ::math_expr::config::build_options::kDisableSuperscalarUnroll ?
            4u : 16u;

         explicit loop_unroll(const std::size_t& vsize,
                           const unsigned int loop_batch_size = global_loop_batch_size)
         : batch_size(loop_batch_size   )
         , remainder (vsize % batch_size)
         , upper_bound(static_cast<int>(vsize - remainder))
         {}

         template <class F>
         void foreach_remainder(F&& f) {
            if (remainder < 0) {
               throw std::runtime_error("loop_unroll::foreach() - Invalid remainder");
            }

            if constexpr (!::math_expr::config::build_options::kDisableSuperscalarUnroll)
            {
               switch (remainder)
               {
                  case 15: f();
                  [[fallthrough]];
                  case 14: f();
                  [[fallthrough]];
                  case 13: f();
                  [[fallthrough]];
                  case 12: f();
                  [[fallthrough]];
                  case 11: f();
                  [[fallthrough]];
                  case 10: f();
                  [[fallthrough]];
                  case 9: f();
                  [[fallthrough]];
                  case 8: f();
                  [[fallthrough]];
                  case 7: f();
                  [[fallthrough]];
                  case 6: f();
                  [[fallthrough]];
                  case 5: f();
                  [[fallthrough]];
                  case 4: f();
                  [[fallthrough]];
                  case 3: f();
                  [[fallthrough]];
                  case 2: f();
                  [[fallthrough]];
                  case 1: f();
                     break;
                  case 0:
                     break;
               }
            } else {
               switch (remainder)
               {
                  case 3: f();
                  [[fallthrough]];
                  case 2: f();
                  [[fallthrough]];
                  case 1: f();
                     break;
                  case 0:
                     break;
               }
            }
         }

         unsigned int batch_size;
         int remainder;
         int upper_bound;
      };
      

      inline void dump_ptr(const std::string& s, const void* ptr, const std::size_t size = 0)
      {
         if constexpr (::math_expr::config::build_options::kEnableDebugging)
         {
            if (size)
               math_expr_debug(("%s - addr: %p size: %d\n",
                             s.c_str(),
                             ptr,
                             static_cast<unsigned int>(size)));
            else
               math_expr_debug(("%s - addr: %p\n", s.c_str(), ptr));
         }
      }

      template <typename T>
      inline void dump_vector(const std::string& vec_name, const T* data, const std::size_t size)
      {
         if constexpr (::math_expr::config::build_options::kEnableDebugging)
         {
            std::printf("----- %s (%p) -----\n",
                        vec_name.c_str(),
                        static_cast<const void*>(data));
            std::printf("[ ");
            for (std::size_t i = 0; i <  size; ++i)
            {
               std::printf("%8.3f\t", data[i]);
            }
            std::printf(" ]\n");
            std::printf("---------------------\n");
         }
      }

      template <typename T>
      class vec_data_store
      {
      public:

         typedef vec_data_store<T> type;
         typedef T* data_t;

      private:

         struct control_block
         {
            control_block()
            : ref_count(1)
            , size     (0)
            , data     (0)
            , destruct (true)
            {}

            explicit control_block(const std::size_t& dsize)
            : ref_count(1    )
            , size     (dsize)
            , data     (0    )
            , destruct (true )
            { create_data(); }

            control_block(const std::size_t& dsize, data_t dptr, bool dstrct = false)
            : ref_count(1     )
            , size     (dsize )
            , data     (dptr  )
            , destruct (dstrct)
            {}

           ~control_block()
            {
               if (data && destruct && (0 == ref_count))
               {
                  dump_ptr("~vec_data_store::control_block() data",data);
                  delete[] data;
                  data = reinterpret_cast<data_t>(0);
               }
            }

            static inline control_block* create(const std::size_t& dsize, data_t data_ptr = data_t(0), bool dstrct = false)
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
                  if (
                       (0 !=   cntrl_blck->ref_count) &&
                       (0 == --cntrl_blck->ref_count)
                     )
                  {
                     delete cntrl_blck;
                  }

                  cntrl_blck = 0;
               }
            }

            std::size_t ref_count;
            std::size_t size;
            data_t      data;
            bool        destruct;

         private:

            control_block(const control_block&) = delete;
            control_block& operator=(const control_block&) = delete;

            inline void create_data()
            {
               destruct = true;
               data     = new T[size];
               std::fill_n(data, size, T(0));
               dump_ptr("control_block::create_data() - data", data, size);
            }
         };

      public:

         vec_data_store()
         : control_block_(control_block::create(0))
         {}

         explicit vec_data_store(const std::size_t& size)
         : control_block_(control_block::create(size,reinterpret_cast<data_t>(0),true))
         {}

         vec_data_store(const std::size_t& size, data_t data, bool dstrct = false)
         : control_block_(control_block::create(size, data, dstrct))
         {}

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
            if constexpr (::math_expr::config::build_options::kEnableDebugging)
            {
               math_expr_debug(("size: %d\taddress:%p\tdestruct:%c\n",
                             size(),
                             data(),
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
            const std::size_t size = min_size(vds0.control_block_,vds1.control_block_);
            vds0.control_block_->size = size;
            vds1.control_block_->size = size;
         }

      private:

         static inline std::size_t min_size(const control_block* cb0, const control_block* cb1)
         {
            const std::size_t size0 = cb0->size;
            const std::size_t size1 = cb1->size;

            if (size0 && size1)
               return std::min(size0,size1);
            else
               return (size0) ? size0 : size1;
         }

         control_block* control_block_;
      };

      namespace numeric
      {
         namespace details
         {
            template <typename T>
            inline T process_impl(const operator_type operation, const T arg)
            {
               switch (operation)
               {
                  case operator_type::abs   : return numeric::abs  (arg);
                  case operator_type::acos  : return numeric::acos (arg);
                  case operator_type::acosh : return numeric::acosh(arg);
                  case operator_type::asin  : return numeric::asin (arg);
                  case operator_type::asinh : return numeric::asinh(arg);
                  case operator_type::atan  : return numeric::atan (arg);
                  case operator_type::atanh : return numeric::atanh(arg);
                  case operator_type::ceil  : return numeric::ceil (arg);
                  case operator_type::cos   : return numeric::cos  (arg);
                  case operator_type::cosh  : return numeric::cosh (arg);
                  case operator_type::exp   : return numeric::exp  (arg);
                  case operator_type::expm1 : return numeric::expm1(arg);
                  case operator_type::floor : return numeric::floor(arg);
                  case operator_type::log   : return numeric::log  (arg);
                  case operator_type::log10 : return numeric::log10(arg);
                  case operator_type::log2  : return numeric::log2 (arg);
                  case operator_type::log1p : return numeric::log1p(arg);
                  case operator_type::neg   : return numeric::neg  (arg);
                  case operator_type::pos   : return numeric::pos  (arg);
                  case operator_type::round : return numeric::round(arg);
                  case operator_type::sin   : return numeric::sin  (arg);
                  case operator_type::sinc  : return numeric::sinc (arg);
                  case operator_type::sinh  : return numeric::sinh (arg);
                  case operator_type::sqrt  : return numeric::sqrt (arg);
                  case operator_type::tan   : return numeric::tan  (arg);
                  case operator_type::tanh  : return numeric::tanh (arg);
                  case operator_type::cot   : return numeric::cot  (arg);
                  case operator_type::sec   : return numeric::sec  (arg);
                  case operator_type::csc   : return numeric::csc  (arg);
                  case operator_type::r2d   : return numeric::r2d  (arg);
                  case operator_type::d2r   : return numeric::d2r  (arg);
                  case operator_type::d2g   : return numeric::d2g  (arg);
                  case operator_type::g2d   : return numeric::g2d  (arg);
                  case operator_type::notl  : return numeric::notl (arg);
                  case operator_type::sgn   : return numeric::sgn  (arg);
                  case operator_type::erf   : return numeric::erf  (arg);
                  case operator_type::erfc  : return numeric::erfc (arg);
                  case operator_type::ncdf  : return numeric::ncdf (arg);
                  case operator_type::frac  : return numeric::frac (arg);
                  case operator_type::trunc : return numeric::trunc(arg);

                  default      : math_expr_debug(("numeric::details::process_impl<T> - Invalid unary operation.\n"));
                                 return std::numeric_limits<T>::quiet_NaN();
               }
            }

            template <typename T>
            inline T process_impl(const operator_type operation, const T arg0, const T arg1)
            {
               switch (operation)
               {
                  case operator_type::add    : return (arg0 + arg1);
                  case operator_type::sub    : return (arg0 - arg1);
                  case operator_type::mul    : return (arg0 * arg1);
                  case operator_type::div    : return (arg0 / arg1);
                  case operator_type::mod    : return modulus<T>(arg0,arg1);
                  case operator_type::pow    : return pow<T>(arg0,arg1);
                  case operator_type::atan2  : return atan2<T>(arg0,arg1);
                  case operator_type::min    : return std::min<T>(arg0,arg1);
                  case operator_type::max    : return std::max<T>(arg0,arg1);
                  case operator_type::logn   : return logn<T>(arg0,arg1);
                  case operator_type::lt     : return (arg0 <  arg1) ? T(1) : T(0);
                  case operator_type::lte    : return (arg0 <= arg1) ? T(1) : T(0);
                  case operator_type::eq     : return std::equal_to<T>()(arg0,arg1) ? T(1) : T(0);
                  case operator_type::ne     : return std::not_equal_to<T>()(arg0,arg1) ? T(1) : T(0);
                  case operator_type::gte    : return (arg0 >= arg1) ? T(1) : T(0);
                  case operator_type::gt     : return (arg0 >  arg1) ? T(1) : T(0);
                  case operator_type::logical_and    : return and_opr <T>(arg0,arg1);
                  case operator_type::nand   : return nand_opr<T>(arg0,arg1);
                  case operator_type::logical_or     : return or_opr  <T>(arg0,arg1);
                  case operator_type::nor    : return nor_opr <T>(arg0,arg1);
                  case operator_type::logical_xor    : return xor_opr <T>(arg0,arg1);
                  case operator_type::xnor   : return xnor_opr<T>(arg0,arg1);
                  case operator_type::root   : return root    <T>(arg0,arg1);
                  case operator_type::roundn : return roundn  <T>(arg0,arg1);
                  case operator_type::equal  : return equal   <T>(arg0,arg1);
                  case operator_type::nequal : return nequal  <T>(arg0,arg1);
                  case operator_type::hypot  : return hypot   <T>(arg0,arg1);
                  case operator_type::shr    : return shr     <T>(arg0,arg1);
                  case operator_type::shl    : return shl     <T>(arg0,arg1);

                  default       : math_expr_debug(("numeric::details::process_impl<T> - Invalid binary operation.\n"));
                                  return std::numeric_limits<T>::quiet_NaN();
               }
            }

            template <typename T>
            inline T process_impl(const operator_type operation, const T arg0, const T arg1, int_type_tag)
            {
               switch (operation)
               {
                  case operator_type::add    : return (arg0 + arg1);
                  case operator_type::sub    : return (arg0 - arg1);
                  case operator_type::mul    : return (arg0 * arg1);
                  case operator_type::div    : return (arg0 / arg1);
                  case operator_type::mod    : return arg0 % arg1;
                  case operator_type::pow    : return pow<T>(arg0,arg1);
                  case operator_type::min    : return std::min<T>(arg0,arg1);
                  case operator_type::max    : return std::max<T>(arg0,arg1);
                  case operator_type::logn   : return logn<T>(arg0,arg1);
                  case operator_type::lt     : return (arg0 <  arg1) ? T(1) : T(0);
                  case operator_type::lte    : return (arg0 <= arg1) ? T(1) : T(0);
                  case operator_type::eq     : return (arg0 == arg1) ? T(1) : T(0);
                  case operator_type::ne     : return (arg0 != arg1) ? T(1) : T(0);
                  case operator_type::gte    : return (arg0 >= arg1) ? T(1) : T(0);
                  case operator_type::gt     : return (arg0 >  arg1) ? T(1) : T(0);
                  case operator_type::logical_and    : return ((arg0 != T(0)) && (arg1 != T(0))) ? T(1) : T(0);
                  case operator_type::nand   : return ((arg0 != T(0)) && (arg1 != T(0))) ? T(0) : T(1);
                  case operator_type::logical_or     : return ((arg0 != T(0)) || (arg1 != T(0))) ? T(1) : T(0);
                  case operator_type::nor    : return ((arg0 != T(0)) || (arg1 != T(0))) ? T(0) : T(1);
                  case operator_type::logical_xor    : return arg0 ^ arg1;
                  case operator_type::xnor   : return !(arg0 ^ arg1);
                  case operator_type::root   : return root<T>(arg0,arg1);
                  case operator_type::equal  : return arg0 == arg1;
                  case operator_type::nequal : return arg0 != arg1;
                  case operator_type::hypot  : return hypot<T>(arg0,arg1);
                  case operator_type::shr    : return arg0 >> arg1;
                  case operator_type::shl    : return arg0 << arg1;

                  default       : math_expr_debug(("numeric::details::process_impl<IntType> - Invalid binary operation.\n"));
                                  return std::numeric_limits<T>::quiet_NaN();
               }
            }
         }

         template <typename T>
         inline T process(const operator_type operation, const T arg)
         {
            return math_expr::details::numeric::details::process_impl(operation,arg);
         }

         template <typename T>
         inline T process(const operator_type operation, const T arg0, const T arg1)
         {
            return math_expr::details::numeric::details::process_impl(operation, arg0, arg1);
         }
      }

      template <typename Node>
      struct node_collector_interface
      {
         typedef Node* node_ptr_t;
         typedef Node** node_pp_t;
         typedef std::vector<node_pp_t> noderef_list_t;

         virtual ~node_collector_interface()
         {}

         virtual void collect_nodes(noderef_list_t&)
         {}
      };

      template <typename Node>
      struct node_depth_base;


} // namespace math_expr::details

#endif
