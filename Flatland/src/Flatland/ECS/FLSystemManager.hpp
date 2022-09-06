#pragma once
#include "FLSystem.hpp"
#include <memory>
#include <unordered_map>
#include "core/asserts.hpp"

class FLSystemManager {
public:
	FLSystemManager();
	~FLSystemManager();

	template<typename T>
	inline std::shared_ptr<T> RegisterSystem() {
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

		// Create a pointer to the system and return it so it can be used externally
		auto system = std::make_shared<T>();
		mSystems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	inline void SetSignature(Signature signature) {
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

		// Set the signature for this system
		mSignatures.insert({ typeName, signature });
	}


	void EntityDestroyed(FLEntity entity);

	void EntitySignatureChanged(FLEntity entity, Signature entitySignature);
private:
	// Map from system type string pointer to a signature
	std::unordered_map<const char*, Signature> mSignatures{};

	// Map from system type string pointer to a system pointer
	std::unordered_map<const char*, std::shared_ptr<FLSystem>> mSystems{};
};
