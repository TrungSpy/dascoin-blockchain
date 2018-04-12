/**
 * DASCOIN!
 *
 */
#include <graphene/chain/protocol/upgrade.hpp>

namespace graphene { namespace chain {

  void create_upgrade_event_operation::validate() const
  {
    FC_ASSERT( fee.amount >= 0 );
    FC_ASSERT( comment.size() <= DASCOIN_MAXIMUM_INTERNAL_MEMO_LENGTH );
    for (const auto& subsequent_event : subsequent_execution_times)
    {
      FC_ASSERT( execution_time < subsequent_event,
                 "Subsequent execution time cannot be before the main execution time, execution time: ${exec} subsequent time: ${time} ",
                 ("exec", execution_time)
                 ("time", subsequent_event)
               );
    }
  }

  void update_upgrade_event_operation::validate() const
  {
    FC_ASSERT( fee.amount >= 0 );
    if (comment.valid())
      FC_ASSERT( (*comment).size() <= DASCOIN_MAXIMUM_INTERNAL_MEMO_LENGTH );
    if (subsequent_execution_times.valid())
    {
      for (const auto& subsequent_event : *subsequent_execution_times)
      {
        FC_ASSERT( execution_time < subsequent_event,
                   "Subsequent execution time cannot be before the main execution time, execution time: ${exec} subsequent time: ${time} ",
                   ("exec", execution_time)
                   ("time", subsequent_event)
                 );
      }
    }
  }

  void delete_upgrade_event_operation::validate() const
  {

  }

} } // namespace graphene::chain
