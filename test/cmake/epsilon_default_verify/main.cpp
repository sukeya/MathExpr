#include <limits>

#include "math_expr.hpp"

namespace
{

template <typename T>
bool matches_default_epsilon()
{
    return math_expr::core::numeric::details::epsilon_type<T>::value() ==
           std::numeric_limits<T>::epsilon();
}

}  // namespace

int main()
{
    using numeric_type = double;
    using expression_t = math_expr::expression<numeric_type>;
    using parser_t = math_expr::parser<numeric_type>;

    static_assert(!math_expr::core::build_options::kHasConfiguredEpsilon);
    static_assert(math_expr::core::build_options::kConfiguredEpsilon == 0.0L);

    if (!matches_default_epsilon<float>() || !matches_default_epsilon<double>() ||
        !matches_default_epsilon<long double>())
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

    if (expression.value() != std::numeric_limits<numeric_type>::epsilon())
    {
        return 1;
    }

    const numeric_type near_value =
        numeric_type(1.0) + std::numeric_limits<numeric_type>::epsilon();
    const numeric_type far_value =
        numeric_type(1.0) + (std::numeric_limits<numeric_type>::epsilon() * numeric_type(4.0));

    if (math_expr::core::numeric::equal(numeric_type(1.0), near_value) != numeric_type(1.0))
    {
        return 1;
    }

    if (math_expr::core::numeric::nequal(numeric_type(1.0), far_value) != numeric_type(1.0))
    {
        return 1;
    }

    return 0;
}
