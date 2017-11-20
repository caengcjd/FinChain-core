/*
 * Copyright (c) 2015 FinChain, Inc., and contributors.
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

#include <boost/test/unit_test.hpp>

#include <graphene/chain/database.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/hardfork.hpp>

#include <graphene/chain/account_object.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/committee_member_object.hpp>
#include <graphene/chain/market_object.hpp>
#include <graphene/chain/vesting_balance_object.hpp>
#include <graphene/chain/withdraw_permission_object.hpp>
#include <graphene/chain/witness_object.hpp>

#include <fc/crypto/digest.hpp>

#include "../common/database_fixture.hpp"

using namespace graphene::chain;
using namespace graphene::chain::test;

BOOST_FIXTURE_TEST_SUITE( operation_tests, database_fixture )


BOOST_AUTO_TEST_CASE(lock_balance_test)
{
	try {
		ACTORS((dan));
		const auto& core = asset_id_type()(db);
		auto total = get_balance(committee_account, asset_id_type());
		auto total3 = get_balance(account_id_type(3), asset_id_type());
		BOOST_CHECK_EQUAL(get_balance(dan_id, asset_id_type()), 0);
		transfer(committee_account, dan_id, asset(10000));
		transfer(committee_account, account_id_type(3), asset(100000000));

		create_lock_able_asset(committee_account,asset_id_type(0),50,45,300000000LL*100000LL);
		{
			lock_balance_operation op;
			op.fee = asset();
			op.issuer = dan_id;
			op.amount = asset(1000);
			op.period = 3600 * 24;
			trx.operations.push_back(op);
	
			sign(trx, dan_private_key);
			db.push_transaction(trx);
			trx.clear();
		}
		generate_block();
		BOOST_CHECK_EQUAL(get_balance(dan_id, asset_id_type()), 10000 - 1000);
#if 1
		{
			unlock_balance_operation op;
			op.fee = asset();
			op.issuer = dan_id;
			const auto & ids = db.get_locked_balance_ids(dan_id, asset_id_type());
			FC_ASSERT(ids.size() > 0, "dan didn't has locked balance!");

			unlock_balance_operation::unlock_detail one;
			one.locked_id = ids.at(0);
			one.expired = false;
			op.locked.push_back(one);
			trx.operations.push_back(op);

			sign(trx, dan_private_key);
			db.push_transaction(trx);
			trx.clear();
		}
		generate_block();

		BOOST_CHECK_EQUAL(get_balance(dan_id, asset_id_type()), 10000);
#endif

#if 1
		{
			donation_balance_operation op;
			op.fee = asset();
			op.issuer = dan_id;
			op.amount = asset(999);
			
			trx.operations.push_back(op);

			sign(trx, dan_private_key);
			db.push_transaction(trx);
			trx.clear();
		}
		generate_block();
		BOOST_CHECK_EQUAL(get_balance(dan_id, asset_id_type()), 10000 - 999);
#endif
	}
	catch (fc::exception& e) {
		edump((e.to_detail_string()));
		throw;
	}
}

BOOST_AUTO_TEST_SUITE_END()