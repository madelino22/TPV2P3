// This file is part of the course TPV2@UCM - Samir Genaim

#include "NetworkSystem.h"

#include <SDL_net.h>

#include "../ecs/Manager.h"
#include "GameManagerSystem.h"
#include "netwrok_messages.h"
#include "../utils/Vector2D.h"
#include "FighterSystem.h"




NetworkSystem::NetworkSystem(const char *host, Uint16 port,
		std::string playerName) :
		host_(host), //
		port_(port), //
		isMaster_(false), //
		isGameReday_(false), //
		id_(0), //
		conn_(), //
		p_(nullptr), //
		m_(nullptr), //
		otherPlayerAddress_(), //
		localPlayerName_(playerName), //
		remotePlayerName_("N/A"), //
		lastTimeActive_(0) {
}

NetworkSystem::~NetworkSystem() {

	
	if (conn_) {
		DissConnectMsg *m = static_cast<DissConnectMsg*>(m_);
		m->_type = _DISCONNECTED_;
		m->id = id_;
		p_->len = sizeof(DissConnectMsg);
		p_->address = otherPlayerAddress_;
		SDLNet_UDP_Send(conn_, -1, p_);
	}

	// close the connection
	if (conn_)
		SDLNet_UDP_Close(conn_);

	// free the packet
	if (p_) {
		SDLNet_FreePacket(p_);
	}

	// close SDLNet
	SDLNet_Quit();
}

void NetworkSystem::init() {

	// Initialise SDLNet
	if (SDLNet_Init() < 0) {
		throw SDLNet_GetError();
	}

	// allocate the packet to be used for sending data
	p_ = SDLNet_AllocPacket(100);
	m_ = reinterpret_cast<NetworkMessage*>(p_->data);

	if (host_ == nullptr) { // if started as a master player

		// we use id 0, and open a socket to send/receive messages
		isMaster_ = true;
		id_ = 0;
		conn_ = SDLNet_UDP_Open(port_);
		if (!conn_)
			throw SDLNet_GetError();

		names_[0] = localPlayerName_;
		names_[1] = remotePlayerName_;

	} else { // if started as  other player

		// we use id 1, and open a socket to send/receive messages
		isMaster_ = false;

//		id_ = 1; la id se asigna al cliente al recibir el mensaje de vuelta de i want to play
		conn_ = SDLNet_UDP_Open(0);
		if (!conn_)
			throw SDLNet_GetError();

		std::cout << "Trying to connect to other player at " << host_ << ":"
				<< port_ << std::endl;

		// resolve the host name into an IPAdrress
		if (SDLNet_ResolveHost(&otherPlayerAddress_, host_, port_) < 0) {
			throw SDLNet_GetError();
		}

		// enviar un mensaje diciendo que quiere jugar
		PlayRequestMsg *m = static_cast<PlayRequestMsg*>(m_);
		m->_type = _I_WANT_TO_PLAY_;
		memset(m->name, 0, 10);
		auto size =
				localPlayerName_.length() > 9 ? 9 : localPlayerName_.length();
		memcpy(m->name, localPlayerName_.c_str(), size);
		p_->len = sizeof(PlayRequestMsg);
		p_->address = otherPlayerAddress_;
		SDLNet_UDP_Send(conn_, -1, p_);

		// we use socket set to allow waiting for response instead of just looping
		SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
		SDLNet_UDP_AddSocket(socketSet, conn_);

		// wait for response
		if (SDLNet_CheckSockets(socketSet, 3000)) {
			if (SDLNet_SocketReady(conn_)) {
				if (SDLNet_UDP_Recv(conn_, p_) > 0) {
					if (m_->_type == _WELCOME_) {
						//si se ha el master recibi? el mensaje _I_WANT_TO_PLAY_ env?a uno de vuelta del tipo welcome
						isGameReday_ = true;
						WelcomeMsg *m = static_cast<WelcomeMsg*>(m_);
						remotePlayerName_ = std::string(
								reinterpret_cast<char*>(m->name));
						//el mensaje trae de vuelta la id de este jugador (cliente)
						id_ = m->id;
						names_[id_] = localPlayerName_;
						names_[1 - id_] = remotePlayerName_;
					}
				}
			}
		}

		// free the socket set, won't be used anymore
		SDLNet_FreeSocketSet(socketSet);

		// if did not succeed to connect, throw an exception
		if (!isGameReday_)
			throw "Failed to connect!";

	}

}

void NetworkSystem::update() {

	m_->_type = _ACTIVE_;
	p_->len = sizeof(NetworkMessage);
	p_->address = otherPlayerAddress_;
	SDLNet_UDP_Send(conn_, -1, p_);



	// in each iteration we poll all pending message and process them
	while (SDLNet_UDP_Recv(conn_, p_) > 0) {
		lastTimeActive_ = SDL_GetTicks();
		switch (m_->_type) {

		case _I_WANT_TO_PLAY_: {
			// we accept the connection if the player is the master, and no other player is connected
			if (isMaster_ && !isGameReday_) {
				PlayRequestMsg *m = static_cast<PlayRequestMsg*>(m_);
				otherPlayerAddress_ = p_->address;
				remotePlayerName_ = std::string(
						reinterpret_cast<char*>(m->name));
				names_[1 - id_] = remotePlayerName_;
				WelcomeMsg *mr = static_cast<WelcomeMsg*>(m_);

				mr->_type = _WELCOME_;
				auto size =
						localPlayerName_.size() > 9 ?
								9 : localPlayerName_.size();
				memcpy(mr->name, localPlayerName_.c_str(), size);
				//Se env?a de vuelta la id del cliente (desde el master)
				mr->id = 1 - id_;
				p_->len = sizeof(WelcomeMsg);
				SDLNet_UDP_Send(conn_, -1, p_);
				isGameReday_ = true;
			}
			break;
		}

			// start game request
		case _START_GAME_REQUEST_: {
			if (isMaster_) {
				manager_->getSystem<GameManagerSystem>()->startGame();
			}
			break;
		}

		case _STATE_CHANGED_: {
			StateChangedMessage *m = static_cast<StateChangedMessage*>(m_);
			manager_->getSystem<GameManagerSystem>()->changeState(m->state_,
					m->left_score_, m->right_score_);
			break;
		}
		
		case _FIGHTER_DEATH_: {
			FighterDeathMsg *m = static_cast<FighterDeathMsg*>(m_);
			manager_->getSystem<GameManagerSystem>()->onFighterDeath(nullptr);
			//al meterse en este m?todo siendo el cliente solo reproducir? el sonido
			break;
		}

			//cambiar la posici?n del caza, se hace todo el rato
		case _FIGHTER_POS: {
			if (manager_->getSystem<GameManagerSystem>()->getState() == GameManagerSystem::RUNNING) {
				FighterTransformMsg* m = static_cast<FighterTransformMsg*>(m_);
				Vector2D pos(m->x, m->y);

				manager_->getSystem<FighterSystem>()->setFighterTr(m->id, pos, m->r);
			
			}
			
			break;
		}

		case _SHOT_INFO_: {
			ShootMessage *m = static_cast<ShootMessage*>(m_);
			Vector2D pos(m->pos_x, m->pos_y);
			Vector2D vel(m->vel_x, m->vel_y);
			//si la id = 0 este es el de la izquierda, asique quieres el trransform del de la derecha
			Transform* tr;
			if (id_ == 0) {
				tr = manager_->getComponent<Transform>(manager_->getHandler<RightFighter>());
			}
			else {
				tr = manager_->getComponent<Transform>(manager_->getHandler<LeftFighter>());

			}
			manager_->getSystem<BulletsSystem>()->shoot(pos, vel, 50, 50, tr);

			break;
		}

		case _DISCONNECTED_: {
			DissConnectMsg *m = static_cast<DissConnectMsg*>(m_);
			isGameReday_ = false;
			names_[1 - m->id] = remotePlayerName_ = "N/A";
			manager_->getSystem<GameManagerSystem>()->resetGame();
			if (!isMaster_) {
				SDLNet_UDP_Close(conn_);
				conn_ = SDLNet_UDP_Open(port_);
				isMaster_ = true;
			}
		}
		}
	}

	if (isGameReday_ && SDL_GetTicks() - lastTimeActive_ > 3000) {
		isGameReday_ = false;
		names_[1 - id_] = remotePlayerName_ = "N/A";
		manager_->getSystem<GameManagerSystem>()->resetGame();
		if (!isMaster_) {
			SDLNet_UDP_Close(conn_);
			conn_ = SDLNet_UDP_Open(port_);
			isMaster_ = true;
		}

	}


}

void NetworkSystem::sendFighterPos(Vector2D pos, float r) {

	// if the other player is not connected do nothing
	if (!isGameReday_)
		return;

	// we prepare a message that includes all information
	FighterTransformMsg *m = static_cast<FighterTransformMsg*>(m_);
	m->_type = _FIGHTER_POS;
	m->x = pos.getX();
	m->y = pos.getY();
	m->r = r;
	m->id = id_;

	// set the message length and the address of the other player
	p_->len = sizeof(FighterTransformMsg);
	p_->address = otherPlayerAddress_;

	// send the message
	SDLNet_UDP_Send(conn_, -1, p_);

}

void NetworkSystem::sendStartGameRequest() {
	//
	
	m_->_type = _START_GAME_REQUEST_;
	p_->len = sizeof(NetworkMessage);
	p_->address = otherPlayerAddress_;
	SDLNet_UDP_Send(conn_, -1, p_);
}

void NetworkSystem::tryShoot()
{
	
	if (!isGameReday_)
		return;

	// we prepare a message that includes all information
	ShootMessage* m = static_cast<ShootMessage*>(m_);
	m->_type = _SHOT_INFO_;
	if (id_ == 0) {
		Vector2D pos = manager_->getComponent<Transform>(manager_->getHandler<LeftFighter>())->getPos();
		m->pos_x = pos.getX();
		m->pos_y = pos.getY();
	}
	else {
		Vector2D pos = manager_->getComponent<Transform>(manager_->getHandler<RightFighter>())->getPos();
		m->pos_x = pos.getX();
		m->pos_y = pos.getY();
	}
	
	//Se le pasa al otro jugador la posici?n y velocidad de la bala que se acaba de disparar

	// set the message length and the address of the other player
	p_->len = sizeof(ShootMessage);
	p_->address = otherPlayerAddress_;

	// send the message
	SDLNet_UDP_Send(conn_, -1, p_);
}

void NetworkSystem::tryDestroy()
{
	// if the other player is not connected do nothing
	if (!isGameReday_)
		return;

	FighterDeathMsg* m = static_cast<FighterDeathMsg*>(m_);
	m->_type = _FIGHTER_DEATH_;

	p_->len = sizeof(FighterDeathMsg);
	p_->address = otherPlayerAddress_;

	// send the message
	SDLNet_UDP_Send(conn_, -1, p_);


}

void NetworkSystem::sendStateChanged(Uint8 state, Uint8 left_score,
		Uint8 right_score) {
	// if the other player is not connected do nothing
	if (!isGameReday_)
		return;

	// we prepare a message that includes all information
	StateChangedMessage *m = static_cast<StateChangedMessage*>(m_);
	m->_type = _STATE_CHANGED_;
	m->state_ = state;
	m->left_score_ = left_score;
	m->right_score_ = right_score;

	// set the message length and the address of the other player
	p_->len = sizeof(StateChangedMessage);
	p_->address = otherPlayerAddress_;

	// send the message
	SDLNet_UDP_Send(conn_, -1, p_);

}


