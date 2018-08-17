#include "shop.h"
#include "pc.h"
#include "itemdb.h"
#include "common/packet.h"

ShopSystem* shop = nullptr;

void ShopSystem::pc_entershop(pc* pc) {
	Packet packet;
	packet.write<uint16>(0x20e);
	packet.write_null(8); // ??
	pc->send_packet(&packet);
}

void ShopSystem::pc_buyitem(pc* pc) {
	uint8 buy_type = pc->read<uint8>();
	
	switch (buy_type) {
	case BUY_NORMAL:
		pc_buyitem_normal(pc);
		break;
	case BUY_RENT:
		pc_buyitem_rent(pc);
		break;
	}
}

void ShopSystem::pc_buyitem_normal(pc* pc) {
	uint16 total = pc->read<uint16>();
	uint32 item_amount = 0, pang_amount = 0, cookie_amount = 0;

	buy_data buy[MAX_BUY];

	for (int i = 0; i < total; ++i) {
		buy[i] = pc->read_struct<buy_data>();
	}

	for (int i = 0; i < total; ++i) {
		// item exist?
		if (!itemdb->exists(buy[i].item_typeid)) {
			send_msg(pc, BUY_FAIL);
			return;
		}
		// item buyable?
		else if (!itemdb->buyable(buy[i].item_typeid)) {
			send_msg(pc, CANNOT_BUY_ITEM);
			return;
		}
	}

	// validate that pang and cookie
	for (int i = 0; i < total; ++i) {
		std::pair<char, uint32> price_data = itemdb->get_price(buy[i].item_typeid);
		switch (price_data.first) {
		case TYPE_PANG:
		case TYPE_PANG2:
			pang_amount += price_data.second * buy[i].amount;
			break;
		case TYPE_COOKIE:
			cookie_amount += price_data.second;
			break;
		}
	}

	// you should validate that pc have enought pang and cookie here ?
	// ????


	// validate item that pc can have
	for (int i = 0; i < total; ++i) {
		item_amount = itemdb->get_amount(buy[i].item_typeid) * buy[i].amount;
		switch (pc->inventory->checkitem(pc, buy[i].item_typeid, item_amount)) {
		case CHECKITEM_PASS:
			break;
		case CHECKITEM_FAIL:
			send_msg(pc, BUY_FAIL);
			return;
			break;
		case CHECKITEM_OVERLIMIT:
			send_msg(pc, TOO_MUCH_ITEM);
			return;
			break;
		case CHECKITEM_DUPLICATED:
			send_msg(pc, ALREADY_HAVEITEM);
			return;
			break;
		}
	}

	for (int i = 0; i < total; ++i) {
		item_amount = itemdb->get_amount(buy[i].item_typeid) * buy[i].amount;
		item item;
		item.type_id = buy[i].item_typeid;
		item.amount = item_amount;
		pc->inventory->additem(pc, &item, false, true);
	}

	send_msg(pc, BUY_FAIL);
}

void ShopSystem::pc_buyitem_rent(pc* pc) {
	uint16 total = pc->read<uint16>();
}

void ShopSystem::send_msg(pc* pc, uint32 code) {
	Packet packet;
	packet.write<uint16>(0x68);
	packet.write<uint32>(code);
	pc->send_packet(&packet);
}