/**
 * DASCOIN!
 */
#include <boost/test/unit_test.hpp>
#include <boost/program_options.hpp>

#include <graphene/account_history/account_history_plugin.hpp>
#include <graphene/market_history/market_history_plugin.hpp>

#include <graphene/db/simple_index.hpp>

#include <graphene/chain/license_objects.hpp>
// #include <graphene/chain/account_object.hpp>
// #include <graphene/chain/asset_object.hpp>
// #include <graphene/chain/committee_member_object.hpp>
// #include <graphene/chain/fba_object.hpp>
// #include <graphene/chain/market_object.hpp>
// #include <graphene/chain/vesting_balance_object.hpp>
// #include <graphene/chain/witness_object.hpp>

#include <graphene/utilities/tempdir.hpp>

#include <fc/crypto/digest.hpp>
#include <fc/smart_ref_impl.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "database_fixture.hpp"

using namespace graphene::chain::test;

namespace graphene { namespace chain {

const license_type_object& database_fixture::create_license_type(const string& kind, const string& name,
                                                                 share_type amount, 
                                                                 upgrade_multiplier_type balance_multipliers,
                                                                 upgrade_multiplier_type requeue_multipliers,
                                                                 upgrade_multiplier_type return_multipliers,
                                                                 share_type eur_limit)
{ try {
  create_license_type_operation op;
  op.admin = get_license_administrator_id();
  op.name = name;
  op.amount = amount;
  op.kind = kind;
  op.balance_multipliers = balance_multipliers;
  op.requeue_multipliers = requeue_multipliers;
  op.return_multipliers = return_multipliers;

  set_expiration(db, trx);
  trx.operations.clear();
  trx.operations.push_back(op);
  trx.validate();
  processed_transaction ptx = db.push_transaction(trx, ~0);
  trx.operations.clear();

  return db.get<license_type_object>( ptx.operation_results[0].get<object_id_type>() );

} FC_LOG_AND_RETHROW() }

} }  // namespace graphene::chain
