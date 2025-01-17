#include <eosio/vm/backend.hpp>
#include <eosio/vm/error_codes.hpp>
#include <eosio/vm/host_function.hpp>
#include <eosio/vm/watchdog.hpp>
#include <sys/time.h>

#include <iostream>

using namespace eosio;
using namespace eosio::vm;

#include "hello.wasm.hpp"

// example of host function as a raw C style function
void eosio_assert(bool test, const char* msg) {
   if (!test) {
      std::cout << msg << std::endl;
      throw 0;
   }
}

void print_num(uint64_t n) { std::cout << "Number : " << n << "\n"; }

void printdf(double value) {
   printf("%f\n", value);
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

/**
 * Simple implementation of an interpreter using eos-vm.
 */
int main(int argc, char** argv) {
   if (argc < 4) {
      std::cerr << "Please enter three numbers\n";
      return -1;
   }
   // Thread specific `allocator` used for wasm linear memory.
   wasm_allocator wa;
   // Specific the backend with example_host_methods for host functions.
   using backend_t = eosio::vm::backend<example_host_methods>;
   using rhf_t     = eosio::vm::registered_host_functions<example_host_methods>;

   // register print_num
   rhf_t::add<nullptr_t, &print_num, wasm_allocator>("env", "print_num");

   // register print_num
   rhf_t::add<nullptr_t, &printdf, wasm_allocator>("env", "printdf");

   // register eosio_assert
   rhf_t::add<nullptr_t, &eosio_assert, wasm_allocator>("env", "eosio_assert");
   // register print_name
   rhf_t::add<example_host_methods, &example_host_methods::print_name, wasm_allocator>("env", "print_name");
   // finally register memset
   rhf_t::add<nullptr_t, &example_host_methods::memset, wasm_allocator>("env", "memset");

   watchdog<std::chrono::nanoseconds> wd;
   wd.set_duration(std::chrono::seconds(3));
   try {
      // Instaniate a new backend using the wasm provided.
      backend_t bkend(hello_wasm);
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
      bkend(&ehm, "env", "apply", (uint64_t)std::atoi(argv[1]), (uint64_t)std::atoi(argv[2]),
            (uint64_t)std::atoi(argv[3]));
      uint64_t end = get_microseconds();

      printf("+++++++duration: %d \n", end - start);

   } catch (...) { std::cerr << "eos-vm interpreter error\n"; }
   return 0;
}
