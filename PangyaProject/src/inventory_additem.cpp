#include "inventory.h"
#include "pc.h"
#include "utils.h"
#include "common/packet.h"
#include "common/db.h"

using namespace Poco::Data::Keywords;

char Inventory::additem(pc* pc, struct item* item, bool transaction, bool from_shop) {
	uint32 item_type = utils::itemdb_type(item->type_id);
	switch (item_type) {
	case ITEMDB_CHAR:
		return addChar(pc, item, transaction, from_shop);
		break;
	case ITEMDB_USE:
		return addUse(pc, item, transaction, from_shop);
		break;
	}

	return ADDITEM_INVALID;
}

char Inventory::addChar(pc* pc, struct item* item, bool transaction, bool from_shop) {
	assert(pc);
	assert(item);

	// if character is already existed, return false
	if (pc_item_exists(item->type_id, ITEMDB_CHAR, FIND_BY_TYPEID)) {
		return ADDITEM_DUPLICATED;
	}

	// We should add character to database and reload db again
	Poco::Data::Session sess = sdb->get_session();
	Poco::Data::Statement stm(sess);
	stm << "EXEC sys_make_char ?, ?", use(pc->account_id_), use(item->type_id), now;
	Poco::Data::RecordSet rs(stm);

	// Reload char again
	load_character(pc);

	// put data to transaction if requested
	if (transaction) {
		if (rs.rowCount()) {
			int char_ret = rs["char_ret_id"];
			// find char that we recently insert
			auto nchar = std::find_if(character_.begin(), character_.end(), [&char_ret](std::shared_ptr<INV_CHAR> const& charf) {
				return charf->id == char_ret;
			});
			if (nchar != character_.end()) {
				add_transaction(item->item_type, *nchar);
			}
		}
	}
	return ADDITEM_SUCCESS;
}

char Inventory::addUse(pc* pc, struct item* item, bool transaction, bool from_shop) {
	assert(pc);
	assert(item);

	int old_amount = 0;
	int last_id = 0;

	auto pitem = std::find_if(item_.begin(), item_.end(), [&item](std::shared_ptr<INV_ITEM> const &itemf) {
		return (itemf->item_typeid == item->type_id) && (itemf->c0 > 0) && (itemf->valid == 1);
	}); 

	// stackable item
	if (pitem != item_.end()) {
		old_amount = (*pitem)->c0;
		// plus amount of item that stackable
		(*pitem)->c0 += item->amount;
		printf("old amount is = %d", (*pitem)->c0);
		(*pitem)->sync = true;
		if (transaction) {
			add_transaction(item->item_type, *pitem, old_amount);
		}
		if (from_shop) {
			show_buyitem(pc, *pitem);
		}
		return ADDITEM_SUCCESS;
	}

	// Create new one
	Poco::Data::Session sess = sdb->get_session();
	sess << "INSERT INTO inventory(account_id, typeid, c0) VALUES(?, ?, ?)", use(pc->account_id_), use(item->type_id), use(item->amount), now;
	sess << "SELECT @@IDENTITY", into(last_id), now;

	// Push item to vector
	std::shared_ptr<INV_ITEM> new_item(new INV_ITEM());
	new_item->id = last_id;
	new_item->item_typeid = item->type_id;
	new_item->c0 = item->amount;
	item_.push_back(new_item);

	if (transaction) {
		add_transaction(item->item_type, new_item, 0);
	}
	if (from_shop) {
		show_buyitem(pc, new_item);
	}

	return ADDITEM_SUCCESS;
}

void Inventory::show_buyitem(pc* pc, std::shared_ptr<INV_ITEM> const& item) {
	Packet packet;
	packet.write<uint16>(0xaa);
	packet.write<uint16>(1);
	packet.write<uint32>(item->item_typeid);
	packet.write<uint32>(item->id);
	packet.write<uint16>(0); // day total
	packet.write<uint8>(0); // flag
	packet.write<uint16>(item->c0);
	packet.write_null(16); // date
	packet.write_null(9); // ucc
	packet.write<uint64>(100000000); //pang
	packet.write<uint64>(100000000); //cookie
	pc->send_packet(&packet);
}