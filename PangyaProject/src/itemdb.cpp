#include "itemdb.h"
#include "utils.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <fstream>

ItemDB* itemdb = nullptr;

ItemDB::ItemDB() {
	readdb_normal();
	readdb_part();
	readdb_club();
	readdb_ball();
}

void ItemDB::readdb_normal() {
	std::fstream f;
	int count;

	f.open("db/Item.iff", std::fstream::in | std::fstream::binary);
	f.read((char*)&count, sizeof(count));
	f.seekg(4, std::ios::cur);
	
	for (int i = 1; i <= count; ++i) {
		std::shared_ptr<itemdb_normal> it(new itemdb_normal());
		f.read((char*)&it->base.enable, sizeof(itemdb_normal));
		item_.push_back(it);
	}

	f.close();
}

void ItemDB::readdb_part() {
	std::fstream f;
	int count;

	f.open("db/Part.iff", std::fstream::in | std::fstream::binary);
	f.read((char*)&count, sizeof(count));
	f.seekg(4, std::ios::cur);

	for (int i = 1; i <= count; ++i) {
		std::shared_ptr<itemdb_part> it(new itemdb_part());
		f.read((char*)&it->base.enable, sizeof(itemdb_part));
		part_.push_back(it);
	}

	f.close();
}

void ItemDB::readdb_club() {
	std::fstream f;
	int count;

	f.open("db/ClubSet.iff", std::fstream::in | std::fstream::binary);
	f.read((char*)&count, sizeof(count));
	f.seekg(4, std::ios::cur);

	for (int i = 1; i <= count; ++i) {
		std::shared_ptr<itemdb_club> it(new itemdb_club());
		f.read((char*)&it->base.enable, sizeof(itemdb_club));
		club_.push_back(it);
	}

	f.close();
}

void ItemDB::readdb_ball() {
	std::fstream f;
	int count;

	f.open("db/Ball.iff", std::fstream::in | std::fstream::binary);
	f.read((char*)&count, sizeof(count));
	f.seekg(4, std::ios::cur);

	for (int i = 1; i <= count; ++i) {
		std::shared_ptr<itemdb_ball> it(new itemdb_ball());
		f.read((char*)&it->base.enable, sizeof(itemdb_ball));
		ball_.push_back(it);
	}

	f.close();
}

bool ItemDB::exists(uint32 id) {
	uint8 item_type = utils::itemdb_type(id);

	if (item_type == ITEMDB_USE) {
		auto find_item = std::find_if(item_.begin(), item_.end(), [&id](std::shared_ptr<itemdb_normal> const& item) {
			return item->base.item_typeid == id;
		});
		if (find_item != item_.end()) {
			return true;
		}
	}
	else if (item_type == ITEMDB_PART) {
		auto find_part = std::find_if(part_.begin(), part_.end(), [&id](std::shared_ptr<itemdb_part> const& part) {
			return part->base.item_typeid == id;
		});
		if (find_part != part_.end()) {
			return true;
		}
	}
	else if (item_type == ITEMDB_CLUB) {
		auto find_club = std::find_if(club_.begin(), club_.end(), [&id](std::shared_ptr<itemdb_club> const& club) {
			return club->base.item_typeid == id;
		});
		if (find_club != club_.end()) {
			return true;
		}
	}
	else if (item_type == ITEMDB_BALL) {
		auto find_ball = std::find_if(ball_.begin(), ball_.end(), [&id](std::shared_ptr<itemdb_ball> const& ball) {
			return ball->base.item_typeid == id;
		});
		if (find_ball != ball_.end()) {
			return true;
		}
	}
	return false;
}

uint32 ItemDB::get_amount(uint32 id) {
	uint8 item_type = utils::itemdb_type(id);

	if (item_type == ITEMDB_USE) {
		auto find_item = std::find_if(item_.begin(), item_.end(), [&id](std::shared_ptr<itemdb_normal> const& item) {
			return item->base.item_typeid == id;
		});
		if (find_item != item_.end()) {
			return (*find_item)->status[0] == 0 ? 1 : (*find_item)->status[0];
		}
	}

	return 1;
}

std::pair<char, uint32> ItemDB::get_price(uint32 id) {
	uint8 item_type = utils::itemdb_type(id);

	if (item_type == ITEMDB_USE) {
		auto find_item = std::find_if(item_.begin(), item_.end(), [&id](std::shared_ptr<itemdb_normal> const& item) {
			return item->base.item_typeid == id;
		});
		if (find_item != item_.end()) {
			return std::make_pair((*find_item)->base.price_type, (*find_item)->base.true_price > 0 ? (*find_item)->base.true_price : (*find_item)->base.price);
		}
	}

	return std::make_pair(TYPE_INVALID, 0);
}

bool ItemDB::buyable(uint32 id) {
	uint8 item_type = utils::itemdb_type(id);

	if (item_type == ITEMDB_USE) {
		auto find_item = std::find_if(item_.begin(), item_.end(), [&id](std::shared_ptr<itemdb_normal> const& item) {
			return item->base.item_typeid == id;
		});
		if (find_item != item_.end()) {
			return (*find_item)->base.flag & 1;
		}
	}

	return false;
}