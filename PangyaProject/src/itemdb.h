#pragma once
#include "typedef.h"
#include <vector>
#include <memory>

#define NAME_MAX 40
#define DATE_MAX 16

enum {
	TYPE_INVALID = -1,
	TYPE_PANG = 0,
	TYPE_PANG2 = 2,
	TYPE_COOKIE = 1
};

enum {
	ITEMDB_NORMAL = 0,
	ITEMDB_PERIOD = 32,
	ITEMDB_SKIN_PERIOD = 33,
	ITEMDB_PART_RENT = 96,
	ITEMDB_PART_RENT_END = 98
};

enum item_type_name {
	ITEMDB_CHAR = 1,
	ITEMDB_PART = 2,
	ITEMDB_CLUB = 4,
	ITEMDB_BALL = 5,
	ITEMDB_USE = 6,
	ITEMDB_CADDIE = 7,
	ITEMDB_CADDIE_ITEM = 8,
	ITEMDB_SETITEM = 9,
	ITEMDB_SKIN = 14,
	ITEMDB_MASCOT = 16,
	ITEMDB_AUX = 28,
	ITEMDB_CARD = 31
};

struct itemdb_base {
	uint32 enable;
	uint32 item_typeid;
	char name[NAME_MAX];
	uint8 minlv;
	char preview[NAME_MAX];
	char un[3];
	uint32 price;
	uint32 discount_price;
	uint32 true_price;
	uint8 price_type;
	uint8 flag;
	uint8 time_flag;
	uint8 timing;
	uint32 tp_item;
	uint32 tp_count;
	uint16 mileage;
	uint16 bonusMileage;
	uint16 mileage1;
	uint16 mileage2;
	uint32 tiki_point;
	uint32 tiki_pang;
	uint32 activedate;
	char date_start[DATE_MAX];
	char date_end[DATE_MAX];
};

struct itemdb_normal {
	struct itemdb_base base;
	uint32 type;
	char mpet[NAME_MAX];
	uint16 status[5];
};

struct itemdb_part {
	struct itemdb_base base;
	char mpet[NAME_MAX];
	uint32 ucctype;
	uint32 slot_total;
	uint32 un1; // ?
	char texture[6][NAME_MAX];
	uint16 status[5];
	uint16 slot[5];
	char un2[48];
	uint32 un3, un4;
	uint32 rent_price;
	uint32 un5;
};

struct itemdb_club {
	struct itemdb_base base;
	uint32 club_typeid[4];
	uint16 status[5];
	uint16 max_status[5];
	uint32 club_type;
	uint32 club_special_status;
	uint32 recovery_limit;
	float rate_workshop;
	uint32 un1;
	uint16 transfer;
	uint16 flag;
	uint32 un2[2];
};


struct itemdb_ball {
	struct itemdb_base base;
	uint32 un1;
	char mpet[NAME_MAX];
	uint32 un2[2];
	char un3[560];
	uint16 status[5];
	uint16 un4;
};

class ItemDB {
private:
	void readdb_normal();
	void readdb_part();
	void readdb_club();
	void readdb_ball();
public:
	ItemDB();

	std::vector<std::shared_ptr<itemdb_normal>> item_;
	std::vector<std::shared_ptr<itemdb_part>> part_;
	std::vector<std::shared_ptr<itemdb_club>> club_;
	std::vector<std::shared_ptr<itemdb_ball>> ball_;
	
	bool exists(uint32 id);
	bool buyable(uint32 id);
	uint32 get_amount(uint32 id);
	std::pair<char, uint32> get_price(uint32 id);
};

extern ItemDB* itemdb;