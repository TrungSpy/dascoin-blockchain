/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <graphene/chain/protocol/authority.hpp>
#include <graphene/app/impacted.hpp>

namespace graphene { namespace app {

using namespace fc;
using namespace graphene::chain;

// TODO:  Review all of these, especially no-ops
struct get_impacted_account_visitor
{
   flat_set<account_id_type>& _impacted;
   get_impacted_account_visitor( flat_set<account_id_type>& impact ):_impacted(impact) {}
   typedef void result_type;

   void operator()( const transfer_operation& op )
   {
      _impacted.insert( op.to );
   }

   void operator()( const asset_claim_fees_operation& op ){}
   void operator()( const limit_order_create_operation& op ) {}
   void operator()( const limit_order_cancel_operation& op )
   {
      _impacted.insert( op.fee_paying_account );
   }
   void operator()( const call_order_update_operation& op ) {}
   void operator()( const fill_order_operation& op )
   {
      _impacted.insert( op.account_id );
   }

   void operator()( const account_create_operation& op )
   {
      _impacted.insert( op.registrar );
      _impacted.insert( op.referrer );
      add_authority_accounts( _impacted, op.owner );
      add_authority_accounts( _impacted, op.active );
   }

   void operator()( const account_update_operation& op )
   {
      _impacted.insert( op.account );
      if( op.owner )
         add_authority_accounts( _impacted, *(op.owner) );
      if( op.active )
         add_authority_accounts( _impacted, *(op.active) );
   }

   void operator()( const account_whitelist_operation& op )
   {
      _impacted.insert( op.account_to_list );
   }

   void operator()( const account_upgrade_operation& op ) {}
   void operator()( const account_transfer_operation& op )
   {
      _impacted.insert( op.new_owner );
   }

   void operator()( const asset_create_operation& op ) {}
   void operator()( const asset_update_operation& op )
   {
      if( op.new_issuer )
         _impacted.insert( *(op.new_issuer) );
   }

   void operator()( const asset_update_bitasset_operation& op ) {}
   void operator()( const asset_update_feed_producers_operation& op ) {}

   void operator()( const asset_issue_operation& op )
   {
      _impacted.insert( op.issue_to_account );
   }

   void operator()( const asset_reserve_operation& op ) {}
   void operator()( const asset_fund_fee_pool_operation& op ) {}
   void operator()( const asset_settle_operation& op ) {}
   void operator()( const asset_global_settle_operation& op ) {}
   void operator()( const asset_publish_feed_operation& op ) {}
   void operator()( const remove_root_authority_operation& op ) {}
   void operator()( const create_witness_operation& op ) {}
   void operator()( const update_witness_operation& op ) {}
   void operator()( const remove_witness_operation& op ) {}
   void operator()( const activate_witness_operation& op ) {}
   void operator()( const deactivate_witness_operation& op ) {}

   void operator()( const remove_vault_limit_operation& op )
   {
      _impacted.insert( op.authority );
   }

   void operator()( const witness_create_operation& op )
   {
      _impacted.insert( op.witness_account );
   }
   void operator()( const witness_update_operation& op )
   {
      _impacted.insert( op.witness_account );
   }

   void operator()( const proposal_create_operation& op )
   {
      vector<authority> other;
      for( const auto& proposed_op : op.proposed_ops )
         operation_get_required_authorities( proposed_op.op, _impacted, _impacted, other );
      for( auto& o : other )
         add_authority_accounts( _impacted, o );
   }

   void operator()( const proposal_update_operation& op ) {}
   void operator()( const proposal_delete_operation& op ) {}

   void operator()( const withdraw_permission_create_operation& op )
   {
      _impacted.insert( op.authorized_account );
   }

   void operator()( const withdraw_permission_update_operation& op )
   {
      _impacted.insert( op.authorized_account );
   }

   void operator()( const withdraw_permission_claim_operation& op )
   {
      _impacted.insert( op.withdraw_from_account );
   }

   void operator()( const withdraw_permission_delete_operation& op )
   {
      _impacted.insert( op.authorized_account );
   }

   void operator()( const committee_member_create_operation& op )
   {
      _impacted.insert( op.committee_member_account );
   }
   void operator()( const committee_member_update_operation& op )
   {
      _impacted.insert( op.committee_member_account );
   }
   void operator()( const committee_member_update_global_parameters_operation& op ) {}
   
   void operator()( const board_update_chain_authority_operation& op ) 
   {
      _impacted.insert( op.account );
      _impacted.insert( op.committee_member_account );
   }

   void operator()( const create_license_type_operation& op ) {}

   void operator()( const edit_license_type_operation& op ) {}

   void operator()( const issue_license_operation& op )
   {
      _impacted.insert( op.issuer );
      _impacted.insert( op.account );
   }

   void operator()( const vesting_balance_create_operation& op )
   {
      _impacted.insert( op.owner );
   }

   void operator()( const vesting_balance_withdraw_operation& op ) {}
   void operator()( const worker_create_operation& op ) {}
   void operator()( const custom_operation& op ) {}
   void operator()( const assert_operation& op ) {}
   void operator()( const balance_claim_operation& op ) {}
   void operator()( const change_operation_fee_operation& op )
   {
      _impacted.insert( op.issuer );
   }

   void operator()( const change_fee_pool_account_operation& op )
   {
      _impacted.insert( op.issuer );
      _impacted.insert( op.fee_pool_account_id );
   }

   void operator()( const override_transfer_operation& op )
   {
      _impacted.insert( op.to );
      _impacted.insert( op.from );
      _impacted.insert( op.issuer );
   }

   void operator()( const transfer_to_blind_operation& op )
   {
      _impacted.insert( op.from );
      for( const auto& out : op.outputs )
         add_authority_accounts( _impacted, out.owner );
   }

   void operator()( const blind_transfer_operation& op )
   {
      for( const auto& in : op.inputs )
         add_authority_accounts( _impacted, in.owner );
      for( const auto& out : op.outputs )
         add_authority_accounts( _impacted, out.owner );
   }

   void operator()( const transfer_from_blind_operation& op )
   {
      _impacted.insert( op.to );
      for( const auto& in : op.inputs )
         add_authority_accounts( _impacted, in.owner );
   }

   void operator()( const asset_settle_cancel_operation& op )
   {
      _impacted.insert( op.account );
   }

   void operator()( const fba_distribute_operation& op )
   {
      _impacted.insert( op.account_id );
   }

   void operator()( const tether_accounts_operation& op )
   {
      _impacted.insert( op.wallet_account );
      _impacted.insert( op.vault_account );
   }

   void operator()( const upgrade_account_cycles_operation& op )
   {
      _impacted.insert( op.account );
   }

   void operator()( const change_public_keys_operation& op )
   {
      _impacted.insert( op.account );
   }

   void operator()( const update_pi_limits_operation& op )
   {
      _impacted.insert( op.pi_validator );
      _impacted.insert( op.account );
   }

   void operator()( const asset_create_issue_request_operation& op )
   {
      _impacted.insert( op.issuer );
      _impacted.insert( op.receiver );
   }

   void operator()( const asset_distribute_completed_request_operation& op )
   {
      _impacted.insert( op.issuer );
      _impacted.insert( op.receiver );
   }

   void operator()( const asset_deny_issue_request_operation& op ) {}

   void operator()( const wire_out_operation& op )
   {
      _impacted.insert( op.account );
   }

   void operator()( const wire_out_complete_operation& op )
   {
      _impacted.insert( op.wire_out_handler );
   }

   void operator()( const wire_out_reject_operation& op )
   {
      _impacted.insert( op.wire_out_handler );
   }

   void operator()( const wire_out_result_operation& op )
   {
      _impacted.insert( op.account );
   }

   void operator()( const wire_out_with_fee_operation& op )
   {
     _impacted.insert( op.account );
   }

   void operator()( const wire_out_with_fee_complete_operation& op )
   {
     _impacted.insert( op.wire_out_handler );
   }

   void operator()( const wire_out_with_fee_reject_operation& op )
   {
     _impacted.insert( op.wire_out_handler );
   }

   void operator()( const wire_out_with_fee_result_operation& op )
   {
     _impacted.insert( op.account );
   }

   void operator()( const transfer_vault_to_wallet_operation& op )
   {
      _impacted.insert( op.from_vault );
      _impacted.insert( op.to_wallet );
   }

   void operator()( const transfer_wallet_to_vault_operation& op )
   {
      _impacted.insert( op.from_wallet );
      _impacted.insert( op.to_vault );
   }

   void operator()( const submit_reserve_cycles_to_queue_operation& op ) 
   {
      _impacted.insert( op.issuer );
      _impacted.insert( op.account );
   }

   void operator()( const submit_cycles_to_queue_operation& op )
   {
      _impacted.insert( op.account );
   }

   void operator()( const submit_cycles_to_queue_by_license_operation& op )
   {
      _impacted.insert( op.account );
   }

   void operator()( const record_submit_reserve_cycles_to_queue_operation& op )
   {
      _impacted.insert( op.cycle_issuer );
      _impacted.insert( op.account );
   }

   void operator()( const record_submit_charter_license_cycles_operation& op )
   {
      _impacted.insert( op.license_issuer );
      _impacted.insert( op.account );
   }

   void operator()( const record_distribute_dascoin_operation& op )
   {
      _impacted.insert( op.account );
   }
   
   void operator()( const update_queue_parameters_operation& op ) {}

   void operator()( const update_global_frequency_operation& op )
   {
      _impacted.insert( op.authority );
   }

   void operator() ( const issue_free_cycles_operation& op )
   {
      _impacted.insert( op.authority );
      _impacted.insert( op.account );
   }

   void operator() ( const update_euro_limit_operation& op )
   {
      _impacted.insert( op.authority );
      _impacted.insert( op.account );
   }

   void operator() ( const create_upgrade_event_operation& op )
   {
      _impacted.insert( op.upgrade_creator );
   }

   void operator() ( const update_upgrade_event_operation& op )
   {
      _impacted.insert( op.upgrade_creator );
   }

   void operator() ( const delete_upgrade_event_operation& op )
   {
     _impacted.insert( op.upgrade_creator );
   }

   void operator() ( const update_license_operation& op )
   {
     _impacted.insert( op.authority );
   }

   void operator() ( const issue_cycles_to_license_operation& op )
   {
      _impacted.insert( op.authority );
      _impacted.insert( op.account );
   }

   void operator() ( const purchase_cycle_asset_operation& op )
   {
      _impacted.insert( op.wallet_id );
   }

   void operator() ( const transfer_cycles_from_licence_to_wallet_operation& op )
   {
      _impacted.insert( op.vault_id );
      _impacted.insert( op.wallet_id );
   }

   void operator() (const set_starting_cycle_asset_amount_operation& op)
   {
     _impacted.insert(op.issuer);
   }
};

void operation_get_impacted_accounts( const operation& op, flat_set<account_id_type>& result )
{
   get_impacted_account_visitor vtor = get_impacted_account_visitor( result );
   op.visit( vtor );
}

void transaction_get_impacted_accounts( const transaction& tx, flat_set<account_id_type>& result )
{
   for( const auto& op : tx.operations )
      operation_get_impacted_accounts( op, result );
}

} }
