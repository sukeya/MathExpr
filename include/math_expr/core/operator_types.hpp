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
#include "math_expr/core/macros.hpp"
#include "math_expr/core/numeric.hpp"

namespace math_expr::details
{
      enum operator_type
      {
         e_default , e_null    , e_add     , e_sub     ,
         e_mul     , e_div     , e_mod     , e_pow     ,
         e_atan2   , e_min     , e_max     , e_avg     ,
         e_sum     , e_prod    , e_lt      , e_lte     ,
         e_eq      , e_equal   , e_ne      , e_nequal  ,
         e_gte     , e_gt      , e_and     , e_nand    ,
         e_or      , e_nor     , e_xor     , e_xnor    ,
         e_mand    , e_mor     , e_scand   , e_scor    ,
         e_shr     , e_shl     , e_abs     , e_acos    ,
         e_acosh   , e_asin    , e_asinh   , e_atan    ,
         e_atanh   , e_ceil    , e_cos     , e_cosh    ,
         e_exp     , e_expm1   , e_floor   , e_log     ,
         e_log10   , e_log2    , e_log1p   , e_logn    ,
         e_neg     , e_pos     , e_round   , e_roundn  ,
         e_root    , e_sqrt    , e_sin     , e_sinc    ,
         e_sinh    , e_sec     , e_csc     , e_tan     ,
         e_tanh    , e_cot     , e_clamp   , e_iclamp  ,
         e_inrange , e_sgn     , e_r2d     , e_d2r     ,
         e_d2g     , e_g2d     , e_hypot   , e_notl    ,
         e_erf     , e_erfc    , e_ncdf    , e_frac    ,
         e_trunc   , e_assign  , e_addass  , e_subass  ,
         e_mulass  , e_divass  , e_modass  , e_in      ,
         e_like    , e_ilike   , e_multi   , e_smulti  ,
         e_swap    ,

         // Do not add new functions/operators after this point.
         e_sf00 = 1000, e_sf01 = 1001, e_sf02 = 1002, e_sf03 = 1003,
         e_sf04 = 1004, e_sf05 = 1005, e_sf06 = 1006, e_sf07 = 1007,
         e_sf08 = 1008, e_sf09 = 1009, e_sf10 = 1010, e_sf11 = 1011,
         e_sf12 = 1012, e_sf13 = 1013, e_sf14 = 1014, e_sf15 = 1015,
         e_sf16 = 1016, e_sf17 = 1017, e_sf18 = 1018, e_sf19 = 1019,
         e_sf20 = 1020, e_sf21 = 1021, e_sf22 = 1022, e_sf23 = 1023,
         e_sf24 = 1024, e_sf25 = 1025, e_sf26 = 1026, e_sf27 = 1027,
         e_sf28 = 1028, e_sf29 = 1029, e_sf30 = 1030, e_sf31 = 1031,
         e_sf32 = 1032, e_sf33 = 1033, e_sf34 = 1034, e_sf35 = 1035,
         e_sf36 = 1036, e_sf37 = 1037, e_sf38 = 1038, e_sf39 = 1039,
         e_sf40 = 1040, e_sf41 = 1041, e_sf42 = 1042, e_sf43 = 1043,
         e_sf44 = 1044, e_sf45 = 1045, e_sf46 = 1046, e_sf47 = 1047,
         e_sf48 = 1048, e_sf49 = 1049, e_sf50 = 1050, e_sf51 = 1051,
         e_sf52 = 1052, e_sf53 = 1053, e_sf54 = 1054, e_sf55 = 1055,
         e_sf56 = 1056, e_sf57 = 1057, e_sf58 = 1058, e_sf59 = 1059,
         e_sf60 = 1060, e_sf61 = 1061, e_sf62 = 1062, e_sf63 = 1063,
         e_sf64 = 1064, e_sf65 = 1065, e_sf66 = 1066, e_sf67 = 1067,
         e_sf68 = 1068, e_sf69 = 1069, e_sf70 = 1070, e_sf71 = 1071,
         e_sf72 = 1072, e_sf73 = 1073, e_sf74 = 1074, e_sf75 = 1075,
         e_sf76 = 1076, e_sf77 = 1077, e_sf78 = 1078, e_sf79 = 1079,
         e_sf80 = 1080, e_sf81 = 1081, e_sf82 = 1082, e_sf83 = 1083,
         e_sf84 = 1084, e_sf85 = 1085, e_sf86 = 1086, e_sf87 = 1087,
         e_sf88 = 1088, e_sf89 = 1089, e_sf90 = 1090, e_sf91 = 1091,
         e_sf92 = 1092, e_sf93 = 1093, e_sf94 = 1094, e_sf95 = 1095,
         e_sf96 = 1096, e_sf97 = 1097, e_sf98 = 1098, e_sf99 = 1099,
         e_sffinal  = 1100,
         e_sf4ext00 = 2000, e_sf4ext01 = 2001, e_sf4ext02 = 2002, e_sf4ext03 = 2003,
         e_sf4ext04 = 2004, e_sf4ext05 = 2005, e_sf4ext06 = 2006, e_sf4ext07 = 2007,
         e_sf4ext08 = 2008, e_sf4ext09 = 2009, e_sf4ext10 = 2010, e_sf4ext11 = 2011,
         e_sf4ext12 = 2012, e_sf4ext13 = 2013, e_sf4ext14 = 2014, e_sf4ext15 = 2015,
         e_sf4ext16 = 2016, e_sf4ext17 = 2017, e_sf4ext18 = 2018, e_sf4ext19 = 2019,
         e_sf4ext20 = 2020, e_sf4ext21 = 2021, e_sf4ext22 = 2022, e_sf4ext23 = 2023,
         e_sf4ext24 = 2024, e_sf4ext25 = 2025, e_sf4ext26 = 2026, e_sf4ext27 = 2027,
         e_sf4ext28 = 2028, e_sf4ext29 = 2029, e_sf4ext30 = 2030, e_sf4ext31 = 2031,
         e_sf4ext32 = 2032, e_sf4ext33 = 2033, e_sf4ext34 = 2034, e_sf4ext35 = 2035,
         e_sf4ext36 = 2036, e_sf4ext37 = 2037, e_sf4ext38 = 2038, e_sf4ext39 = 2039,
         e_sf4ext40 = 2040, e_sf4ext41 = 2041, e_sf4ext42 = 2042, e_sf4ext43 = 2043,
         e_sf4ext44 = 2044, e_sf4ext45 = 2045, e_sf4ext46 = 2046, e_sf4ext47 = 2047,
         e_sf4ext48 = 2048, e_sf4ext49 = 2049, e_sf4ext50 = 2050, e_sf4ext51 = 2051,
         e_sf4ext52 = 2052, e_sf4ext53 = 2053, e_sf4ext54 = 2054, e_sf4ext55 = 2055,
         e_sf4ext56 = 2056, e_sf4ext57 = 2057, e_sf4ext58 = 2058, e_sf4ext59 = 2059,
         e_sf4ext60 = 2060, e_sf4ext61 = 2061
      };

      inline std::string to_str(const operator_type opr)
      {
         switch (opr)
         {
            case e_add    : return  "+"  ;
            case e_sub    : return  "-"  ;
            case e_mul    : return  "*"  ;
            case e_div    : return  "/"  ;
            case e_mod    : return  "%"  ;
            case e_pow    : return  "^"  ;
            case e_assign : return ":="  ;
            case e_addass : return "+="  ;
            case e_subass : return "-="  ;
            case e_mulass : return "*="  ;
            case e_divass : return "/="  ;
            case e_modass : return "%="  ;
            case e_lt     : return  "<"  ;
            case e_lte    : return "<="  ;
            case e_eq     : return "=="  ;
            case e_equal  : return  "="  ;
            case e_ne     : return "!="  ;
            case e_nequal : return "<>"  ;
            case e_gte    : return ">="  ;
            case e_gt     : return  ">"  ;
            case e_and    : return "and" ;
            case e_or     : return "or"  ;
            case e_xor    : return "xor" ;
            case e_nand   : return "nand";
            case e_nor    : return "nor" ;
            case e_xnor   : return "xnor";
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

      namespace loop_unroll
      {
         static constexpr unsigned int global_loop_batch_size =
            ::math_expr::config::build_options::kDisableSuperscalarUnroll ?
            4u : 16u;

         struct details
         {
            explicit details(const std::size_t& vsize,
                             const unsigned int loop_batch_size = global_loop_batch_size)
            : batch_size(loop_batch_size   )
            , remainder (vsize % batch_size)
            , upper_bound(static_cast<int>(vsize - (remainder ? loop_batch_size : 0)))
            {}

            unsigned int batch_size;
            int remainder;
            int upper_bound;
         };
      }

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
                  case e_abs   : return numeric::abs  (arg);
                  case e_acos  : return numeric::acos (arg);
                  case e_acosh : return numeric::acosh(arg);
                  case e_asin  : return numeric::asin (arg);
                  case e_asinh : return numeric::asinh(arg);
                  case e_atan  : return numeric::atan (arg);
                  case e_atanh : return numeric::atanh(arg);
                  case e_ceil  : return numeric::ceil (arg);
                  case e_cos   : return numeric::cos  (arg);
                  case e_cosh  : return numeric::cosh (arg);
                  case e_exp   : return numeric::exp  (arg);
                  case e_expm1 : return numeric::expm1(arg);
                  case e_floor : return numeric::floor(arg);
                  case e_log   : return numeric::log  (arg);
                  case e_log10 : return numeric::log10(arg);
                  case e_log2  : return numeric::log2 (arg);
                  case e_log1p : return numeric::log1p(arg);
                  case e_neg   : return numeric::neg  (arg);
                  case e_pos   : return numeric::pos  (arg);
                  case e_round : return numeric::round(arg);
                  case e_sin   : return numeric::sin  (arg);
                  case e_sinc  : return numeric::sinc (arg);
                  case e_sinh  : return numeric::sinh (arg);
                  case e_sqrt  : return numeric::sqrt (arg);
                  case e_tan   : return numeric::tan  (arg);
                  case e_tanh  : return numeric::tanh (arg);
                  case e_cot   : return numeric::cot  (arg);
                  case e_sec   : return numeric::sec  (arg);
                  case e_csc   : return numeric::csc  (arg);
                  case e_r2d   : return numeric::r2d  (arg);
                  case e_d2r   : return numeric::d2r  (arg);
                  case e_d2g   : return numeric::d2g  (arg);
                  case e_g2d   : return numeric::g2d  (arg);
                  case e_notl  : return numeric::notl (arg);
                  case e_sgn   : return numeric::sgn  (arg);
                  case e_erf   : return numeric::erf  (arg);
                  case e_erfc  : return numeric::erfc (arg);
                  case e_ncdf  : return numeric::ncdf (arg);
                  case e_frac  : return numeric::frac (arg);
                  case e_trunc : return numeric::trunc(arg);

                  default      : math_expr_debug(("numeric::details::process_impl<T> - Invalid unary operation.\n"));
                                 return std::numeric_limits<T>::quiet_NaN();
               }
            }

            template <typename T>
            inline T process_impl(const operator_type operation, const T arg0, const T arg1)
            {
               switch (operation)
               {
                  case e_add    : return (arg0 + arg1);
                  case e_sub    : return (arg0 - arg1);
                  case e_mul    : return (arg0 * arg1);
                  case e_div    : return (arg0 / arg1);
                  case e_mod    : return modulus<T>(arg0,arg1);
                  case e_pow    : return pow<T>(arg0,arg1);
                  case e_atan2  : return atan2<T>(arg0,arg1);
                  case e_min    : return std::min<T>(arg0,arg1);
                  case e_max    : return std::max<T>(arg0,arg1);
                  case e_logn   : return logn<T>(arg0,arg1);
                  case e_lt     : return (arg0 <  arg1) ? T(1) : T(0);
                  case e_lte    : return (arg0 <= arg1) ? T(1) : T(0);
                  case e_eq     : return std::equal_to<T>()(arg0,arg1) ? T(1) : T(0);
                  case e_ne     : return std::not_equal_to<T>()(arg0,arg1) ? T(1) : T(0);
                  case e_gte    : return (arg0 >= arg1) ? T(1) : T(0);
                  case e_gt     : return (arg0 >  arg1) ? T(1) : T(0);
                  case e_and    : return and_opr <T>(arg0,arg1);
                  case e_nand   : return nand_opr<T>(arg0,arg1);
                  case e_or     : return or_opr  <T>(arg0,arg1);
                  case e_nor    : return nor_opr <T>(arg0,arg1);
                  case e_xor    : return xor_opr <T>(arg0,arg1);
                  case e_xnor   : return xnor_opr<T>(arg0,arg1);
                  case e_root   : return root    <T>(arg0,arg1);
                  case e_roundn : return roundn  <T>(arg0,arg1);
                  case e_equal  : return equal   <T>(arg0,arg1);
                  case e_nequal : return nequal  <T>(arg0,arg1);
                  case e_hypot  : return hypot   <T>(arg0,arg1);
                  case e_shr    : return shr     <T>(arg0,arg1);
                  case e_shl    : return shl     <T>(arg0,arg1);

                  default       : math_expr_debug(("numeric::details::process_impl<T> - Invalid binary operation.\n"));
                                  return std::numeric_limits<T>::quiet_NaN();
               }
            }

            template <typename T>
            inline T process_impl(const operator_type operation, const T arg0, const T arg1, int_type_tag)
            {
               switch (operation)
               {
                  case e_add    : return (arg0 + arg1);
                  case e_sub    : return (arg0 - arg1);
                  case e_mul    : return (arg0 * arg1);
                  case e_div    : return (arg0 / arg1);
                  case e_mod    : return arg0 % arg1;
                  case e_pow    : return pow<T>(arg0,arg1);
                  case e_min    : return std::min<T>(arg0,arg1);
                  case e_max    : return std::max<T>(arg0,arg1);
                  case e_logn   : return logn<T>(arg0,arg1);
                  case e_lt     : return (arg0 <  arg1) ? T(1) : T(0);
                  case e_lte    : return (arg0 <= arg1) ? T(1) : T(0);
                  case e_eq     : return (arg0 == arg1) ? T(1) : T(0);
                  case e_ne     : return (arg0 != arg1) ? T(1) : T(0);
                  case e_gte    : return (arg0 >= arg1) ? T(1) : T(0);
                  case e_gt     : return (arg0 >  arg1) ? T(1) : T(0);
                  case e_and    : return ((arg0 != T(0)) && (arg1 != T(0))) ? T(1) : T(0);
                  case e_nand   : return ((arg0 != T(0)) && (arg1 != T(0))) ? T(0) : T(1);
                  case e_or     : return ((arg0 != T(0)) || (arg1 != T(0))) ? T(1) : T(0);
                  case e_nor    : return ((arg0 != T(0)) || (arg1 != T(0))) ? T(0) : T(1);
                  case e_xor    : return arg0 ^ arg1;
                  case e_xnor   : return !(arg0 ^ arg1);
                  case e_root   : return root<T>(arg0,arg1);
                  case e_equal  : return arg0 == arg1;
                  case e_nequal : return arg0 != arg1;
                  case e_hypot  : return hypot<T>(arg0,arg1);
                  case e_shr    : return arg0 >> arg1;
                  case e_shl    : return arg0 << arg1;

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
