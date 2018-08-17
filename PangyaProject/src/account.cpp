#include "account.h"
#include "pc.h"
#include "clif.h"
#include "typedef.h"
#include "utils.h"
#include "pc_manager.h"
#include "reader.h"
#include "channel.h"
#include "struct.h"
#include "static.h"
#include "common/db.h"
#include "common/packet.h"
#include "inventory.h"

using namespace Poco::Data::Keywords;
using namespace Poco::Data;

account* pc_process = nullptr;

account::account(){}
account::~account(){}

void account::pc_login(pc* pc) {
	std::string username = pc->read<std::string>();
	int account_id = pc->read<int>();
	pc->skip(6);
	std::string login_key = pc->read<std::string>();
	std::string game_ver = pc->read<std::string>();
	pc->skip(8);
	std::string game_key = pc->read<std::string>();

	Poco::Data::Session sess = sdb->get_session();
	Poco::Data::Statement stm(sess);
	stm << "SELECT account_id, userid, name, capability, sex FROM account WHERE account_id=? AND userid=? AND login_key=? AND game_key=?", 
		use(account_id), use(username), use(login_key), use(game_key), now;
	Poco::Data::RecordSet rs(stm);
	
	if (rs.rowCount() <= 0) {
		Packet packet;
		packet.write<uint16>(630);
		packet.write<uint32>(300);
		pc->send_packet(&packet);
		pc->disconnect();
		return;
	}

	pc->login_key = login_key;
	pc->game_key = game_key;
	pc->account_id_ = rs["account_id"];
	pc->username_ = rs["userid"].toString();
	pc->name_ = rs["name"].toString();
	pc->capability_ = rs["capability"];
	pc->sex_ = rs["sex"];
	
	sys_send_jerk(pc);
	sys_send_pc_data(pc);
	channel_manager->send_channel(pc);

	pc->inventory->load_character(pc);
	pc->inventory->load_item(pc);
	pc->inventory->load_club_data(pc);
	pc->inventory->load_equipment(pc);

	pc->inventory->send_char(pc);
	pc->inventory->send_item(pc);
	pc->inventory->send_equipment(pc);

	// temporarily sync money
	pc->sync_money();
}

void account::sys_send_pc_data(pc* pc) {
	Packet packet;
	packet.write<uint16>(0x44);
	packet.write<uint8>(0);
	packet.write<std::string>(config->read->Get("server", "client_Ver", "829.01"));
	packet.write<uint16>(65535);
	packet.write_string(pc->username_, 15);
	packet.write_null(7);
	packet.write_string(pc->name_, 16);
	packet.write_null(6);
	packet.write_null(21); // guild name
	packet.write_null(9); // guild image
	packet.write_null(7);
	packet.write<uint32>(pc->capability_);
	packet.write<uint32>(0);
	packet.write<uint32>(pc->connection_id_);
	packet.write_null(12);
	packet.write<uint32>(0); // guild id
	packet.write_hex(&pc_data1[0], sizeof(pc_data1));
	packet.write_string(pc->username_ + "@NT", 18);
	packet.write_null(0x6e);
	packet.write<uint32>(pc->account_id_);
	packet.write_null(239); // TODO: statistic
	packet.write_hex(&pc_data2[0], sizeof(pc_data2));
	packet.write_time();
	packet.write_hex(&pc_data3[0], sizeof(pc_data3));
	packet.write_null(8);
	packet.write<uint8>(1);
	packet.write<uint32>(0);
	packet.write<uint8>(8); // grandprix? 0 = no grandprix
	packet.write<uint16>(0);
	
	packet.write<uint32>(0); // guild id
	packet.write_string("", 20); // guild name
	packet.write_null(9);
	packet.write<uint32>(0); // guild total members
	packet.write_string("", 9); // guild image to file
	packet.write_null(3);
	packet.write_string("", 0x65); // guild notice
	packet.write_string("", 101); // guild introduce
	packet.write<uint32>(0); // my current position
	packet.write<uint32>(0); // guild leader account_id
	packet.write_string("", 22); // guild leader nick

	pc->send_packet(&packet);
}

void account::sys_send_jerk(pc* pc) {
	Packet packet;
	packet.write_hex(&game_login1[0], sizeof(game_login1));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login2[0], sizeof(game_login2));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login3[0], sizeof(game_login3));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login4[0], sizeof(game_login4));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login5[0], sizeof(game_login5));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login6[0], sizeof(game_login6));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login7[0], sizeof(game_login7));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login8[0], sizeof(game_login8));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login9[0], sizeof(game_login9));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login10[0], sizeof(game_login10));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login11[0], sizeof(game_login11));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login12[0], sizeof(game_login12));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login13[0], sizeof(game_login13));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login14[0], sizeof(game_login14));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login15[0], sizeof(game_login15));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login16[0], sizeof(game_login16));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login17[0], sizeof(game_login17));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login18[0], sizeof(game_login18));
	pc->send_packet(&packet);

	packet.reset();
	packet.write_hex(&game_login19[0], sizeof(game_login19));
	pc->send_packet(&packet);
}