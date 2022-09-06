#include "FLComponentManager.hpp"

FLComponentManager::FLComponentManager(){
}

FLComponentManager::~FLComponentManager(){
}

void FLComponentManager::EntityDestroyed(FLEntity entity){
	// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (auto const& pair : mComponentArrays)
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
}

void FLComponentManager::cleanup() {
	for (auto& arr : mComponentArrays) {
		arr.second.reset();
	}
}
