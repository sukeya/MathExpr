# Architecture

[Back to index](index.md)

---

## Overview

`MathExpr` is a header-first mathematical expression engine centered on four layers:

1. `symbol_table<T>` and its registries expose external variables, vectors, strings, and functions.
2. `parser<T>` tokenizes and parses source text, performs local folding and strength reduction, and builds an expression tree.
3. `expression<T>` owns the compiled result, including parser-owned storage, runtime result state, and an optional compact hot-path tree.
4. Evaluation uses either a compact `hot_expression_tree<T>` or the legacy virtual AST rooted at `expression_node<T>`.

The current architecture is transitional by design:

- The public AST model is still the legacy virtual `expression_node<T>` hierarchy.
- The hot evaluation path has begun migrating to a compact contiguous representation.
- Parser-time node allocation is already arena-backed to reduce heap churn.

---

## End-to-End Pipeline

```text
Source Code (std::string_view / std::string)
    |
    v
lexer::generator
    |
    v
parser<T>
  - precedence handling via expression_table
  - parser helpers split under include/math_expr/parser/
  - local folding / strength reduction
  - legacy AST construction through node_allocator
    |
    v
expression<T>::control_block
  - root: expression_node<T>*
  - node arena ownership
  - local owned runtime data
  - optional hot_expression_tree<T>
    |
    v
expression<T>::value()
  - use hot_expression_tree<T> if available
  - otherwise fall back to node_variant_adapter / legacy AST
    |
    v
Evaluated Result (T)
```

---

## Main Subsystems

### 1. Symbol and Function Registration

The public entry point is still `symbol_table<T>`, but its storage responsibilities are now split:

- `variable_registry<T>`
  Stores scalar variables, vectors, and strings plus their node wrappers and reverse lookups.
- `function_registry<T>`
  Stores fixed-arity, vararg, generic, string, and overload function registrations.
- `symbol_table<T>`
  Thin facade that preserves the public API and coordinates both registries.

`symbol_table<T>` also keeps the internal `type_store<..., Owned>` helpers used to map names to wrapped runtime entities. The current ownership model is explicit:

- owning stores use `std::unique_ptr`
- non-owning stores keep caller-owned references only

Relevant headers:

- `include/math_expr/symbol_table.hpp`
- `include/math_expr/variable_registry.hpp`
- `include/math_expr/function_registry.hpp`

### 2. Parsing

`parser<T>` remains the orchestration class, but much of its logic has been split into focused helper headers under `include/math_expr/parser/`.

Core support modules:

- `scope_manager.hpp`
  Scope objects and scope-exit cleanup.
- `expression_table.hpp`
  Operator precedence / dispatch metadata.
- `fold_passes.hpp`
  Constant folding and simplification helpers.
- `rtl_wiring.hpp`
  Builtin operator and RTL registration wiring.
- `parser_state.hpp`
  Parser-local mutable state.
- `symtab_store.hpp`
  Lookup adapter over one or more `symbol_table<T>` instances.

Parse-domain helpers:

- `branch_parser.hpp`
- `control_flow_parser.hpp`
- `definition_parser.hpp`
- `dynamic_function_parser.hpp`
- `entity_parser.hpp`
- `function_call_parser.hpp`
- `range_parser.hpp`
- `sequence_parser.hpp`
- `special_case_parser.hpp`
- `statement_parser.hpp`
- `string_range_parser.hpp`
- `switch_parser.hpp`
- `symbol_parser.hpp`
- `symbol_resolution_parser.hpp`
- `vararg_parser.hpp`
- `vector_definition_parser.hpp`
- `vector_index_parser.hpp`

`parser<T>` owns the high-level compile flow, error reporting, synthesis policy, and the `expression_generator` that still emits legacy AST nodes.

### 3. Legacy AST

The primary runtime tree representation is still the virtual `expression_node<T>` hierarchy.

Key families include:

- literals and nulls
- scalar variables
- strings and string ranges
- vectors and vector element access
- unary / binary / trinary / quaternary operator nodes
- control-flow nodes
- fixed-arity / vararg / generic function nodes
- many specialized strength-reduction nodes such as `uv`, `vov`, `cov`, `voc`, `cob`, `boc`, `uvouv`, `T0oT1oT2*`, and `T0oT1oT2oT3*`

This hierarchy remains important because:

- parser synthesis still produces it directly
- many node families are not yet materialized into the compact hot tree
- public and internal tooling still rely on `expression_node<T>*`, `branch()`, `collect_nodes()`, and `node_depth()`

Relevant headers:

- `include/math_expr/details/expression_node.hpp`
- `include/math_expr/details/literal_nodes.hpp`
- `include/math_expr/details/operator_nodes.hpp`
- `include/math_expr/details/function_nodes.hpp`
- `include/math_expr/details/conditional_nodes.hpp`
- `include/math_expr/details/loop_nodes.hpp`
- `include/math_expr/details/vector_nodes.hpp`
- `include/math_expr/details/string_nodes/`

### 4. Variant-Based Migration Layer

`node_variant_adapter<T>` is the main compatibility seam between the legacy AST and the newer compact evaluation path.

It provides:

- root-node classification for `expression_helper<T>`
- typed views for literal / variable / string / null categories
- hot-path classification via `classify_hot(...)`
- helper accessors for branch/base/release/value/depth operations

The hot-set is intentionally small relative to the full node family count. It currently includes:

- literal and variable nodes
- generic unary / binary / trinary nodes
- `sf3` / `sf4`
- fixed-arity `function_N_node`
- `conditional`
- `scand` / `scor` / `null==`
- `uv`, `ipow` / `bipow`, unary-branch
- `vov`, `cov`, `voc`, `vob`, `bov`, `cob`, `boc`
- `uvouv`
- type-erased `T0oT1oT2*` and `T0oT1oT2oT3*` specialized synthesis nodes
- `vararg_multi` — multi-statement sequence blocks `(a; b; c)` that evaluate each expression and return the last result
- `vec_celem` — constant-index vector element read `v[0]`, `v[1]`, …
- `vec_elem` — dynamic-index vector element read `v[i]`
- `swap_node` — variable swap `swap(x, y)` returning the new value of the second variable
- `vec_elem_rtc` / `vec_celem_rtc` — runtime-bounds-checked dynamic and constant vector element access
- `rbvec_elem` / `rbvec_celem` — rebase vector element access (base pointer read from holder at evaluation time)
- `rbvec_elem_rtc` / `rbvec_celem_rtc` — runtime-bounds-checked rebase vector element access
- `null` nodes — baked to NaN literal at tree-build time (zero-cost at evaluation)
- `vec_size` — vector size query (reads `holder->size()` at evaluation time without baking)
- `assignment_node` — scalar variable assignment (`v = rhs`)
- `assignment_vec_elem_node` — dynamic-index vector element assignment (`v[i] = rhs`)
- `assignment_rebasevec_elem_node` / `assignment_rebasevec_celem_node` — rebase vector element assignment
- `assignment_op_node` — scalar compound assignment (`v += rhs`, `v -= rhs`, `v *= rhs`, `v /= rhs`, `v %= rhs`)
- `assignment_vec_elem_op_node` — vector element compound assignment (`v[i] += rhs`, etc.)
- `assignment_vec_elem_rtc_node` — RTC-checked dynamic-index vector element assignment (`v[i] = rhs` with bounds check)
- `assignment_rebasevec_elem_rtc_node` — RTC-checked rebase vector element assignment (dynamic base + dynamic index + bounds check)
- `assignment_rebasevec_elem_op_node` — rebase vector element compound assignment (`rb[i] += rhs`, etc.)
- `assignment_rebasevec_celem_op_node` — rebase vector constant-index compound assignment (`rb[k] += rhs`, etc.)
- `assignment_vec_elem_op_rtc_node` — RTC-checked dynamic-index vector element compound assignment
- `assignment_vec_celem_op_rtc_node` — RTC-checked constant-index vector element compound assignment
- `assignment_rebasevec_elem_op_rtc_node` — RTC-checked rebase vector element compound assignment
- `assignment_rebasevec_celem_op_rtc_node` — RTC-checked rebase vector constant-index compound assignment
- `assignment_vec_node` — whole-vector scalar fill (`v := scalar`)
- `assignment_vec_op_node` — whole-vector scalar compound fill (`v += scalar`, etc.)
- `while_loop_node` / `while_loop_rtc_node` — while-loop with and without RTC iteration guard (break/continue variants remain fallback)
- `repeat_until_loop_node` / `repeat_until_loop_rtc_node` — repeat-until-loop with and without RTC guard
- `for_loop_node` / `for_loop_rtc_node` — for-loop with and without RTC guard
- `switch_node` — switch/case chain (base type only; `switch_n_node` variants remain fallback)
- `multi_switch_node` — multi-match switch (evaluates all matching cases)
- `assignment_vecvec_node` — whole-vector copy (`v0 := v1`) for the non-ivec path
- `assignment_vecvec_op_node` — element-wise compound assignment (`v0 += v1`, `v0 -= v1`, etc.)
- `vector_initialisation_node` — general-case element-wise and single-value vector initialization
- `vector_init_zero_value_node` — zero-fill vector initialisation
- `vector_init_single_constvalue_node` — constant-fill vector initialisation
- `vector_init_single_value_node` — dynamic single-value vector fill
- `vector_init_iota_constconst_node` — iota init with baked base and increment
- `vector_init_iota_constnconst_node` — iota init with baked base, dynamic increment
- `vector_init_iota_nconstconst_node` — iota init with dynamic base, baked increment
- `vector_init_iota_nconstnconst_node` — iota init with both dynamic base and increment
- `vec_binop_vecvec_node` — element-wise binary vector operation `out[i] = op(v0[i], v1[i])` for non-rebaseable operands
- `vec_binop_vecval_node` — element-wise vector-scalar binary operation `out[i] = op(v[i], scalar)` for non-rebaseable vectors
- `vec_binop_valvec_node` — element-wise scalar-vector binary operation `out[i] = op(scalar, v[i])` for non-rebaseable vectors
- `unary_vector_node` — element-wise unary vector operation `out[i] = op(v[i])` for non-rebaseable vectors
- all `switch_n_node<Switch_N>` variants — handled identically to `switch_node` via the shared `arg_list()` accessor from the base class; the typeid restriction has been removed
- `vararg_node` (RTL vararg: sum, avg, min, max, prod, mand, mor) — children hot-evaluated; result computed via `VarArgFunction::process(std::vector<T>)` through the new `vararg_evaluable_node<T>` base
- `vararg_function_node` (user-defined vararg) — children hot-evaluated; result dispatched via `ivararg_function<T>::operator()(const std::vector<T>&)` through the same base

Anything outside this set is represented as `fallback_view` and continues to use the legacy node path via `fallback_subtree_data`. Notable remaining exclusions: string nodes; generic function nodes; `assignment_vecvec_node` (src_is_ivec_ path); `return_node`; break/continue loop variants (which use C++ exceptions for propagation); and rebaseable-vector variants of vector arithmetic nodes.

Relevant header:

- `include/math_expr/details/node_variant_adapter.hpp`

### 5. Compact Hot Evaluation

`hot_expression_tree<T>` is the compact runtime representation used for supported hot roots.

Design goals:

- keep hot nodes contiguous in `std::vector`
- avoid repeated tree-shape virtual dispatch on evaluation
- allow partial migration by embedding unsupported subtrees as fallback leaves

`hot_expression_tree<T>::try_build(root)` succeeds for any non-null root. Unsupported subtrees are stored as `fallback_subtree_data` leaves that call back into the legacy virtual AST. `expression<T>::control_block` always stores the result and `expression<T>::value()` always uses it.

Important properties:

- it is not yet a full AST replacement
- it can mix compact nodes with fallback subtree leaves
- it short-circuits logical nodes directly
- it evaluates supported specialized synthesis nodes without going back through the full legacy dispatch path
- it is always built for any valid expression; the old compact_node_count_ guard that prevented all-fallback trees has been removed

Relevant header:

- `include/math_expr/details/hot_expression_tree.hpp`

### 6. Parser-Time Memory Management

Parser-side node creation is no longer ordinary per-node heap allocation.

`node_allocator` now creates nodes inside a per-compilation `node_memory_arena`, activated through a thread-local scope. Each node still has the same public shape, but parser-time allocation and destruction are cheaper because:

- construction happens from arena blocks
- parse-time cleanup stops paying per-node `delete`
- successful compilation transfers arena ownership into `expression<T>::control_block`

This means the project currently uses:

- legacy virtual nodes for structure
- arena allocation for parser-time ownership
- compact trees for selected evaluation hot paths

Relevant headers:

- `include/math_expr/details/node_memory_arena.hpp`
- `include/math_expr/details/node_utils.hpp`

### 7. Expression Ownership Model

`expression<T>` is the compiled artifact exposed to users.

Its `control_block` currently owns:

- `expression_node<T>* expr`
  The legacy AST root.
- `std::shared_ptr<node_memory_arena> node_arena`
  Parser-side arena backing the legacy AST.
- `std::unique_ptr<hot_expression_tree<T>> hot_tree`
  Optional compact representation for supported hot roots.
- `local_data_list`
  Owned runtime data packs for nodes, vectors, strings, and temporaries.
- `std::unique_ptr<results_context<T>> results`
  Return / result state.

`expression<T>` instances are cheap to copy because they share the `control_block`.

Relevant header:

- `include/math_expr/expression.hpp`

---

## Current Evaluation Strategy

### Uniform Hot Path

`expression<T>::value()` always evaluates through `control_block.hot_tree`.

- `hot_tree` is always built for any valid (non-null root) expression.
- Supported node families evaluate without virtual dispatch via `std::visit` on `node_data_t`.
- Unsupported subtrees are stored as `fallback_subtree_data` leaves that delegate back to `node_variant_adapter<T>::value(node)`, which calls the virtual `->value()` as a last resort.

### Fallback Path (for unsupported subtree leaves)

When a `fallback_subtree_data` leaf is evaluated:

- `node_variant_adapter<T>::value(node)` is called, which may itself recurse through hot dispatch for supported child nodes, or call `node->value()` directly for fully unsupported nodes.

This design eliminates the separate legacy code path in `expression<T>::value()` while keeping correctness for the remaining unsupported node families.

---

## Directory Map

```text
include/math_expr/
├── core/                         low-level numeric, operator, string, and type utilities
├── details/                      runtime nodes, allocators, adapters, compact hot tree
│   ├── expression_node.hpp
│   ├── node_memory_arena.hpp
│   ├── node_variant_adapter.hpp
│   ├── hot_expression_tree.hpp
│   ├── node_utils.hpp
│   ├── function_nodes.hpp
│   ├── operator_nodes.hpp
│   ├── conditional_nodes.hpp
│   ├── loop_nodes.hpp
│   ├── vector_nodes.hpp
│   └── string_nodes/
├── lexer/                        token generation and scanners
├── parser/                       split parser helper modules
├── rtl/                          runtime library packages
├── expression.hpp                compiled expression facade
├── expression_helper.hpp         node classification helper API
├── symbol_table.hpp              public symbol facade
├── variable_registry.hpp         variable/vector/string registry
├── function_registry.hpp         function registry
├── ifunction.hpp                 fixed-arity function interface
├── ivararg_function.hpp          vararg function interface
├── igeneric_function.hpp         generic function interface
└── math_expr.hpp                 top-level public include
```

---

## Design Notes

### Why the AST is still virtual

The full node family is large and deeply specialized. Replacing the whole runtime structure in one step would be high-risk and would destabilize parser synthesis, control-flow nodes, string/vector nodes, and ownership rules all at once.

The current architecture therefore uses staged migration:

1. keep the legacy AST as the source of truth
2. classify stable hot families with `node_variant_adapter`
3. materialize compact hot roots in `hot_expression_tree`
4. gradually reduce fallback coverage

### Why arena allocation matters

Even before full AST replacement, parser-time performance improves materially when temporary and final nodes stop using per-node heap allocation. This fits the project's HPC constraints better than a documentation-only “virtual vs variant” story.

### What is still transitional

The following are still not fully replaced:

- the full `expression_node<T>` hierarchy
- parser-side direct legacy AST synthesis
- fallback evaluation (via `fallback_subtree_data`) for generic / vararg function nodes, string nodes, `assignment_vecvec_op_node`, `assignment_vecvec_node` (src_is_ivec_ path), `vector_initialisation_node` (general-case), `switch_n_node` variants, `return_node`, and break/continue loop variants
- tree traversal APIs such as `collect_nodes()` as the universal ownership mechanism

---

## Practical Mental Model

When reading the current code, the safest model is:

- `symbol_table<T>` is the public registration facade
- `parser<T>` still builds legacy nodes
- node allocation is arena-backed
- `expression<T>` owns both the legacy root and the compact hot tree (always present for valid expressions)
- evaluation always goes through `hot_expression_tree`; unsupported subtrees fall back to virtual dispatch via `fallback_subtree_data` leaves

That model matches the codebase today more closely than a “fully virtual AST” description or a “fully variant AST” description.
