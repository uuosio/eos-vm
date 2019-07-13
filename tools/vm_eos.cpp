#include <eosio/vm/backend.hpp>
#include <eosio/vm/error_codes.hpp>
#include <eosio/vm/host_function.hpp>
#include <eosio/vm/watchdog.hpp>
#include <sys/time.h>

#include <iostream>
#include <eosiolib/action.h>
#include <eosiolib/system.h>

using namespace eosio;
using namespace eosio::vm;

void print_num(uint64_t n) { std::cout << "Number : " << n << "\n"; }

extern "C" {
   void printdf(double value);
}

struct example_host_methods {
   // example of a host "method"
   void print_name(const char* nm) { std::cout << "Name : " << nm << " " << field << "\n"; }
   // example of another type of host function
   static void* memset(void* ptr, int x, size_t n) { return ::memset(ptr, x, n); }
   std::string  field = "";
};

static uint64_t get_microseconds() {
   struct timeval  tv;
   gettimeofday(&tv, NULL);
   return tv.tv_sec * 1000000LL + tv.tv_usec * 1LL ;
}

using rhf_t     = eosio::vm::registered_host_functions<example_host_methods>;

extern "C" void eos_vm_init() {
   // register print_num
   rhf_t::add<nullptr_t, &print_num, wasm_allocator>("env", "print_num");

   // register print_num
   rhf_t::add<nullptr_t, &printdf, wasm_allocator>("env", "printdf");

   // register eosio_assert
//   rhf_t::add<nullptr_t, &eosio_assert, wasm_allocator>("env", "eosio_assert");
   // register print_name
   rhf_t::add<example_host_methods, &example_host_methods::print_name, wasm_allocator>("env", "print_name");
   // finally register memset
   rhf_t::add<nullptr_t, &example_host_methods::memset, wasm_allocator>("env", "memset");


   rhf_t::add<nullptr_t, &read_action_data,           wasm_allocator>("env", "read_action_data");
   rhf_t::add<nullptr_t, &action_data_size,           wasm_allocator>("env", "action_data_size");
   rhf_t::add<nullptr_t, &require_recipient,          wasm_allocator>("env", "require_recipient");
   rhf_t::add<nullptr_t, &require_auth,               wasm_allocator>("env", "require_auth");
   rhf_t::add<nullptr_t, &has_auth,                   wasm_allocator>("env", "has_auth");
   rhf_t::add<nullptr_t, &require_auth2,              wasm_allocator>("env", "require_auth2");
   rhf_t::add<nullptr_t, &is_account,                 wasm_allocator>("env", "is_account");
   rhf_t::add<nullptr_t, &send_inline,                wasm_allocator>("env", "send_inline");

   rhf_t::add<nullptr_t, &send_context_free_inline,   wasm_allocator>("env", "send_context_free_inline");
   rhf_t::add<nullptr_t, &publication_time,           wasm_allocator>("env", "publication_time");
   rhf_t::add<nullptr_t, &current_receiver,           wasm_allocator>("env", "current_receiver");

   rhf_t::add<nullptr_t, &eosio_assert,               wasm_allocator>("env", "eosio_assert");
}

extern "C" int eos_vm_apply(uint64_t receiver, uint64_t code, uint64_t action, const unsigned char *wasm_code, size_t wasm_code_size) {

   // Thread specific `allocator` used for wasm linear memory.
   wasm_allocator wa;
   // Specific the backend with example_host_methods for host functions.
   using backend_t = eosio::vm::backend<example_host_methods>;

   watchdog<std::chrono::nanoseconds> wd;
   wd.set_duration(std::chrono::seconds(3));
   try {
      wasm_code_ptr cp((unsigned char*)wasm_code, 0);
      // Instaniate a new backend using the wasm provided.
      backend_t bkend(cp, wasm_code_size);
      wd.set_callback([&]() { bkend.get_context().exit(); });

      // Point the backend to the allocator you want it to use.
      bkend.set_wasm_allocator(&wa);
      // Resolve the host functions indices.
      rhf_t::resolve(bkend.get_module());

      // Instaniate a "host"
      example_host_methods ehm;
      ehm.field = "testing";


      // Execute apply.
      uint64_t start = get_microseconds();
      bkend(&ehm, "env", "apply", receiver, code, action);
      uint64_t end = get_microseconds();

      printf("+++++++duration: %d \n", end - start);

   } catch (...) { std::cerr << "eos-vm interpreter error\n"; }
   return 0;
}


int main(int argc, char **argv) {
   return 0;
}