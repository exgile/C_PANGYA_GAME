#pragma once

#include "reader/ini_reader.h"

class Config {
public:
	Config();
	~Config();
	INIReader* read;
};

extern Config* config;