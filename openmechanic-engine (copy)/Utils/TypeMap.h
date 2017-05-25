#pragma once

#include <memory>
#include <list>
#include <stdexcept>

/**
* @brief base class for type map entries
*/
struct AbstractTypeDataContainer {
	typedef std::shared_ptr<AbstractTypeDataContainer> Ptr;

	inline virtual ~AbstractTypeDataContainer() {
	}
};

/**
* concrete implimentation of a type map entry for a type. Basically boxes the type.
*/
template<class T>
struct ConcreateTypeDataContainer : public AbstractTypeDataContainer {
	typedef std::shared_ptr<ConcreateTypeDataContainer> Ptr;

	ConcreateTypeDataContainer(const T& data) : data(data) {
	}

	ConcreateTypeDataContainer() : data() {
	}

	T data;
};

/**
* Map mapping T -> DataType<T> where T can be an arbitrary type determined during run-time.
*
* Example usage with DataType = std::vector :
*      TypeMap<std::vector> map;
*      map.put<int>(std::vector<int>({1, 2, 3}));
*      map.put<float>(std::vector<float>({3.14, 2.72}));
*
*      std::vector<int> intVec = map.get<int>(); // intVec now contains [1,2,3]
*      std::vector<float> floatVec = map.get<float>(); // floatVec contains [3.14, 2.72]
*
*      bool hasBoolVec = map.exists<bool>(); // returns false, since we didn't put a value for bool before
*      std::vector<bool> boolVec = map.get<bool>(); // causes an std::runtime_error exception since no entry for bool exists
*/
template<template <typename> class DataType>
class TypeMap {
public:

	/**
	* @brief put a value under given key. Will override previous value if any. Linar in map size.
	* @param value falue to put under key Key
	* @return true if entry for this key is new, false if there was an entry before and was overridden
	*/
	template<class Key>
	bool put(const DataType<Key>& value) {
		typename ConcreateTypeDataContainer<DataType<Key>>::Ptr container = getContainer<Key>();
		if (container) {
			container->data = value;
			return false;
		}
		else {
			// create new container
			typename ConcreateTypeDataContainer<DataType<Key>>::Ptr container(new ConcreateTypeDataContainer<DataType<Key>>(value));
			containers.push_back(container);
			return true;
		}
	}

	/**
	* @brief Linar in map size.
	* @return mapped value for given type.
	* @throw std::runtime_error if no value for Key exists
	*/
	template<class Key>
	DataType<Key>& get() const {
		typename ConcreateTypeDataContainer<DataType<Key>>::Ptr container = getContainer<Key>();
		if (container) {
			return container->data;
		}
		else {
			throw std::runtime_error(std::string("No Container for type ") + typeid(Key).name());
		}
	}

	template<class Key>
	DataType<Key>& getCreate() {
		typename ConcreateTypeDataContainer<DataType<Key>>::Ptr container = getContainer<Key>();

		if (!container) {
			container = std::make_shared<ConcreateTypeDataContainer<DataType<Key>>>();
			containers.push_back(container);
		}

		return container->data;
	}

	/**
	* @brief checks if entry for given type exists. Linar in map size.
	* @return true if entry for Key exists, otherwise false
	*/
	template<class Key>
	bool exists() const {
		typename ConcreateTypeDataContainer<DataType<Key>>::Ptr container = getContainer<Key>();
		return !!container;
	}

private:
	// TODO FIXME: use map
	std::list<AbstractTypeDataContainer::Ptr> containers;

	/**
	* @brief
	* @return entry container for given Key or null pointer if no entry for Key exists
	*/
	template<class Key>
	typename ConcreateTypeDataContainer<DataType<Key>>::Ptr getContainer() const {
		for (auto c : containers) {
			typename ConcreateTypeDataContainer<DataType<Key>>::Ptr containerPtr = std::dynamic_pointer_cast<ConcreateTypeDataContainer<DataType<Key>>>(c);
			if (containerPtr) {
				return containerPtr;
			}
		}

		return typename ConcreateTypeDataContainer<DataType<Key>>::Ptr();
	}
};