// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <SDL_stdinc.h>

#include "GameManagerSystem.h"

enum MsgId : Uint8 {
	_BALL_EXIT, _ROUND_START, _ROUND_OVER, _GAME_OVER
};

struct Message {
	Message(MsgId id) :
			id_(id) {
	}
	MsgId id_;

	virtual ~Message() {
	}
	virtual Message* clone() const {
		return new Message(*this);
	}
};

struct BallExitMsg: Message {
	BallExitMsg(GameManagerSystem::Side side) :
			Message(_BALL_EXIT), side_(side) {
	}

	GameManagerSystem::Side side_;

	virtual ~BallExitMsg() {
	}
	virtual BallExitMsg* clone() const {
		return new BallExitMsg(*this);
	}

};
