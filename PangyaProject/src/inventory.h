#pragma once

#include <string>
#include <vector>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTime.h>
#include "typedef.h"
#include "itemdb.h"

#define MAX_AMOUNT_ITEM 30000

class pc;

enum e_checkitem_result {
	CHECKITEM_PASS,
	CHECKITEM_FAIL,
	CHECKITEM_OVERLIMIT,
	CHECKITEM_DUPLICATED
};

enum e_additem_result {
	ADDITEM_SUCCESS,
	ADDITEM_DUPLICATED,
	ADDITEM_INVALID,
	ADDITEM_OVERWEIGHT,
	ADDITEM_OVERITEM,
	ADDITEM_OVERAMOUNT,
	ADDITEM_STACKLIMIT
};

enum find_by {
	FIND_BY_ID,
	FIND_BY_TYPEID
};

struct item {
	uint8 item_type;
	uint32 type_id;
	uint16 amount;
	uint16 expire_day;
	uint8 flag;
};

struct INV_TRANSACTION {
	uint8 type = 2;
	uint32 item_id;
	uint32 item_typeid;
	uint32 old_amount = 0;
	uint32 new_amount = 0;
	uint32 timestamp_reg = 0;
	uint32 timestamp_end = 0;
	std::string ucc_unique = "";
	uint8 ucc_status = 0;
	uint8 ucc_copycount = 0;
	uint16 c0, c1, c2, c3, c4 = 0;
	uint32 club_point = 0;
	uint32 club_count = 0;
	uint32 club_cancelcount = 0;
	uint32 card_typeid = 0;
	uint8 char_slot = 0;
};

struct INV_ITEM {
	uint32 id;
	uint32 item_typeid;
	uint16 hour_left = 0;
	uint16 c0, c1, c2, c3, c4 = 0;
	uint8 flag = 0;
	uint8 type = 0;
	std::string ucc_name = "";
	std::string ucc_key = "";
	uint8 ucc_state = 0;
	uint32 ucc_copy_count = 0;
	std::string ucc_drawer_name = "";
	Poco::LocalDateTime reg_date;
	Poco::LocalDateTime end_date;
	bool valid = true;
	bool sync = false;
};

struct INV_CLUB_DATA {
	uint32 item_id;
	uint8 c0, c1, c2, c3, c4;
	uint32 point;
	uint32 work_count;
	uint32 cancel_count;
	uint32 point_total;
	uint32 pang_total;
};

struct INV_CHAR {
	uint32 id;
	uint32 char_typeid;
	uint8 hair_color;
	uint8 flag;
	uint8 c0, c1, c2, c3, c4;
	bool valid = true;
	bool sync = false;
};

struct INV_CHAR_EQUIP {
	uint32 item_id;
	uint32 item_typeid;
	uint32 char_id;
	uint8 num;
	bool sync = false;
};

struct INV_MASCOT {
	uint32 id;
	uint32 mascot_typeid;
	std::string message;
	uint16 end_date_count;
	Poco::LocalDateTime date_end;
	bool valid = true;
	bool sync = false;
};

struct INV_CARD {
	uint32 id;
	uint32 card_typeid;
	uint32 amout;
	bool valid = true;
	bool sync = false;
};

struct INV_CARD_CHAR_EQUIP {
	uint32 id;
	uint32 char_id;
	uint32 char_typeid;
	uint32 card_typeid;
	uint8 slot;
	uint8 flag;
	Poco::LocalDateTime date_in;
	Poco::LocalDateTime date_end;
	bool valid = true;
	bool sync = false;
};

struct PC_EQUIPMENT {
	uint32 caddie_id;
	uint32 char_id;
	uint32 club_id;
	uint32 ball_id;
	uint32 item_slot[10];
	uint32 title_id;
	uint32 mascot_id;
	uint32 poster1;
	uint32 poster2;
};

class Inventory {
private:
	void show_buyitem(pc* pc, std::shared_ptr<INV_ITEM> const& item);
public:
	std::vector<std::shared_ptr<INV_ITEM>> item_;
	std::vector<std::shared_ptr<INV_CLUB_DATA>> club_;	
	std::vector<std::unique_ptr<INV_MASCOT>> mascot;
	std::vector<std::unique_ptr<INV_CARD>> card;
	std::vector<std::unique_ptr<INV_CARD_CHAR_EQUIP>> card_equipment;
	std::vector<std::shared_ptr<INV_CHAR>> character_;
	std::vector<std::shared_ptr<INV_CHAR_EQUIP>> character_equip_;
	std::vector<std::shared_ptr<INV_TRANSACTION>> transaction_;
	PC_EQUIPMENT equipment;

	uint32 sys_cal_hour_left(std::shared_ptr<INV_ITEM> const& item);
	bool sys_is_rent(uint8 rent_flag);

	char checkitem(pc* pc, uint32 id, uint32 amount = 1);
	bool pc_item_exists(uint32 val, enum item_type_name find_type, enum find_by by_);

	char additem(pc* pc, struct item* item, bool transaction, bool from_shop = false);
	char addChar(pc* pc, struct item* item, bool transaction, bool from_shop);
	char addUse(pc* pc, struct item* item, bool transaction, bool from_shop);

	void add_transaction(uint8 types, std::shared_ptr<INV_CHAR> const& char_);
	void add_transaction(uint8 types, std::shared_ptr<INV_ITEM> const& pitem_, uint32 old_amount);

	void load_character(pc* pc);
	void send_char(pc* pc);

	void load_item(pc* pc);
	void send_item(pc* pc);
	void load_club_data(pc* pc);

	void load_equipment(pc* pc);
	void send_equipment(pc* pc);
};