/**
 * @file
 * @copyright defined in eos-vm/LICENSE
 */
#pragma once

// eos-vm headers
#include <eosio/vm/constants.hpp>
#include <eosio/vm/exceptions.hpp>
#include <eosio/vm/outcome.hpp>

// stl headers
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

// libc headers
#include <setjmp.h>
#include <signal.h>

// aux headers
#include <sys/mman.h>

namespace eosio { namespace vm {

   /// \group memory_allocators Allocator Utils
   /// Type to house global registration of allocators for signal handling purposes.
   struct allocator_registration {
      using allocator_memory_range = std::tuple<uintptr_t, size_t, jmp_buf>;
      struct allocator_memory_range_compare {
         bool operator()(const allocator_memory_range& lhs, const allocator_memory_range& rhs) const {
            return ((std::get<0>(lhs) == std::get<0>(rhs)) && (std::get<1>(lhs) == std::get<1>(rhs)));
         }
      };
      struct allocator_memory_range_hasher {
         bool operator()(const allocator_memory_range& amr) const {
            return std::hash<uintptr_t>()(std::get<0>(amr)) ^ std::hash<size_t>()(std::get<1>(amr));
         }
      };
      static std::unordered_set<allocator_memory_range, allocator_memory_range_hasher, allocator_memory_range_compare>
            allocators_regions;

      template <typename Allocator>
      static void register_allocator(Allocator&& allocator) {
         allocators_regions.emplace(allocator.get_base_address(), allocator.get_max_size(), jmp_buf{});
      }

      template <typename Allocator>
      static void unregister_allocator(Allocator&& allocator) {
         allocators_regions.erase({ allocator.get_base_address(), allocator.get_max_size(), jmp_buf{} });
      }

      static allocator_memory_range* get(uintptr_t address) {
         for (auto& amr : allocators_regions)
            if (std::get<0>(amr) <= address && address < (std::get<0>(amr) + std::get<1>(amr)))
               return (allocator_memory_range*)&amr;
         return nullptr;
      }
   };

   [[noreturn]] void default_wasm_segfault_handler(int sig, siginfo_t* siginfo, void*) {
      if (auto* amr = allocator_registration::get((uintptr_t)siginfo->si_addr))
         longjmp(std::get<2>(*amr), 1);
      else
         raise(sig);
      throw;
   }

   outcome::result<result_void> setup_signal_handler() {
      struct sigaction sa;
      sa.sa_sigaction = &default_wasm_segfault_handler;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_NODEFER | SA_SIGINFO;
      sigaction(SIGSEGV, &sa, NULL);
      sigaction(SIGBUS, &sa, NULL);
      return result_void{};
   }

   class bounded_allocator {
    public:
      static outcome::result<bounded_allocator> init(size_t sz) {
         bounded_allocator ba(sz);
         if (LIKELY(ba._valid))
            return std::move(ba);
         return system_errors::constructor_failure;
      }

      template <typename T>
      outcome::result<T*> alloc(size_t size = 1) {
         EOS_VM_ASSERT((sizeof(T) * size) + _index <= _size, memory_errors::bad_alloc);

         T* ret = (T*)(_raw.get() + _index);
         _index += sizeof(T) * size;
         return ret;
      }

      outcome::result<result_void> free() {
         EOS_VM_ASSERT(_index > 0, memory_errors::double_free);
         _index = 0;
         return result_void{};
      }

      void reset() { _index = 0; }

      uintptr_t get_base_address() const { return (uintptr_t)_raw.get(); }
      size_t    get_max_size() const { return _size; }

    private:
      bounded_allocator(size_t size) : _size(size) {
         try {
            _raw = std::unique_ptr<char[]>(new char[_size]);
         } catch (...) { _valid = false; }
      }
      bool                    _valid = true;
      size_t                  _size  = 0;
      std::unique_ptr<char[]> _raw   = nullptr;
      size_t                  _index = 0;
   };

   class growable_allocator {
    public:
      static constexpr size_t max_memory_size = 1024 * 1024 * 1024; // 1GB
      static constexpr size_t chunk_size      = 128 * 1024;         // 128KB
      static constexpr size_t align_amt       = 16;
      static constexpr size_t align_offset(size_t offset) { return (offset + align_amt - 1) & ~(align_amt - 1); }

      static outcome::result<growable_allocator> init(size_t sz) {
         growable_allocator ga(sz);
         if (LIKELY(ga._valid))
            return std::move(ga);
         return system_errors::constructor_failure;
      }

      ~growable_allocator() {
         if (LIKELY(_valid))
            munmap(_base, max_memory_size);
      }

      template <typename T>
      T* alloc(size_t size = 0) {
         size_t aligned = align_offset((sizeof(T) * size) + _offset);
         if (aligned >= _size) {
            size_t chunks_to_alloc = aligned / chunk_size;
            EOS_VM_ASSERT_INVALIDATE(
                  mprotect((char*)_base + _size, (chunk_size * chunks_to_alloc), PROT_READ | PROT_WRITE) == -1,
                  memory_errors::bad_alloc);
            _size += (chunk_size * chunks_to_alloc);
         }

         T* ptr  = (T*)(_base + _offset);
         _offset = aligned;
         return ptr;
      }

      outcome::result<result_void> free() { EOS_VM_ASSERT(false, system_errors::unimplemented_failure); }

      void reset() { _offset = 0; }

    private:
      // size in bytes
      growable_allocator(size_t size) : _size((size / chunk_size)) {
         _base  = (char*)mmap(NULL, max_memory_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
         _valid = _base == MAP_FAILED;
         if (size != 0) {
            size_t chunks_to_alloc = (size / chunk_size) + 1;
            _size += (chunk_size * chunks_to_alloc);
            _valid |= mprotect((char*)_base, _size, PROT_READ | PROT_WRITE) == -1;
         }
      }

      bool   _valid  = true;
      size_t _offset = 0;
      size_t _size   = 0;
      char*  _base;
   };

   class wasm_allocator {
    private:
      bool    _valid    = true;
      char*   _raw      = nullptr;
      char*   _previous = _raw;
      int32_t _page     = 0;

      wasm_allocator() {
         // set_up_signals();
         _raw      = (char*)mmap(NULL, max_memory, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
         _previous = _raw;
         mprotect(_raw, 1 * page_size, PROT_READ | PROT_WRITE);
         _page = 1;
      }

    public:
      template <typename T>
      T* alloc(size_t size) {
         EOS_WB_ASSERT(_page + size <= max_pages, wasm_bad_alloc, "exceeded max number of pages");
         mprotect(_raw + (page_size * _page), (page_size * size), PROT_READ | PROT_WRITE);
         T* ptr    = (T*)_previous;
         _previous = (_raw + (page_size * _page));
         _page += size;
         return ptr;
      }

      void free() { munmap(_raw, max_memory); }

      static outcome::result<wasm_allocator> init() {
         wasm_allocator wa;
         if (LIKELY(wa._valid))
            return std::move(wa);
         return system_errors::constructor_failure;
      }

      void reset() {
         uint64_t size = page_size * _page;
         _previous     = _raw;
         memset(_raw, 0, size);
         _page = 1;
         mprotect(_raw, size, PROT_NONE);
         mprotect(_raw, 1 * page_size, PROT_READ | PROT_WRITE);
      }
      template <typename T>
      inline T* get_base_ptr() const {
         return reinterpret_cast<T*>(_raw);
      }
      inline int32_t get_current_page() const { return _page; }
   };

   template <typename Allocator>
   class maybe_allocator {
    public:
      inline maybe_allocator(outcome::result<Allocator>&& alloc) : _alloc(std::move(alloc)) {}
      inline auto reset() { return _alloc.reset(); }
      template <typename T>
      inline auto alloc(size_t sz) {
         return _alloc.template alloc<T>(sz);
      }

      inline auto       get_base_ptr() const { return _alloc.get_base_ptr(); }
      inline auto       free() { return _alloc.free(); }
      inline Allocator* get_allocator() { return _alloc ? &_alloc.value() : nullptr; }

    private:
      outcome::result<Allocator> _alloc;
   };
}} // namespace eosio::vm
