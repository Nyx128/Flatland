#pragma once
#include "FLEntity.hpp"
#include <queue>
#include <array>

class FLEntityManager {
public:
	FLEntityManager();
	~FLEntityManager();

	FLEntity CreateEntity();
	void DestroyEntity(FLEntity entity);
	void SetSignature(FLEntity entity, Signature signature);
	Signature GetSignature(FLEntity entity);
private:
	std::queue<FLEntity> mAvailableEntities;
	std::array<Signature, MAX_ENTITIES> mSignatures;

	uint32_t mLivingEntityCount = 0;
};