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

#ifndef MATH_EXPR_DETAILS_NODE_MEMORY_ARENA_HPP
#define MATH_EXPR_DETAILS_NODE_MEMORY_ARENA_HPP

#include "math_expr/core/std_includes.hpp"

namespace math_expr::details
{
class node_memory_arena
{
   public:
    explicit node_memory_arena(const std::size_t block_size = 64U * 1024U) : block_size_(block_size)
    {
    }

    node_memory_arena(const node_memory_arena&) = delete;
    node_memory_arena& operator=(const node_memory_arena&) = delete;
    node_memory_arena(node_memory_arena&&) = delete;
    node_memory_arena& operator=(node_memory_arena&&) = delete;

    ~node_memory_arena() = default;

    void* allocate(const std::size_t size, const std::size_t alignment)
    {
        if ((0 == size) || (alignment > alignof(std::max_align_t)))
        {
            return nullptr;
        }

        const std::size_t minimum_block_size = size + alignment;

        if (blocks_.empty() || !blocks_.back().can_fit(size, alignment))
        {
            blocks_.emplace_back(std::max(block_size_, minimum_block_size));
        }

        return blocks_.back().allocate(size, alignment);
    }

   private:
    struct block
    {
        explicit block(const std::size_t size)
            : memory(static_cast<std::byte*>(::operator new(size))), size(size), offset(0)
        {
        }

        block(const block&) = delete;
        block& operator=(const block&) = delete;

        block(block&& other) noexcept : memory(other.memory), size(other.size), offset(other.offset)
        {
            other.memory = nullptr;
            other.size = 0;
            other.offset = 0;
        }

        block& operator=(block&& other) noexcept
        {
            if (this != &other)
            {
                if (memory)
                {
                    ::operator delete(memory);
                }

                memory = other.memory;
                size = other.size;
                offset = other.offset;

                other.memory = nullptr;
                other.size = 0;
                other.offset = 0;
            }

            return *this;
        }

        ~block()
        {
            if (memory)
            {
                ::operator delete(memory);
            }
        }

        bool can_fit(const std::size_t bytes, const std::size_t alignment) const
        {
            if (!memory || (0 == bytes))
            {
                return false;
            }

            const std::uintptr_t begin = reinterpret_cast<std::uintptr_t>(memory + offset);
            const std::uintptr_t aligned =
                (begin + (alignment - 1U)) & ~(static_cast<std::uintptr_t>(alignment) - 1U);
            const std::size_t aligned_offset =
                static_cast<std::size_t>(aligned - reinterpret_cast<std::uintptr_t>(memory));

            return (aligned_offset + bytes) <= size;
        }

        void* allocate(const std::size_t bytes, const std::size_t alignment)
        {
            const std::uintptr_t begin = reinterpret_cast<std::uintptr_t>(memory + offset);
            const std::uintptr_t aligned =
                (begin + (alignment - 1U)) & ~(static_cast<std::uintptr_t>(alignment) - 1U);
            const std::size_t aligned_offset =
                static_cast<std::size_t>(aligned - reinterpret_cast<std::uintptr_t>(memory));

            if ((aligned_offset + bytes) > size)
            {
                return nullptr;
            }

            offset = aligned_offset + bytes;
            return reinterpret_cast<void*>(aligned);
        }

        std::byte* memory;
        std::size_t size;
        std::size_t offset;
    };

    std::size_t block_size_;
    std::deque<block> blocks_;
};

inline thread_local node_memory_arena* g_active_node_memory_arena = nullptr;

class node_memory_arena_scope
{
   public:
    explicit node_memory_arena_scope(node_memory_arena* arena)
        : previous_(g_active_node_memory_arena)
    {
        g_active_node_memory_arena = arena;
    }

    node_memory_arena_scope(const node_memory_arena_scope&) = delete;
    node_memory_arena_scope& operator=(const node_memory_arena_scope&) = delete;

    ~node_memory_arena_scope()
    {
        g_active_node_memory_arena = previous_;
    }

   private:
    node_memory_arena* previous_;
};

inline node_memory_arena* active_node_memory_arena() noexcept
{
    return g_active_node_memory_arena;
}
}  // namespace math_expr::details

#endif
