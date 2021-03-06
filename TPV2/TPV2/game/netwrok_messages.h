// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include "network_types.h"

#pragma pack(push,1)

enum MsgType : uint8_nt {
	_NONE_ = 0, //
	_I_WANT_TO_PLAY_, //
	_WELCOME_, //
	_FIGHTER_POS, //
	_FIGHTER_DEATH_, //
	_START_GAME_REQUEST_,
	_STATE_CHANGED_, //
	_SHOT_INFO_, //
	_DISCONNECTED_, //
	_ACTIVE_
	
};

struct NetworkMessage {
	uint8_nt _type;
};

struct DissConnectMsg: NetworkMessage {
	uint8_nt id;
};


//es un mensaje vac?o, la ?nica infomaci?n que queremos es saber cuando se ha destuido el caza, que se obtiene al recibir el mensaje
struct FighterDeathMsg: NetworkMessage {
};


struct PlayRequestMsg: NetworkMessage {
	uint8_nt name[10];
};

struct WelcomeMsg: NetworkMessage {
	uint8_nt id;
	uint8_nt name[10];
};

struct FighterTransformMsg: NetworkMessage {
	uint8_nt id;
	float32_nt x;
	float32_nt y;
	float32_nt r;
};


struct ShootMessage: NetworkMessage {
	float32_nt pos_x;
	float32_nt pos_y;
	float32_nt vel_x;
	float32_nt vel_y;
};



struct StateChangedMessage: NetworkMessage {
	uint8_nt state_;
	uint8_nt left_score_;
	uint8_nt right_score_;
};


#pragma pack(pop)

