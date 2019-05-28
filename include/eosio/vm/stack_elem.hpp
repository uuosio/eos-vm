#pragma once

#include <eosio/vm/types.hpp>
#include <eosio/vm/variant.hpp>

#include <cstdint>
#include <string>
#include <variant>

namespace eosio { namespace vm {
   using stack_elem =
         variant<activation_frame, i32_const_t, i64_const_t, f32_const_t, f64_const_t, block_t, loop_t, if__t, else__t>;

   inline constexpr int32_t  to_i32(const stack_elem& elem) { return elem.get<i32_const_t>().data.i; }
   inline constexpr uint32_t to_ui32(const stack_elem& elem) { return elem.get<i32_const_t>().data.ui; }
   inline constexpr float    to_f32(const stack_elem& elem) { return elem.get<f32_const_t>().data.f; }
   inline constexpr uint32_t to_fui32(const stack_elem& elem) { return elem.get<f32_const_t>().data.ui; }

   inline constexpr int32_t&  to_i32(stack_elem& elem) { return elem.get<i32_const_t>().data.i; }
   inline constexpr uint32_t& to_ui32(stack_elem& elem) { return elem.get<i32_const_t>().data.ui; }
   inline constexpr float&    to_f32(stack_elem& elem) { return elem.get<f32_const_t>().data.f; }
   inline constexpr uint32_t& to_fui32(stack_elem& elem) { return elem.get<f32_const_t>().data.ui; }

   inline constexpr int64_t  to_i64(const stack_elem& elem) { return elem.get<i64_const_t>().data.i; }
   inline constexpr uint64_t to_ui64(const stack_elem& elem) { return elem.get<i64_const_t>().data.ui; }
   inline constexpr double   to_f64(const stack_elem& elem) { return elem.get<f64_const_t>().data.f; }
   inline constexpr uint64_t to_fui64(const stack_elem& elem) { return elem.get<f64_const_t>().data.ui; }

   inline constexpr int64_t&  to_i64(stack_elem& elem) { return elem.get<i64_const_t>().data.i; }
   inline constexpr uint64_t& to_ui64(stack_elem& elem) { return elem.get<i64_const_t>().data.ui; }
   inline constexpr double&   to_f64(stack_elem& elem) { return elem.get<f64_const_t>().data.f; }
   inline constexpr uint64_t& to_fui64(stack_elem& elem) { return elem.get<f64_const_t>().data.ui; }

}} // namespace eosio::vm
