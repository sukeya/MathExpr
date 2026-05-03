#include "math_expr.hpp"

namespace
{

constexpr long double kExpectedEpsilon = 0.125L;

template <typename T>
bool matches_expected_epsilon()
{
    return math_expr::core::numeric::details::epsilon_type<T>::value() ==
           static_cast<T>(kExpectedEpsilon);
}

}  // namespace

int main()
{
    using numeric_type = double;
    using expression_t = math_expr::expression<numeric_type>;
    using parser_t = math_expr::parser<numeric_type>;

    static_assert(math_expr::core::build_options::kHasConfiguredEpsilon);
    static_assert(math_expr::core::build_options::kConfiguredEpsilon == kExpectedEpsilon);

    if (!matches_expected_epsilon<float>() || !matches_expected_epsilon<double>() ||
        !matches_expected_epsilon<long double>())
    {
        return 1;
    }

    math_expr::symbol_table<numeric_type> symbol_table;
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    if (!parser.compile("epsilon", expression))
    {
        return 1;
    }

    if (expression.value() != static_cast<numeric_type>(kExpectedEpsilon))
    {
        return 1;
    }

    if (math_expr::core::numeric::equal(numeric_type(1.0), numeric_type(1.1)) != numeric_type(1.0))
    {
        return 1;
    }

    if (math_expr::core::numeric::nequal(numeric_type(1.0), numeric_type(1.1)) != numeric_type(0.0))
    {
        return 1;
    }

    if (math_expr::core::numeric::nequal(numeric_type(1.0), numeric_type(1.3)) != numeric_type(1.0))
    {
        return 1;
    }

    return 0;
}
