# Section 24: Runtime Checks

[Back to index](index.md)

[Previous](23-helpers-and-utils.md) | [Index](index.md) | [Next](25-benchmarking.md)

The ExprTk library provides the ability to perform runtime checks during expression evaluation so as to ensure situations such as memory access violation, compilation stack-overflow and evaluation time limit errors are caught and handled without causing further issues. The checks typically cover:

1. Vector access and handling
1. String access and handling
1. Loop iteration checks
1. Compilation checkpointing
1. Assert statements

## Vector Access Runtime Checks

Expressions that contain vectors where elements of the vectors may be accessed using indexes that can only be determined at runtime may result in memory access violations when the index is out of the vector's bound. Some examples of problematic expressions are as follows:

1. vec[i]
1. vec[i + j]
1. vec[i + 10]
1. vec[i + vec[]] := x + y
1. vec[i + j] <=> vec[i]
1. vec[i + j] := (vec1 + vec2)[i + j]

In the above expressions, it is assumed that the values used in the index operator may either exceed the vector bounds or precede the vector's start, In short, the indexes may not necessarily be within the range [0,vec[]).

ExprTk provides the ability to inject a runtime check at the point of index evaluation and handle situations where the index violates the vector's bounds. This capability is done by registering a user- implemented Vector Access Runtime Check (VARTC) to the parser before expression compilation. Initially a VARTC can be defined as follows:

```cpp
struct my_vector_access_rtc final :
   public math_expr::vector_access_runtime_check
{
   bool handle_runtime_violation(violation_context& context)
   override
   {
      // Handling of the violation
      return ...;
   }
};
```

Then an instance of the VARTC can be registered with a parser instance as follows:

```cpp
my_vector_access_rtc vartc;

math_expr::symbol_table<T> symbol_table;

T i;
T x;
T y;
std::vector<T> vec = { 0, 1, 2, 3, 4 };

symbol_table.add_variable("i"  , i  );
symbol_table.add_variable("x"  , x  );
symbol_table.add_variable("y"  , y  );
symbol_table.add_vector  ("vec", vec);

math_expr::expression<T> expression;
math_expr::parser<T> parser;

parser.register_vector_access_runtime_check(vartc);

std::string expression = "vec[i + vec[]] := x + y";

parser.compile(expression_str, expression);

try
{
   expression.value();
}
catch (std::runtime_error& rte)
{
   printf("Exception: %s\n", rte.what());
}
```

**Note 36.** The lifetime of any parser or expression instance must not exceed that of any VARTC instance that has been registered with it.

When a vector access violation occurs, the registered VARTC instance's handle_runtime_violation method will be invoked, coupled with it a violation_context shall be provided that will contain the following members:

1. base_ptr: Of type void*, which points to the first element of the vector. The base_ptr can also be used as a key to determine the vector upon which the access violation has occurred.

1. end_ptr : Of type void*, which points to one position after the last element of the vector

1. access_ptr: Of type void*, points to the memory location which is the base_ptr offset by the derived index value.

1. type_size: Size of the vector's element type in bytes. This value can be used to determine the number of elements in the vector based on the base_ptr and end_ptr.

The implementation of the handle_runtime_violation method can at this point perform various actions such as:

1. Log the violation
1. Throw an exception (eg: std::runtime_error)
1. Remedy the access_ptr to allow for the evaluation to continue

**Note 37.** When employing option [3], handle_runtime_violation needs to return true, otherwise the caller will assume an unhandled access violation and default to using the base_ptr.

It is recommended, at the very least, to throw an exception when handling vector access violations and to only consider option [3] when the the ramifications of changing the access_ptr are well understood.

The following are simple examples of how the handle_runtime_violation can be implemented.

Example 1: Log the access violation to stdout and then throw a runtime error exception:

```cpp
bool handle_runtime_violation(violation_context& context) override
{
   printf("ERROR -  Runtime vector access violation. "
          "base: %p end: %p access: %p typesize: %lu\n",
          context.base_ptr  ,
          context.end_ptr   ,
          context.access_ptr,
          context.type_size);

   throw std::runtime_error("Runtime vector access violation.");
   return false;
}
```

Example 2: Handle the access violation by resetting the access pointer to the last value in the vector.

```cpp
bool handle_runtime_violation(violation_context& context) override
{
   context.access_ptr =
      static_cast<char*>(context.end_ptr) - context.type_size;
   return true;
}
```

**Note 38.** The return value of true in the above handler method signals the caller to continue the vector access using the updated access_ptr.

## String Access Runtime Checks

Expressions that contain strings where elements or substrings of the strings may be accessed using indexes that can only be determined at runtime may result in memory access violations when the index or range is out of the string's bound. Examples of problematic expressions are as follows:

1. s[i : j + k]
1. s[i : j + k][-x : y]
1. (s1 + s2)[i : j + k]
1. '01234'[5 + i]
1. s += s[i : j + k]
1. s[i : j + k] := 'chappy days'[1 : ]

To enable string access runtime checks all one needs to do is simply use the following define before the ExprTk header is included or as part of the compilation define parameters, or enable the same-named CMake cache variable:

```text
MATH_EXPR_ENABLE_RANGE_RUNTIME_CHECKS
```

When the above define is used, and a string related runtime access violation occurs a std::runtime_error exception will be thrown. The following demonstrates the general flow of handling the access violation:

```cpp
parser.compile(expression_string, expression)
.
.
try
{
   expression.value();
}
catch (std::runtime_error& rte)
{
   printf("Exception: %s\n", rte.what());
}
```

## Loop Iteration Checks

Expressions that contain loop structures (eg: for/while/repeat et al) can be problematic from a usage point of view due to the difficulty in determining the following:

1. Will the loop ever complete (aka is this an infinite loop?)
1. Maximum loop execution time

ExprTk provides the ability to inject a runtime check within loop conditionals, and to have the result of the check either signal the loop to continue or for the check to raise a loop violation error.

The process involves instantiating a user defined loop_runtime_check (LRTC), registering the instance with a math_expr::parser instance and specifying which loop types the check is to performed upon. The following code demonstrates a how custom LRTC can be instantiated and registered with the associated parser:

```cpp
typedef math_expr::parser<T> parser_t;
typedef math_expr::loop_runtime_check loop_runtime_check_t;

my_loop_rtc loop_runtime_check;
loop_runtime_check.loop_set = loop_runtime_check_t::e_all_loops;
loop_runtime_check.max_loop_iterations = 100000;

parser_t parser;

parser.register_loop_runtime_check(loop_runtime_check);
```

The following is an example of how one could derive from and implement a custom loop_runtime_check:

```cpp
struct my_loop_rtc final : math_expr::loop_runtime_check
{
   bool check() override
   {
      //
      return ...
   }

   void handle_runtime_violation
      (const violation_context&) override
   {
      throw std::runtime_error("Loop runtime violation.");
   }
};
```

In the above code, if either the check method returns false or the loop iteration count exceeds the max_loop_iterations value, the handle_runtime_violation method will be invoked, coupled with it a violation_context shall be provided that will contain the following members:

1. loop: Of type loop_types. This value denotes the type of loop that triggered the violation (e_for_loop, e_while_loop, e_repeat_until_loop).

1. violation: Of type type. This value denotes the type of violation (e_iteration_count, e_timeout)

1. iteration_count: Of type uint64_t. The number of iterations that the triggering loop has executed since the start of the expression.

**Note 39.** The lifetime of any parser or expression instance must not exceed that of any LRTC instance that has been registered with it.

The following is an example implementation of an LRTC that supports loop timeout violations:

```cpp
struct timeout_loop_rtc final : math_expr::loop_runtime_check
{
   using time_point_t =
      std::chrono::time_point<std::chrono::steady_clock>;

   std::size_t iterations_ = 0;
   time_point_t timeout_tp_;

   bool check() override
   {
      if (std::chrono::steady_clock::now() >= timeout_tp_)
      {
         // handle_runtime_violation shall be invoked
         return false;
      }

      return true;
   }

   void handle_runtime_violation
      (const math_expr::violation_context&) override
   {
      throw std::runtime_error("Loop timed out");
   }

   void set_timeout_time(const time_point_t& timeout_tp)
   {
      timeout_tp_ = timeout_tp;
   }
};
```

In the above code, the check method shall be invoked on each iteration of the associated loop. Within the method the current time is compared to the setup timeout time-point, in the event the current time exceeds the timeout, the method returns false, triggering the violation, which in turn will result in the handle_runtime_violation being invoked.

The following code demonstrates how the above defined LRTC can be used to ensure that at the very least the loop portion(s) of an expression will never exceed a given amount of execution time.

```cpp
typedef math_expr::parser<T> parser_t;
typedef math_expr::loop_runtime_check loop_runtime_check_t;

my_loop_rtc loop_rtc;
loop_rtc.loop_set = loop_runtime_check_t::e_all_loops;
loop_rtc.max_loop_iterations = 100000;

parser_t parser;

parser.register_loop_runtime_check(loop_rtc);
.
.
.
.
using namespace std::chrono;
const auto max_duration = seconds(25);

try
{
   loop_rtc.set_timeout_time(steady_clock::now() + max_duration);
   expression.value();

   loop_rtc.set_timeout_time(steady_clock::now() + max_duration);
   expression.value();

   loop_rtc.set_timeout_time(steady_clock::now() + max_duration);
   expression.value();

}
catch(std::runtime_error& exception)
{
   printf("Exception: %s\n",exception.what());
}
```

## Compilation Process Checkpointing

When compiling an expression, one may require the compilation process to periodically checkpoint its internal state, subsequently at the checkpoint one can then make the decision to continue the compilation process or to immediately terminate and return.

The following are reasons one may want to checkpoint the compilation process:

1. Determine if the compilation process has run for far too long
1. Determine if the current stack frame size exceeds a limit
1. Enforce an external termination request

ExprTk provides the ability to inject a checkpoint into the compilation process that will be evaluated periodically. This capability is achieved by registering a user-implemented compilation check (CCK) to the parser before expression compilation. Initially a CCK can be defined as follows:

```cpp
struct compilation_timeout_check final :
   public math_expr::compilation_check
{
   bool continue_compilation(compilation_context& context)
   override
   {
      // Determine if compilation should continue
      return ...;
   }
};
```

An example checkpoint use-case could be that we do not want the compilation process to take longer than a maximum defined period, eg: five seconds. The associated compilation check implementation could be as follows:

```cpp
struct my_compilation_timeout_check final :
   public math_expr::compilation_check
{

   bool continue_compilation(compilation_context& context)
   override
   {
      static constexpr std::size_t max_iters_per_check = 1000;

      if (++iterations_ >= max_iters_per_check)
      {
         if (std::chrono::steady_clock::now() >= timeout_tp_)
         {
            context.error_message = "Compilation has timed-out";
            return false;
         }

         iterations_ = 0;
      }

      return true;
   }

   using namespace std::chrono;
   using time_point_t = time_point<steady_clock>;

   void set_timeout_time(const time_point_t& timeout_tp)
   {
      timeout_tp_ = timeout_tp;
   }

   std::size_t iterations_ = 0;
   time_point_t timeout_tp_;
};
```

Usage of the above defined compilation check will require registering the check with the parser, setting up the expiry time and then proceeding to compile the expression. The following is a general outline of what will be needed:

```cpp
typedef math_expr::expression<T> expression_t;
typedef math_expr::parser<T>     parser_t;

expression_t expression;

my_compilation_timeout_check compilation_timeout_check;

parser_t parser;
parser.
   register_compilation_timeout_check(compilation_timeout_check);

using namespace std::chrono;

const auto max_duration = seconds(5);
const auto timeout_tp   = steady_clock::now() + max_duration;

compilation_timeout_check.set_timeout_time(timeout_tp);

if (!parser.compile(large_expression_string, expression))
{
   printf("Error: %s\t\n", parser.error().c_str());
   return;
}
```

## Assert statements

ExprTk supports the use of assert statements to verify pre and post conditions during the evaluation of expressions. The assert statements are only active when a user defined assert handler is registered with the parser before expression compilation, otherwise they are compiled out, this is similar to how asserts are included/excluded in C++ coupled with the definition of NDEBUG. The assert syntax has three variations as described below:

```text
assert(x + y > i);
assert(x + y > i, 'assert statement 1');
assert(x + y > i, 'assert statement 1', 'ASSERT01');
```

The three assert statement input parameters are as follows:

1. assert condition (mandatory)
1. assert message (optional)
1. assert id (optional)

The assert condition is essentially a boolean statement that is expected to be true during evaluation. The other two parameters of assert message and ID are string values that are intended to provide feedback to the handler and to ensure the uniqueness of assert statement respectively. The three parameters denoted above and the offset of the assert statement from the beginning of the expression are placed inside assert_context that is provided as part of the assert_check handler. A user defined assert_check handler can be defined as follows:

```cpp
struct my_assert_handler final : public math_expr::assert_check
{
   void handle_assert(const assert_context& ctxt) override
   {
      printf("condition: [%s] \n", ctxt.condition.c_str());
      printf("message:   [%s] \n", ctxt.message  .c_str());
      printf("id:        [%s] \n", ctxt.id       .c_str());
      printf("offset:    [%lu]\n", ctxt.offet            );
      // throw std::runtime_error(.....);
   }
};
```

Once the assert_check handler has been registered with the parser, expressions that contain assert statements will have their asserts compiled in as part final evaluable expression instance:

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

const std::string program =
   " var x := 4;                             "
   "                                         "
   " for (var i := 0; i < 10; i += 1)        "
   " {                                       "
   "    assert(i < x, 'assert statement 1'); "
   " }                                       ";

my_assert_handler handler;

expression_t expression;
parser_t parser;

parser.register_assert_check(handler);
parser.compile(program, expression);
```

## Runtime Check Overheads

All of the above mentioned runtime checks will incur an execution time overhead during the evaluation of expressions. This is an unfortunate but necessary side-effect of the process when runtime safety is of concern.

A recommendation to consider, that is not demonstrated above, is that in the check method of the LRTC, one should not evaluate the timeout condition on every call to check (aka on every loop iteration). Instead a counter should be maintained and incremented on each call and when the counter exceeds some predefined amount (eg: 10000 iterations), then the timeout based check can be preformed. The reasoning here is that incrementing an integer should be far less expensive than computing the current "now" time-point.

## Runtime Check Limitations

The available RTC mechanisms in ExprTk are limited to implementing said checks only within ExprTk based syntax sections of an expression. The RTCs will not be active within user defined functions, or composited functions that have been compiled with parser instances that don't have the same set of RTC configurations enabled.

## Runtime Handlers

When implementing stateful run-time check handlers one must be careful to ensure the handler is setup correctly or reset between calls to the expression::value or parser::compile methods.

The following example code utilises the compilation timeout RTC and expression loop duration RTC examples from above to demonstrate the need to reset the internal state of the various handlers before compilation and valuation processes are invoked, as not doing so will affect the ability for the next expression in the list to either be correctly compiled or evaluated due to the potential of erroneous timeouts occurring.

```cpp
typedef math_expr::expression<T> expression_t;
typedef math_expr::parser<T>     parser_t;

my_compilation_timeout_check compilation_timeout_check;

my_loop_rtc loop_rtc;
loop_rtc.loop_set = loop_runtime_check_t::e_all_loops;
loop_rtc.max_loop_iterations = 100000;

parser_t parser;
parser.register_loop_runtime_check(loop_rtc);
parser.
   register_compilation_timeout_check(compilation_timeout_check);

using namespace std::chrono;

const auto compile_timeout_tp = []()
{
   const auto max_duration = seconds(5);
   return steady_clock::now() + max_duration;
};

const auto loop_timeout_tp = []()
{
   const auto max_duration = seconds(10);
   return steady_clock::now() + max_duration;
};

const std::vector<std::string> expressions =
{
   "x + y / 2",
   "sin(x) / cos(y) + 1",
   "clamp(-1, sin(2 * pi * x) + cos(y / 2 * pi), +1)"
};

for (const auto& expr_str : expressions)
{
   // Reset the timeout for the compilation RTC
   compilation_timeout_check
      .set_timeout_time(compile_timeout_tp());

   expression_t expression;

   if (!parser.compile(large_expression_string, expression))
   {
      printf("Error: %s\t\n", parser.error().c_str());
      continue;
   }

   try
   {
      // Reset the timeout for the loop duration RTC
      loop_rtc.set_timeout_time(loop_timeout_tp());

      expression.value();
   }
   catch(std::runtime_error& exception)
   {
      printf("Exception: %s\n Expression: %s\n",
             exception.what(),
             expr_str.c_str());
   }
}
```

[Previous](23-helpers-and-utils.md) | [Index](index.md) | [Next](25-benchmarking.md)
