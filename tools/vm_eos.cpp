#include <eosio/vm/backend.hpp>
#include <eosio/vm/error_codes.hpp>
#include <eosio/vm/host_function.hpp>
#include <eosio/vm/watchdog.hpp>
#include <sys/time.h>

#include <iostream>
#include <eosiolib/action.h>
#include <eosiolib/system.h>
#include <eosiolib/crypto.h>
#include <eosiolib/chain.h>
#include <eosiolib/db.h>
#include <eosiolib/permission.h>
#include <eosiolib/print.h>
#include <eosiolib/privileged.h>
#include <eosiolib/transaction.h>

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


//eosiolib/action.h
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

//eosiolib/system.h
   rhf_t::add<nullptr_t, &eosio_assert_message,       wasm_allocator>("env", "eosio_assert_message");
   rhf_t::add<nullptr_t, &eosio_assert_code,          wasm_allocator>("env", "eosio_assert_code");
   rhf_t::add<nullptr_t, &eosio_exit,                 wasm_allocator>("env", "eosio_exit");
   rhf_t::add<nullptr_t, &current_time,               wasm_allocator>("env", "current_time");

//eosiolib/chain.h
   rhf_t::add<nullptr_t, &get_active_producers,       wasm_allocator>("env", "get_active_producers");

//eosiolib/crypto.h
   rhf_t::add<nullptr_t, &assert_sha256,              wasm_allocator>("env", "assert_sha256");
   rhf_t::add<nullptr_t, &assert_sha1,                wasm_allocator>("env", "assert_sha1");
   rhf_t::add<nullptr_t, &assert_sha512,              wasm_allocator>("env", "assert_sha512");
   rhf_t::add<nullptr_t, &assert_ripemd160,           wasm_allocator>("env", "assert_ripemd160");
   rhf_t::add<nullptr_t, &sha256,                     wasm_allocator>("env", "sha256");
   rhf_t::add<nullptr_t, &sha1,                       wasm_allocator>("env", "sha1");
   rhf_t::add<nullptr_t, &sha512,                     wasm_allocator>("env", "sha512");
   rhf_t::add<nullptr_t, &ripemd160,                  wasm_allocator>("env", "ripemd160");
   rhf_t::add<nullptr_t, &recover_key,                wasm_allocator>("env", "recover_key");
   rhf_t::add<nullptr_t, &assert_recover_key,         wasm_allocator>("env", "assert_recover_key");

//eosiolib/db.h
rhf_t::add<nullptr_t, &db_store_i64,                  wasm_allocator>("env", "db_store_i64");
rhf_t::add<nullptr_t, &db_update_i64,                 wasm_allocator>("env", "db_update_i64");
rhf_t::add<nullptr_t, &db_remove_i64,                 wasm_allocator>("env", "db_remove_i64");
rhf_t::add<nullptr_t, &db_get_i64,                    wasm_allocator>("env", "db_get_i64");
rhf_t::add<nullptr_t, &db_next_i64,                   wasm_allocator>("env", "db_next_i64");
rhf_t::add<nullptr_t, &db_previous_i64,               wasm_allocator>("env", "db_previous_i64");
rhf_t::add<nullptr_t, &db_find_i64,                   wasm_allocator>("env", "db_find_i64");
rhf_t::add<nullptr_t, &db_lowerbound_i64,             wasm_allocator>("env", "db_lowerbound_i64");
rhf_t::add<nullptr_t, &db_upperbound_i64,             wasm_allocator>("env", "db_upperbound_i64");
rhf_t::add<nullptr_t, &db_end_i64,                    wasm_allocator>("env", "db_end_i64");
rhf_t::add<nullptr_t, &db_idx64_store,                wasm_allocator>("env", "db_idx64_store");
rhf_t::add<nullptr_t, &db_idx64_update,               wasm_allocator>("env", "db_idx64_update");
rhf_t::add<nullptr_t, &db_idx64_remove,               wasm_allocator>("env", "db_idx64_remove");
rhf_t::add<nullptr_t, &db_idx64_next,                 wasm_allocator>("env", "db_idx64_next");
rhf_t::add<nullptr_t, &db_idx64_previous,             wasm_allocator>("env", "db_idx64_previous");
rhf_t::add<nullptr_t, &db_idx64_find_primary,         wasm_allocator>("env", "db_idx64_find_primary");
rhf_t::add<nullptr_t, &db_idx64_find_secondary,       wasm_allocator>("env", "db_idx64_find_secondary");
rhf_t::add<nullptr_t, &db_idx64_lowerbound,           wasm_allocator>("env", "db_idx64_lowerbound");
rhf_t::add<nullptr_t, &db_idx64_upperbound,           wasm_allocator>("env", "db_idx64_upperbound");
rhf_t::add<nullptr_t, &db_idx64_end,                  wasm_allocator>("env", "db_idx64_end");
rhf_t::add<nullptr_t, &db_idx128_store,               wasm_allocator>("env", "db_idx128_store");
rhf_t::add<nullptr_t, &db_idx128_update,              wasm_allocator>("env", "db_idx128_update");
rhf_t::add<nullptr_t, &db_idx128_remove,              wasm_allocator>("env", "db_idx128_remove");
rhf_t::add<nullptr_t, &db_idx128_next,                wasm_allocator>("env", "db_idx128_next");
rhf_t::add<nullptr_t, &db_idx128_previous,            wasm_allocator>("env", "db_idx128_previous");
rhf_t::add<nullptr_t, &db_idx128_find_primary,        wasm_allocator>("env", "db_idx128_find_primary");
rhf_t::add<nullptr_t, &db_idx128_find_secondary,      wasm_allocator>("env", "db_idx128_find_secondary");
rhf_t::add<nullptr_t, &db_idx128_lowerbound,          wasm_allocator>("env", "db_idx128_lowerbound");
rhf_t::add<nullptr_t, &db_idx128_upperbound,          wasm_allocator>("env", "db_idx128_upperbound");
rhf_t::add<nullptr_t, &db_idx128_end,                 wasm_allocator>("env", "db_idx128_end");
rhf_t::add<nullptr_t, &db_idx256_store,               wasm_allocator>("env", "db_idx256_store");
rhf_t::add<nullptr_t, &db_idx256_update,              wasm_allocator>("env", "db_idx256_update");
rhf_t::add<nullptr_t, &db_idx256_remove,              wasm_allocator>("env", "db_idx256_remove");
rhf_t::add<nullptr_t, &db_idx256_next,                wasm_allocator>("env", "db_idx256_next");
rhf_t::add<nullptr_t, &db_idx256_previous,            wasm_allocator>("env", "db_idx256_previous");
rhf_t::add<nullptr_t, &db_idx256_find_primary,        wasm_allocator>("env", "db_idx256_find_primary");
rhf_t::add<nullptr_t, &db_idx256_find_secondary,      wasm_allocator>("env", "db_idx256_find_secondary");
rhf_t::add<nullptr_t, &db_idx256_lowerbound,          wasm_allocator>("env", "db_idx256_lowerbound");
rhf_t::add<nullptr_t, &db_idx256_upperbound,          wasm_allocator>("env", "db_idx256_upperbound");
rhf_t::add<nullptr_t, &db_idx256_end,                 wasm_allocator>("env", "db_idx256_end");
rhf_t::add<nullptr_t, &db_idx_double_store,           wasm_allocator>("env", "db_idx_double_store");
rhf_t::add<nullptr_t, &db_idx_double_update,          wasm_allocator>("env", "db_idx_double_update");
rhf_t::add<nullptr_t, &db_idx_double_remove,          wasm_allocator>("env", "db_idx_double_remove");
rhf_t::add<nullptr_t, &db_idx_double_next,            wasm_allocator>("env", "db_idx_double_next");
rhf_t::add<nullptr_t, &db_idx_double_previous,        wasm_allocator>("env", "db_idx_double_previous");
rhf_t::add<nullptr_t, &db_idx_double_find_primary,    wasm_allocator>("env", "db_idx_double_find_primary");
rhf_t::add<nullptr_t, &db_idx_double_find_secondary,  wasm_allocator>("env", "db_idx_double_find_secondary");
rhf_t::add<nullptr_t, &db_idx_double_lowerbound,      wasm_allocator>("env", "db_idx_double_lowerbound");
rhf_t::add<nullptr_t, &db_idx_double_upperbound,      wasm_allocator>("env", "db_idx_double_upperbound");
rhf_t::add<nullptr_t, &db_idx_double_end,             wasm_allocator>("env", "db_idx_double_end");
rhf_t::add<nullptr_t, &db_idx_long_double_store,      wasm_allocator>("env", "db_idx_long_double_store");
rhf_t::add<nullptr_t, &db_idx_long_double_update,     wasm_allocator>("env", "db_idx_long_double_update");
rhf_t::add<nullptr_t, &db_idx_long_double_remove,     wasm_allocator>("env", "db_idx_long_double_remove");
rhf_t::add<nullptr_t, &db_idx_long_double_next,       wasm_allocator>("env", "db_idx_long_double_next");
rhf_t::add<nullptr_t, &db_idx_long_double_previous,   wasm_allocator>("env", "db_idx_long_double_previous");
rhf_t::add<nullptr_t, &db_idx_long_double_find_primary,   wasm_allocator>("env", "db_idx_long_double_find_primary");
rhf_t::add<nullptr_t, &db_idx_long_double_find_secondary, wasm_allocator>("env", "db_idx_long_double_find_secondary");
rhf_t::add<nullptr_t, &db_idx_long_double_lowerbound,  wasm_allocator>("env", "db_idx_long_double_lowerbound");
rhf_t::add<nullptr_t, &db_idx_long_double_upperbound,  wasm_allocator>("env", "db_idx_long_double_upperbound");
rhf_t::add<nullptr_t, &db_idx_long_double_end,         wasm_allocator>("env", "db_idx_long_double_end");

//eosiolib/permission.h
rhf_t::add<nullptr_t, &check_transaction_authorization,wasm_allocator>("env", "check_transaction_authorization");
rhf_t::add<nullptr_t, &get_permission_last_used,       wasm_allocator>("env", "get_permission_last_used");
rhf_t::add<nullptr_t, &get_account_creation_time,      wasm_allocator>("env", "get_account_creation_time");

//eosiolib/print.h
rhf_t::add<nullptr_t, & prints,        wasm_allocator>("env", "prints");
rhf_t::add<nullptr_t, & prints_l,      wasm_allocator>("env", "prints_l");
rhf_t::add<nullptr_t, & printi,        wasm_allocator>("env", "printi");
rhf_t::add<nullptr_t, & printui,       wasm_allocator>("env", "printui");
rhf_t::add<nullptr_t, & printi128,     wasm_allocator>("env", "printi128");
rhf_t::add<nullptr_t, & printui128,    wasm_allocator>("env", "printui128");
rhf_t::add<nullptr_t, & printsf,       wasm_allocator>("env", "printsf");
rhf_t::add<nullptr_t, & printdf,       wasm_allocator>("env", "printdf");
rhf_t::add<nullptr_t, & printqf,       wasm_allocator>("env", "printqf");
rhf_t::add<nullptr_t, & printn,        wasm_allocator>("env", "printn");
rhf_t::add<nullptr_t, & printhex,      wasm_allocator>("env", "printhex");

//eosiolib/privileged.h
rhf_t::add<nullptr_t, &get_resource_limits,           wasm_allocator>("env", "get_resource_limits");
rhf_t::add<nullptr_t, &set_resource_limits,           wasm_allocator>("env", "set_resource_limits");
rhf_t::add<nullptr_t, &set_proposed_producers,        wasm_allocator>("env", "set_proposed_producers");
//rhf_t::add<nullptr_t, &set_active_producers,          wasm_allocator>("env", "set_active_producers");
rhf_t::add<nullptr_t, &is_privileged,                 wasm_allocator>("env", "is_privileged");
rhf_t::add<nullptr_t, &set_privileged,                wasm_allocator>("env", "set_privileged");
rhf_t::add<nullptr_t, &set_blockchain_parameters_packed,      wasm_allocator>("env", "set_blockchain_parameters_packed");
rhf_t::add<nullptr_t, &get_blockchain_parameters_packed,      wasm_allocator>("env", "get_blockchain_parameters_packed");
rhf_t::add<nullptr_t, &activate_feature,              wasm_allocator>("env", "activate_feature");

//eosiolib/transaction.h
rhf_t::add<nullptr_t, &send_deferred,                 wasm_allocator>("env", "send_deferred");
rhf_t::add<nullptr_t, &cancel_deferred,               wasm_allocator>("env", "cancel_deferred");
rhf_t::add<nullptr_t, &read_transaction,              wasm_allocator>("env", "read_transaction");
rhf_t::add<nullptr_t, &transaction_size,              wasm_allocator>("env", "transaction_size");
rhf_t::add<nullptr_t, &tapos_block_num,               wasm_allocator>("env", "tapos_block_num");
rhf_t::add<nullptr_t, &tapos_block_prefix,            wasm_allocator>("env", "tapos_block_prefix");
rhf_t::add<nullptr_t, &expiration,                    wasm_allocator>("env", "expiration");
rhf_t::add<nullptr_t, &get_action,                    wasm_allocator>("env", "get_action");
rhf_t::add<nullptr_t, &get_context_free_data,         wasm_allocator>("env", "get_context_free_data");

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