#pragma once
#include <algorithm>
#include <list>
#include "snowflake.h"
#include "server.h"
#include "channel.h"

namespace SleepyDiscord {
	template<class Type>
	struct Cache : public std::list<Type> {
	public:
		template<class Container, class Object>
		const_iterator findOnetWithObject(Container Type::*list, const Snowflake<Object>& objectID) {
			return std::find_if(begin(), end(), [&objectID](Type& Thing) {
				auto result = std::find_if(Thing.*list.begin(), Thing.*list.end(),
					[&objectID](Object& object) {
					return object.ID == objectID;
				}
				);
				return result != Thing.*list.end();
			});
		}
	};
}