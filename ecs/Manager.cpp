// This file is part of the course TPV2@UCM - Samir Genaim

#include "Manager.h"

Manager::Manager() {
}

Manager::~Manager() {
}

void Manager::refresh() {

	// remove dead entities from the list of entities
	entities_.erase( //
			std::remove_if( //
					entities_.begin(), //
					entities_.end(), //
					[](const ecs::uptr_entity &e) { //
						return !e->isActive();
					}), //
			entities_.end());
}

void Manager::update() {
	auto n = entities_.size();
	for (auto i = 0u; i < n; i++)
		entities_[i]->update();
}

void Manager::render() {
	auto n = entities_.size();
	for (auto i = 0u; i < n; i++)
		entities_[i]->render();
}
