#pragma once
#include "FLComponentArray.hpp"
#include "core/asserts.hpp"
#include <array>
#include <unordered_map>
#include <memory>
#include "components/component.hpp"

class FLComponentManager {
public:
	FLComponentManager();
	~FLComponentManager();

	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

		// Add this component type to the component type map
		mComponentTypes.insert({ typeName, mNextComponentType });

		// Create a FLComponentArray pointer and add it to the component arrays map
		mComponentArrays.insert({ typeName, std::make_shared<FLComponentArray<T>>() });

		// Increment the value so that the next component registered will be different
		++mNextComponentType;
	}

	template<typename T>
	component_ID GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		// Return this component's type - used for creating signatures
		return mComponentTypes[typeName];
	}

	template<typename T>
	std::shared_ptr<FLComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<FLComponentArray<T>>(mComponentArrays[typeName]);
	}

	template<typename T>
	void AddComponent(FLEntity entity, T component)
	{
		// Add a component to the array for an entity
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(FLEntity entity)
	{
		// Remove a component from the array for an entity
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(FLEntity entity)
	{
		// Get a reference to a component from the array for an entity
		return GetComponentArray<T>()->GetData(entity);
	}

	void EntityDestroyed(FLEntity entity);
	void cleanup();//clean all the components after finishing process to avoid memory issues

private:
	// Map from type string pointer to a component type
	std::unordered_map<const char*, component_ID> mComponentTypes{};

	// Map from type string pointer to a component array
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

	// The component type to be assigned to the next registered component - starting at 0
	component_ID mNextComponentType{};

	// Convenience function to get the statically casted pointer to the FLComponentArray of type T.
};


