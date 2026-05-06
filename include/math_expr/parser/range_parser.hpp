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

#ifndef MATH_EXPR_PARSER_RANGE_PARSER_HPP
#define MATH_EXPR_PARSER_RANGE_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_range
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;
    using range_t = details::range_pack<T>;

    template <typename Context>
    static inline bool parse_range(Context& ctx, range_t& rp, const bool skip_lsqr = false)
    {
        // Examples of valid ranges:
        // 1. [1:5]     -> [1,5)
        // 2. [ :5]     -> [0,5)
        // 3. [1: ]     -> [1,end)
        // 4. [x:y]     -> [x,y) where x <= y
        // 5. [x+1:y/2] -> [x+1,y/2) where x+1 <= y/2
        // 6. [ :y]     -> [0,y) where 0 <= y
        // 7. [x: ]     -> [x,end) where x <= end

        rp.clear();

        if (!skip_lsqr && !ctx.token_is(token_t::e_lsqrbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR113 - Expected '[' for start of range", core::error_location()));

            return false;
        }

        if (ctx.token_is(token_t::e_colon))
        {
            rp.n0_c.first = true;
            rp.n0_c.second = 0;
            rp.cache.first = 0;
        }
        else
        {
            expression_node_ptr r0 = ctx.parse_expression();

            if (nullptr == r0)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR114 - Failed parse begin section of range", core::error_location()));

                return false;
            }
            else if (details::is_constant_node(r0))
            {
                const T r0_value = r0->value();

                if (r0_value >= T(0))
                {
                    rp.n0_c.first = true;
                    rp.n0_c.second = static_cast<std::size_t>(core::numeric::to_int64(r0_value));
                    rp.cache.first = rp.n0_c.second;
                }

                ctx.free_node(r0);

                if (r0_value < T(0))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR115 - Range lower bound less than zero! Constraint: r0 >= 0",
                        core::error_location()));

                    return false;
                }
            }
            else
            {
                rp.n0_e.first = true;
                rp.n0_e.second = r0;
            }

            if (!ctx.token_is(token_t::e_colon))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR116 - Expected ':' for break  in range", core::error_location()));

                rp.free();
                return false;
            }
        }

        if (ctx.token_is(token_t::e_rsqrbracket))
        {
            rp.n1_c.first = true;
            rp.n1_c.second = std::numeric_limits<std::size_t>::max();
        }
        else
        {
            expression_node_ptr r1 = ctx.parse_expression();

            if (nullptr == r1)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR117 - Failed parse end section of range", core::error_location()));

                rp.free();
                return false;
            }
            else if (details::is_constant_node(r1))
            {
                const T r1_value = r1->value();

                if (r1_value >= T(0))
                {
                    rp.n1_c.first = true;
                    rp.n1_c.second = static_cast<std::size_t>(core::numeric::to_int64(r1_value));
                    rp.cache.second = rp.n1_c.second;
                }

                ctx.free_node(r1);

                if (r1_value < T(0))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR118 - Range upper bound less than zero! Constraint: r1 >= 0",
                        core::error_location()));

                    rp.free();
                    return false;
                }
            }
            else
            {
                rp.n1_e.first = true;
                rp.n1_e.second = r1;
            }

            if (!ctx.token_is(token_t::e_rsqrbracket))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR119 - Expected ']' for start of range", core::error_location()));

                rp.free();
                return false;
            }
        }

        if (rp.const_range())
        {
            std::size_t r0 = 0;
            std::size_t r1 = 0;

            bool rp_result = false;

            try
            {
                rp_result = rp(r0, r1);
            }
            catch (std::runtime_error&)
            {
            }

            if (!rp_result || (r0 > r1))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR120 - Invalid range, Constraint: r0 <= r1", core::error_location()));

                return false;
            }
        }

        return true;
    }
};
}  // namespace math_expr

#endif
