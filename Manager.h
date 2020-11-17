#pragma once
#include "Command.h"
#include "BasicSc2Bot.h"


class Manager {
public:
	Manager(BasicSc2Bot* bot) : bot(bot) { }
	virtual ~Manager() { }
	void ExecuteCommand(const Command& cmd) {
		bot->ExecuteCommand(cmd);
	}

private:
	BasicSc2Bot* bot;
};