/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * MathExpr vs Native Benchmarks                              *
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

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>

#include "math_expr.hpp"
#include <benchmark/benchmark.h>

namespace
{

constexpr double kPi = 3.141592653589793238462643383279502;
constexpr double kLowerBoundX = -100.0;
constexpr double kLowerBoundY = -100.0;
constexpr double kUpperBoundX = 100.0;
constexpr double kUpperBoundY = 100.0;
constexpr double kDelta = 0.0111;

template <typename T>
struct native
{
    using functor_t = typename math_expr::core::numeric::functor_t<T>;
    using Type = typename functor_t::Type;

    static inline T clamp(const Type l, const Type v, const Type u)
    {
        return ((v < l) ? l : ((v > u) ? u : v));
    }

    static inline T func00(Type x, Type y)
    {
        return y + x;
    }

    static inline T func01(Type x, Type y)
    {
        return T(2) * (y + x);
    }

    static inline T func02(Type x, Type y)
    {
        return (T(2) * y + T(2) * x);
    }

    static inline T func03(Type x, Type y)
    {
        return ((T(1.23) * (x * x)) / y) - T(123.123);
    }

    static inline T func04(Type x, Type y)
    {
        return (y + x / y) * (x - y / x);
    }

    static inline T func05(Type x, Type y)
    {
        return x / ((x + y) + (x - y)) / y;
    }

    static inline T func06(Type x, Type y)
    {
        return T(1) - ((x * y) + (y / x)) - T(3);
    }

    static inline T func07(Type x, Type y)
    {
        return (T(5.5) + x) + (T(2) * x - T(2) / T(3) * y) * (x / T(3) + y / T(4)) + (y + T(7.7));
    }

    static inline T func08(Type x, Type y)
    {
        using namespace std;
        return (T(1.1) * pow(x, T(1)) + T(2.2) * pow(y, T(2)) - T(3.3) * pow(x, T(3)) +
                T(4.4) * pow(y, T(15)) - T(5.5) * pow(x, T(23)) + T(6.6) * pow(y, T(55)));
    }

    static inline T func09(Type x, Type y)
    {
        return std::sin(T(2) * x) + std::cos(kPi / y);
    }

    static inline T func10(Type x, Type y)
    {
        return T(1) - std::sin(T(2) * x) + std::cos(kPi / y);
    }

    static inline T func11(Type x, Type y)
    {
        return std::sqrt(T(111.111) - std::sin(T(2) * x) + std::cos(kPi / y) / T(333.333));
    }

    static inline T func12(Type x, Type y)
    {
        return ((x * x) / std::sin(T(2) * kPi / y)) - x / T(2);
    }

    static inline T func13(Type x, Type y)
    {
        return x +
               (std::cos(y - std::sin(T(2) / x * kPi)) - std::sin(x - std::cos(T(2) * y / kPi))) -
               y;
    }

    static inline T func14(Type x, Type y)
    {
        return clamp(T(-1), std::sin(T(2) * kPi * x) + std::cos(y / T(2) * kPi), +T(1));
    }

    static inline T func15(Type x, Type y)
    {
        return std::max(
            T(3.33),
            std::min(std::sqrt(T(1) - std::sin(T(2) * x) + std::cos(kPi / y) / T(3)), T(1.11)));
    }

    static inline T func16(Type x, Type y)
    {
        return (((y + (x * T(2.2))) <= (x + y + T(1.1))) ? x - y : x * y) + T(2) * kPi / x;
    }
};

using NativeFunction = double (*)(const double&, const double&);

struct BenchmarkCase
{
    const char* expression;
    NativeFunction native_function;
};

constexpr std::array<BenchmarkCase, 17> kBenchmarkCases{{
    {"(y + x)", native<double>::func00},
    {"2 * (y + x)", native<double>::func01},
    {"(2 * y + 2 * x)", native<double>::func02},
    {"((1.23 * x^2) / y) - 123.123", native<double>::func03},
    {"(y + x / y) * (x - y / x)", native<double>::func04},
    {"x / ((x + y) + (x - y)) / y", native<double>::func05},
    {"1 - ((x * y) + (y / x)) - 3", native<double>::func06},
    {"(5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)", native<double>::func07},
    {"1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55", native<double>::func08},
    {"sin(2 * x) + cos(pi / y)", native<double>::func09},
    {"1 - sin(2 * x) + cos(pi / y)", native<double>::func10},
    {"sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)", native<double>::func11},
    {"(x^2 / sin(2 * pi / y)) - x / 2", native<double>::func12},
    {"x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y", native<double>::func13},
    {"clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)", native<double>::func14},
    {"max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))", native<double>::func15},
    {"if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x", native<double>::func16},
}};

std::int64_t compute_axis_count(double lower, double upper, double delta)
{
    std::int64_t count = 0;

    for (double value = lower; value <= upper; value += delta)
    {
        ++count;
    }

    return count;
}

const std::int64_t kGridEvalCount = compute_axis_count(kLowerBoundX, kUpperBoundX, kDelta) *
                                    compute_axis_count(kLowerBoundY, kUpperBoundY, kDelta);

void benchmark_math_expr(benchmark::State& state, const std::size_t index)
{
    double x = 0.0;
    double y = 0.0;

    math_expr::symbol_table<double> symbol_table;
    symbol_table.add_constants();
    symbol_table.add_variable("x", x);
    symbol_table.add_variable("y", y);

    math_expr::expression<double> expression;
    expression.register_symbol_table(symbol_table);

    math_expr::parser<double> parser;
    if (!parser.compile(kBenchmarkCases[index].expression, expression))
    {
        std::string error = "compile failed: ";
        error += parser.error();
        state.SkipWithError(error.c_str());
        return;
    }

    state.SetLabel(kBenchmarkCases[index].expression);

    for (auto _ : state)
    {
        double total = 0.0;

        for (x = kLowerBoundX; x <= kUpperBoundX; x += kDelta)
        {
            for (y = kLowerBoundY; y <= kUpperBoundY; y += kDelta)
            {
                total += expression.value();
            }
        }

        benchmark::DoNotOptimize(total);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * kGridEvalCount);
}

void benchmark_native(benchmark::State& state, const std::size_t index)
{
    state.SetLabel(kBenchmarkCases[index].expression);

    for (auto _ : state)
    {
        double total = 0.0;

        for (double x = kLowerBoundX; x <= kUpperBoundX; x += kDelta)
        {
            for (double y = kLowerBoundY; y <= kUpperBoundY; y += kDelta)
            {
                total += kBenchmarkCases[index].native_function(x, y);
            }
        }

        benchmark::DoNotOptimize(total);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * kGridEvalCount);
}

void benchmark_parse(benchmark::State& state, const std::size_t index)
{
    double x = 0.0;
    double y = 0.0;

    math_expr::symbol_table<double> symbol_table;
    symbol_table.add_constants();
    symbol_table.add_variable("x", x);
    symbol_table.add_variable("y", y);

    math_expr::parser<double> parser;
    math_expr::expression<double> expression;
    expression.register_symbol_table(symbol_table);

    state.SetLabel(kBenchmarkCases[index].expression);

    for (auto _ : state)
    {
        if (!parser.compile(kBenchmarkCases[index].expression, expression))
        {
            std::string error = "parse failed: ";
            error += parser.error();
            state.SkipWithError(error.c_str());
            return;
        }

        benchmark::DoNotOptimize(expression);
    }

    state.SetItemsProcessed(state.iterations());
}

bool register_benchmarks()
{
    for (std::size_t index = 0; index < kBenchmarkCases.size(); ++index)
    {
        benchmark::RegisterBenchmark(("math_expr/" + std::to_string(index)).c_str(),
                                     [index](benchmark::State& state)
                                     { benchmark_math_expr(state, index); });
        benchmark::RegisterBenchmark(("native/" + std::to_string(index)).c_str(),
                                     [index](benchmark::State& state)
                                     { benchmark_native(state, index); });
        benchmark::RegisterBenchmark(("parse/" + std::to_string(index)).c_str(),
                                     [index](benchmark::State& state)
                                     { benchmark_parse(state, index); });
    }

    return true;
}

[[maybe_unused]] const bool kRegisteredBenchmarks = register_benchmarks();

}  // namespace
