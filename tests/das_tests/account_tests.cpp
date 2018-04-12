/**
 * DASCOIN!
 */

#include <boost/test/unit_test.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/hardfork.hpp>

#include <graphene/chain/account_object.hpp>

#include "../common/database_fixture.hpp"

using namespace graphene::chain;
using namespace graphene::chain::test;

BOOST_FIXTURE_TEST_SUITE( dascoin_tests, database_fixture )

BOOST_FIXTURE_TEST_SUITE( account_unit_tests, database_fixture )

BOOST_AUTO_TEST_CASE( get_free_cycle_balance_non_existant_id_unit_test )
{ try {
  VAULT_ACTOR(vault);

  const fc::ecc::private_key new_key = fc::ecc::private_key::generate();
  const public_key_type key_id = new_key.get_public_key();
  const auto owner = authority(2, key_id, 1, init_account_pub_key, 1);
  const auto active = authority(2, key_id, 1, init_account_pub_key, 1);

  do_op(change_public_keys_operation(vault_id, {owner}, {active}));

  BOOST_TEST_MESSAGE("Updating vault account private keys.");

  BOOST_CHECK(vault.active.weight_threshold == 2);
  BOOST_CHECK(vault.active.num_auths() == 2);
  BOOST_CHECK(vault.active.key_auths.at(key_id) == 1);
  BOOST_CHECK(vault.active.key_auths.at(init_account_pub_key) == 1);
  BOOST_CHECK(vault.active_change_counter == 1);

  BOOST_CHECK(vault.owner.weight_threshold == 2);
  BOOST_CHECK(vault.owner.num_auths() == 2);
  BOOST_CHECK(vault.owner.key_auths.at(key_id) == 1);
  BOOST_CHECK(vault.owner.key_auths.at(init_account_pub_key) == 1);
  BOOST_CHECK(vault.owner_change_counter == 1);

  do_op(change_public_keys_operation(vault_id, {owner}, {active}));

  // Keys are changed twice at this point:
  BOOST_CHECK(vault.active_change_counter == 2);
  BOOST_CHECK(vault.owner_change_counter == 2);

} FC_LOG_AND_RETHROW() }

// this test works only when HARDFORK_EXEX_102_TIME is 1509516032
// if we change this time test will fail and we need to rewrite test
// to work with new time, basically we have to generate blocks to that moment....
BOOST_AUTO_TEST_CASE( remove_limit_from_all_vaults_test )
{ try {

  VAULT_ACTOR(vault1);

  BOOST_CHECK(!vault1.disable_vault_to_wallet_limit);

  generate_blocks(HARDFORK_EXEX_102_TIME - fc::hours(1));

  push_op(remove_vault_limit_operation(get_global_properties().authorities.license_administrator,""),true);

  VAULT_ACTOR(vault2);

  BOOST_CHECK(vault1.disable_vault_to_wallet_limit);
  BOOST_CHECK(!vault2.disable_vault_to_wallet_limit);
  generate_blocks(db.head_block_time() + fc::hours(2));

  VAULT_ACTOR(vault3);

  generate_blocks(db.head_block_time() + fc::hours(1));

  BOOST_CHECK(vault1.disable_vault_to_wallet_limit);
  BOOST_CHECK(vault3.disable_vault_to_wallet_limit);
  // this one has to be false because it is created before HARDFORK_EXEX_102_TIME
  // and after removing limit operation
  BOOST_CHECK(!vault2.disable_vault_to_wallet_limit);

  GRAPHENE_REQUIRE_THROW(do_op(update_euro_limit_operation(get_global_properties().authorities.license_administrator,
        vault2_id, true, optional<share_type>(),"comment")), fc::exception);

} FC_LOG_AND_RETHROW() }

BOOST_AUTO_TEST_CASE(starting_amount_of_cycle_asset_test)
{
  try
  {
    // Create some accounts and check if they have default balance
    ACTOR(wallet0)
    ACTOR(wallet1)
    VAULT_ACTOR(vault0);

    // Wallet accounts should have default balance
    auto balance = get_balance(wallet0_id, get_cycle_asset_id());
    BOOST_CHECK_EQUAL(balance, DASCOIN_DEFAULT_STARTING_CYCLE_ASSET_AMOUNT);
    balance = get_balance(wallet1_id, get_cycle_asset_id());
    BOOST_CHECK_EQUAL(balance, DASCOIN_DEFAULT_STARTING_CYCLE_ASSET_AMOUNT);
    // Vault account should have empty balance
    balance = get_balance(vault0_id, get_cycle_asset_id());
    BOOST_CHECK_EQUAL(balance, 0);

    // Change starting balance
    auto root_id = db.get_global_properties().authorities.root_administrator;
    do_op(set_starting_cycle_asset_amount_operation(root_id, 145));

    // Make sure it works for wallets ...
    ACTOR(wallet2)
    balance = get_balance(wallet2_id, get_cycle_asset_id());
    BOOST_CHECK_EQUAL(balance, 145);
    // ... and that nothing is change for vaults
    VAULT_ACTOR(vault1);
    balance = get_balance(vault1_id, get_cycle_asset_id());
    BOOST_CHECK_EQUAL(balance, 0);
  }
  FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_SUITE_END()  // account_unit_tests
BOOST_AUTO_TEST_SUITE_END()  // dascoin_tests
