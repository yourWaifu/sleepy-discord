#pragma once
#include <algorithm>
#include <unordered_map>
#include "snowflake.h"
#include "server.h"
#include "channel.h"

namespace SleepyDiscord {
	template<class _Type>
	using CacheParent = std::unordered_map<typename Snowflake<_Type>::RawType, _Type>;

	//This is basicity an unordered_map made to work with the library
	//only works with Discord Object with an id
	template<class _Type>
	struct Cache : public CacheParent<_Type> {
	public:
		using Type = _Type;
		using Parent = CacheParent<_Type>;
		using std::unordered_map<typename Snowflake<_Type>::RawType, _Type>::unordered_map;
		using Key = typename Snowflake<Type>::RawType;
		Cache() : Parent() {}
		Cache(Parent map) : Parent(map) {}
		template<class InputIterator>
		Cache(InputIterator first, InputIterator last) {
			for (InputIterator it = first; it != last; ++it) {
				emplace(*it);
			}
		}

		class const_iterator {
		public:
			using Value = typename Parent::const_iterator;
			//standard iterator values
			using self_type = const_iterator;
			using value_type = Type;
			using reference = Type&;
			using pointer = Type*;
			using iterator_category = typename Value::iterator_category;
			using difference_type = typename Value::difference_type;
			const_iterator(Value iter) : value(iter) {}
			inline self_type operator++() { return value++; }
			inline self_type operator++(int junk) { return value.operator++(junk); }
			inline reference operator*() { return value->second; }
			inline pointer operator->() { return &value->second; }
			inline bool operator==(const self_type& right) { return value == right.value; }
			inline bool operator!=(const self_type& right) { return value != value.value; }
			inline Value getParent() { return value; }
		private:
			Value value;
		};

		class iterator {
		public:
			using Value = typename Parent::iterator;
			//standard iterator values
			using self_type = iterator;
			using value_type = Type;
			using reference = Type&;
			using pointer = Type*;
			using iterator_category = typename Value::iterator_category;
			using difference_type = typename Value::difference_type;
			iterator(Value iter) : value(iter) {}
			inline self_type operator++() { return value++; }
			inline self_type operator++(int junk) { return value.operator++(junk); }
			inline reference operator*() { return value->second; }
			inline pointer operator->() { return &value->second; }
			inline bool operator==(const self_type& right) { return value == right.value; }
			inline bool operator!=(const self_type& right) { return value != right.value; }
			inline operator const_iterator() { return const_iterator(value); }
			inline Value getParent() { return value; }
		private:
			Value value;
		};

		inline iterator begin() {
			return iterator(Parent::begin());
		}

		inline iterator end() {
			return iterator(Parent::end());
		}

		inline const_iterator begin() const {
			return const_iterator(Parent::begin());
		}

		inline const_iterator end() const {
			return const_iterator(Parent::begin());
		}

		//Worse then linear time
		template<class Container, class Object>
		const_iterator findOneWithObject(Container Type::*list, const Snowflake<Object>& objectID) {
			return const_iterator(
				std::find_if(Parent::begin(), Parent::end(), [list, &objectID](typename Parent::value_type& thing) {
					auto result = objectID.findObject(thing.second.*list);
					return result != (thing.second.*list).end();
				})
			);
		}

		std::pair<iterator,bool> insert(Type& value) {
			std::pair<typename Parent::iterator,bool> pair = Parent::insert(typename Parent::value_type(value.ID, value));
			return {iterator(pair.first), pair.second};
		}

		template<class... Args>
		std::pair<iterator, bool> emplace(const json::Value& value) {
			const json::Value& ID = value["id"];
			std::pair<typename Parent::iterator,bool> pair = Parent::emplace(std::string(ID.GetString(), ID.GetStringLength()), value);
			return {iterator(pair.first), pair.second};
		}

		iterator find(const Key& key) {
			return iterator(Parent::find(key));
		}

		const_iterator find(const Key& key) const {
			return const_iterator(Parent::find(key));
		}

		iterator erase(const_iterator pos) {
			return iterator(Parent::erase(pos.getParent()));
		}

		iterator erase(const_iterator first, const_iterator last) {
			return iterator(Parent::erase(first.getParent(), last.getParent()));
		}

		//In the case of push_back needed
		//remove /**/
		/*
		//Does not add to the end, this is just for compatability for
		//some SleepyDiscord functions
		inline void push_back(const Type& value) {
			insert(value);
		}

		inline void push_back(Type&& value) {
			insert(value);
		}
		*/
	};
}