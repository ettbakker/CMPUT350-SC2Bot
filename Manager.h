#pragma once
#include "Command.h"

namespace sc2 {
	class Manager {
		class BasicSc2Bot;
	public:
		Manager(BasicSc2Bot* bot) : bot(bot) { }
		virtual ~Manager() { }
		void ExecuteCommand(const Command& cmd) {
			
		}

	private:
		BasicSc2Bot* bot;
	};

}