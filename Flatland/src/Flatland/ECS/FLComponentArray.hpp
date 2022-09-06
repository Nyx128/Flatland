#pragma once
#include "FLEntity.hpp"
#include <array>
#include <unordered_map>
#include "core/asserts.hpp"


class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(FLEntity entity) = 0;
};

template<typename T>
class FLComponentArray :public IComponentArray{
public:
	FLComponentArray() {

	}

	~FLComponentArray() {

	}

	template<typename T>
	void InsertData(FLEntity entity, T component) {
		FL_ASSERT_MSG(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end(), "Trying to add the same component to an entity again");

		size_t newIndex = mSize;
		mIndexToEntityMap[newIndex] = entity;
		mEntityToIndexMap[entity] = newIndex;
		mComponentArray[newIndex] = component;
		mSize++;
	}


	void RemoveData(FLEntity entity) {
		FL_ASSERT_MSG(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end(), "Trying to remove non-existant component");

		//the plan is to remove the component at the index and replace it with the data at the last index
		//then update the IndexToEntity and EntityToIndex map to point to this data
		//all to keep the data contiguous with no holes

		size_t removedEntityIndex = mEntityToIndexMap[entity];
		size_t lastIndex = mSize - 1;
		mComponentArray[removedEntityIndex] = mComponentArray[lastIndex];

		//update the map to point to the new location which we moved the last entity to
		//and move the removed entity and index to the index of the last element
		FLEntity entityOfLastElement = mIndexToEntityMap[lastIndex];
		mEntityToIndexMap[entityOfLastElement] = removedEntityIndex;
		mIndexToEntityMap[removedEntityIndex] = entityOfLastElement;

		//remove the last entity in the map since its been moved
		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(lastIndex);
	}

	void EntityDestroyed(FLEntity entity) {
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
		{
			// Remove the entity's component if it existed
			RemoveData(entity);
		}
	}

	T& GetData(FLEntity entity)
	{ 
		FL_ASSERT_MSG(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() ,"Retrieving non-existent component.");

		// Return a reference to the entity's component
		return mComponentArray[mEntityToIndexMap[entity]];
	}

private:
	// The packed array of components (of generic type T),
	// set to a specified maximum amount, matching the maximum number
	// of entities allowed to exist simultaneously, so that each entity
	// has a unique spot.
	std::array<T, MAX_ENTITIES> mComponentArray;

	// Map from an entity ID to an array index.
	std::unordered_map<FLEntity, size_t> mEntityToIndexMap;

	// Map from an array index to an entity ID.
	std::unordered_map<size_t, FLEntity> mIndexToEntityMap;

	// Total size of valid entries in the array.
	size_t mSize = 0;
};
