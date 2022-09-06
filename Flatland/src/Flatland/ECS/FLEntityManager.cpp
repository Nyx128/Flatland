#include "FLEntityManager.hpp"
#include "core/asserts.hpp"

FLEntityManager::FLEntityManager(){
	for (FLEntity e = 0; e < MAX_ENTITIES; e++) {
		mAvailableEntities.push(e);
	}
}

FLEntityManager::~FLEntityManager(){
}

FLEntity FLEntityManager::CreateEntity(){

	FL_ASSERT_MSG(mLivingEntityCount < MAX_ENTITIES, "Too many entities in existence");

	// Take an ID from the front of the queue
	FLEntity id = mAvailableEntities.front();
	mAvailableEntities.pop();
	mLivingEntityCount++;

	return id;
}

void FLEntityManager::DestroyEntity(FLEntity entity){
	FL_ASSERT_MSG(entity < MAX_ENTITIES, "Entity out of range(invalid entity id)");

	//reset its signature
	mSignatures[entity].reset();
	//put it back into the available queue
	mAvailableEntities.push(entity);
	mLivingEntityCount--;
}

void FLEntityManager::SetSignature(FLEntity entity, Signature signature){
	FL_ASSERT_MSG(entity < MAX_ENTITIES, "Entity out of range(invalid entity id)");

	mSignatures[entity] = signature;
}

Signature FLEntityManager::GetSignature(FLEntity entity){
	FL_ASSERT_MSG(entity < MAX_ENTITIES, "Entity out of range(invalid entity id)");
	return mSignatures[entity];
}
