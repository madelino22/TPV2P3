// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include <initializer_list>
#include <vector>
#include <list>
#include <type_traits>

#include "ecs.h"
#include "Entity.h"
#include "System.h"
#include "../game/messages.h"
class Manager {
public:

	Manager();
	virtual ~Manager();

	void resetGroups(Entity *e) {
		e->groups_.reset();
	}

	template<typename GT>
	inline void setGroup(Entity *e, bool status) {
		assert(e != nullptr);
		e->groups_[ecs::grpIdx<GT>] = status;
	}

	template<typename GT>
	inline bool hasGroup(Entity *e) {
		assert(e != nullptr);
		return e->groups_[ecs::grpIdx<GT>];
	}

	template<typename T, typename ...Ts>
	inline T* addComponent(Entity *e, Ts &&...args) {
		T *c = new T(std::forward<Ts>(args)...);
		e->_cmps[ecs::cmpIdx<T>] = std::unique_ptr<Component>(c);
		return c;
	}

	template<typename T>
	inline T* getComponent(Entity *e) {
		return static_cast<T*>(e->_cmps[ecs::cmpIdx<T>].get());
	}

	template<typename T>
	inline bool hasComponent(Entity *e) {
		return e->_cmps[ecs::cmpIdx<T>].get() != nullptr;
	}

	template<typename T>
	inline void removeComponent(Entity *e) {
		e->_cmps[ecs::cmpIdx<T>].get() = nullptr;
	}

	// entities
	Entity* addEntity() {
		Entity *e = new Entity();
		if (e != nullptr) {
			resetGroups(e);
			e->active_ = true;
			entities_.emplace_back(e);
		}
		return e;
	}

	inline bool isActive(Entity *e) const {
		return e->active_;
	}

	inline void setActive(Entity *e, bool state) {
		e->active_ = state;
	}

	// handlers
	template<typename T>
	inline void setHandler(Entity *e) {
		hdlrs_[ecs::hdlrIdx<T>] = e;
	}

	template<typename T>
	inline Entity* getHandler() {
		return hdlrs_[ecs::hdlrIdx<T>];
	}

	inline const std::vector<Entity*>& getEnteties() {
		return entities_;
	}

	// systems
	template<typename T, typename ...Ts>
	inline T* addSystem(Ts &&...args) {
		T *s = new T(std::forward<Ts>(args)...);
		sys_[ecs::sysIdx<T>] = std::unique_ptr<System>(s);
		s->setMngr(this);
		s->init();
		return s;
	}

	template<typename T>
	inline T* getSystem() {
		return static_cast<T*>(sys_[ecs::sysIdx<T>].get());
	}

	// message
	void send(const Message &msg) {
		// will make a copy of msg and store it in msgsQueue_
		// **IMPORTANT:
		//    -- Manager.h needs to see the actual type of Message, so
		//       add #include "messages" in "game/ecs_defs.h"
		//    -- This will not work if message are defined with
		//       becuase emplace_back needs to copy the object but we don't
		//       know the actual type --- see other versions of ecs that do this
		msgsQueue_.emplace_back(msg);
	}

	void flushMsgsQueue() {

		// will also send those that are sent as a reaction while sending the current
		// messages in msgsQueue_
		for (auto j = 0u; j < msgsQueue_.size(); j++) {
			auto &m = msgsQueue_[j];
			for (auto i = 0u; i < sys_.size(); i++) {
				if (sys_[i] != nullptr)
					sys_[i]->receive(m);
			}
		}
		msgsQueue_.clear();
	}

	void refresh();

private:

	std::vector<Entity*> entities_;
	std::array<Entity*, ecs::maxHdlr> hdlrs_ = { };
	std::array<std::unique_ptr<System>, ecs::maxSystem> sys_ = { };

	std::vector<Message> msgsQueue_;
};
