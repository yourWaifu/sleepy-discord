#pragma once
#include <list>
#include <utility>
#include <vector>
#include <array>
#include <tuple>
//for errrors
#include <iostream>

#define RAPIDJSON_NO_SIZETYPEDEFINE
typedef std::size_t SizeType;
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "nonstd/string_view.hpp"
//#include "json.h"

namespace SleepyDiscord {
	namespace json {
		//using Value = nonstd::string_view;
		//using Values = std::vector<Value>;

		using Value = rapidjson::Value;
		using Array = rapidjson::Value::ConstArray;
		using Values = Value;

		//Values getArray(const Value& source);
		//Values getValues(const char* source, std::initializer_list<const char *const> const &names);
		//Value getValue(const char* source, const char * name);

		const std::string createJSON(std::initializer_list<std::pair<std::string, std::string>> json);
		const std::string string(const std::string& s);
		const std::string UInteger(const uint64_t num);
		const std::string optionalUInteger(const uint64_t num);
		const std::string integer(const int64_t num);
		const std::string optionalInteger(const int64_t num);
		const std::string boolean(const bool boolean);

		template<class Type>
		const std::string createJSONArray(const std::vector<Type> source) {
			std::string target;
			target.reserve(2);	//revents crash
			for (std::string value : source) {
				if (!value.empty())
					target += ',' + value;
			}
			target[0] = '[';
			target.push_back(']');
			return target;
		}

		struct ArrayStringWrapper {
			const Value& json;
			ArrayStringWrapper(const Value& json) : json(json) {}
			inline const Value& getDoc() const { return json; }
			operator const Value&() const { return getDoc(); }
		};

		template<class TypeToConvertTo, class Base = ArrayStringWrapper>
		struct ArrayWrapper : public Base {
			using Base::Base;
			template<template<class...> class Container, typename Type = TypeToConvertTo>
			Container<Type> get() {
				auto&& doc = Base::getDoc();
				Array jsonArray = doc.template Get<Array>();
				return Container<Type>(jsonArray.begin(), jsonArray.end());
			}

			inline std::vector<TypeToConvertTo> vector() { return get<std::vector>(); }
			inline std::list  <TypeToConvertTo> list  () { return get<std::list>();   }

			//c arrays
			inline TypeToConvertTo* cArray() { return &vector()[0]; }

			operator Values() { return Base::getArray(); }
			operator std::vector<TypeToConvertTo>() { return vector(); }
			operator std::list<TypeToConvertTo>() { return list(); }
		};

		template<class Return>
		using JSONConverter = Return(*)(const Value&);

		inline const Value& leaveAlone(const Value& value) {
			return value;
		}

		inline const char* toString(const Value& value) {
			return value.GetString();
		}

		inline std::string toStdString(const Value& value) {
			return std::string(value.GetString(), value.GetStringLength());
		}

		inline nonstd::string_view toStdStringView(const Value& value) {
			return nonstd::string_view(value.GetString(), value.GetStringLength());
		}

		inline int toInt(const Value& value) {
			return value.GetInt();
		}

		inline unsigned int toUint(const Value& value) {
			return value.GetUint();
		}

		inline int64_t toInt64(const Value& value) {
			return value.GetInt64();
		}

		inline uint64_t toUint64(const Value& value) {
			return value.GetUint64();
		}

		inline double toDouble(const Value& value) {
			return value.GetDouble();
		}

		inline float toFloat(const Value& value) {
			return value.GetFloat();
		}

		inline bool toBool(const Value& value) {
			return value.GetBool();
		}

		template<class Type>
		inline ArrayWrapper<Type> toArray(const Value& value) {
			return ArrayWrapper<Type>(value);
		}

		template<class Type>
		inline Type toEnum(const Value& value) {
			return static_cast<Type>(json::toInt64(value));
		}

		template<class Base>
		struct ArrayWrapper<Value, Base> : public Base {
			using Base::Base;
			operator Values() {
				return Base::getArray();
			}
		};

		template <class Type>
		struct EmptyFunction {
			static inline bool empty(const Type& value) {
				return value.empty();
			}
		};

		//ClassTypeHelper needs to be able to call toJSON so it's forward decleared
		template<class Object>
		inline Value toJSON(const Object& object, Value::AllocatorType& allocator);

		template <class Type>
		struct ClassTypeHelper : public EmptyFunction<Type> {
			static inline Type toType(const Value& value) {
				return value;
			}
			static inline Value fromType(const Type& value, Value::AllocatorType& allocator) {
				return toJSON(value, allocator);
			}
		};

		template<>
		struct ClassTypeHelper<std::string> : public EmptyFunction<std::string> {
			static inline std::string toType(const Value& value) {
				return toStdString(value);
			}
			static inline Value fromType(const std::string& value, Value::AllocatorType&) {
				return Value(value.c_str(), value.length());
			}
		};

		template<>
		struct ClassTypeHelper<nonstd::string_view> : public EmptyFunction<nonstd::string_view> {
			static inline nonstd::string_view toType(const Value& value) {
				return toStdStringView(value);
			}
			static inline Value fromType(const nonstd::string_view& value, Value::AllocatorType&) {
				return Value(value.data(), value.length());
			}
		};

		template <class PrimitiveType, int defaultValue = 0>
		struct PrimitiveTypeHelper {
			static inline PrimitiveType toType(const Value& value) {
				return value.Get<PrimitiveType>();
			}
			static inline bool empty(const PrimitiveType& value) {
				return value == static_cast<PrimitiveType>(defaultValue);
			}
			static inline Value fromType(const PrimitiveType& value, Value::AllocatorType& /*allocator*/) {
				return Value(value);
			}
		};

		//for some reason, some compilers need this
		//template <int defaultValue>
		//struct PrimitiveTypeHelper<long int, defaultValue> : public PrimitiveTypeHelper<long long, defaultValue> {};

		template<> struct ClassTypeHelper<int     > : public PrimitiveTypeHelper<int     > {};
		template<> struct ClassTypeHelper<uint32_t> : public PrimitiveTypeHelper<uint32_t> {};
		template<> struct ClassTypeHelper<int64_t > : public PrimitiveTypeHelper<int64_t > {};
		template<> struct ClassTypeHelper<uint64_t> : public PrimitiveTypeHelper<uint64_t> {};
		template<> struct ClassTypeHelper<float   > : public PrimitiveTypeHelper<float   > {};
		template<> struct ClassTypeHelper<double  > : public PrimitiveTypeHelper<double  > {};
		template<> struct ClassTypeHelper<bool    > : public PrimitiveTypeHelper<bool    > {};

		template<class Type>
		struct EnumTypeHelper {
			static inline Type toType(const Value& value) {
				return toEnum<Type>(value);
			}
			static inline Value fromType(const Type& value) {
				return Value(static_cast<int64_t>(value));
			}
			static inline bool empty(const Type& value) {
				return static_cast<int64_t>(value) == 0;
			}
		};

		template<class Container, template<class...> class TypeHelper>
		struct FromContainerFunction {
			static inline Value fromType(const Container& values, Value::AllocatorType& allocator) {
				Value arr(rapidjson::kArrayType);
				arr.Reserve(values.size(), allocator);
				for (const typename Container::value_type& value : values)
					arr.PushBack(TypeHelper<typename Container::value_type>::fromType(value, allocator), allocator);
				return arr;
			} 
		};

		template<class Container, template<class...> class TypeHelper>
		struct ContainerTypeHelper : public EmptyFunction<Container>, public FromContainerFunction<Container, TypeHelper> {
			static inline Container toType(const Value& value) {
				return toArray<typename Container::value_type>(value);
			}
		};

		template<class StdArray, template<class...> class TypeHelper>
		struct StdArrayTypeHelper : public EmptyFunction<StdArray>, public FromContainerFunction<StdArray, TypeHelper> {
			static inline StdArray toType(const Value& value) {
				ArrayWrapper<typename StdArray::value_type> arrayWrapper(value);
				std::array<typename StdArray::value_type, std::tuple_size<StdArray>::value> arr;
				Array jsonArray = arrayWrapper.getDoc().template Get<Array>();
				Value::ConstValueIterator iterator = jsonArray.Begin();
				for (typename StdArray::value_type& v : arr) {
					if (iterator == jsonArray.End())
						break;
					v = TypeHelper<typename StdArray::value_type>::toType(*iterator);
					++iterator;
				}
				return arr;
				//return toArray<typename StdArray::value_type, std::tuple_size<StdArray>::value>(value);
			}
		};

		enum FieldType {
			REQUIRIED_FIELD = 0,
			OPTIONAL_FIELD  = 1 << 0,
			NULLABLE_FIELD  = 1 << 1,
			OPTIONAL_NULLABLE_FIELD = OPTIONAL_FIELD | NULLABLE_FIELD
		};

		template <class Class, class Type, class TypeHelper>
		struct PairImpl {
			using T = Type;
			using Helper = TypeHelper;
			Type Class::*member;
			const char* name;
			const FieldType type;
		};

		template <int defaultValue, template<class, int> class TypeHelper = PrimitiveTypeHelper, class Class, class Type>
		constexpr PairImpl<Class, Type, TypeHelper<Type, defaultValue>> pair(Type Class::*member, const char* name, FieldType type) {
			return PairImpl<Class, Type, TypeHelper<Type, defaultValue>>{member, name, type};
		}

		template <template<class> class TypeHelper = ClassTypeHelper, class Class, class Type>
		constexpr PairImpl<Class, Type, TypeHelper<Type>> pair(Type Class::*member, const char* name, FieldType type) {
			return PairImpl<Class, Type, TypeHelper<Type>>{member, name, type};
		}

		template <template<class, template<class...> class> class TypeHelper, template<class...> class TypeHelper2 = ClassTypeHelper, class Class, class Type>
		constexpr PairImpl<Class, Type, TypeHelper<Type, TypeHelper2>> pair(Type Class::*member, const char* name, FieldType type) {
			return PairImpl<Class, Type, TypeHelper<Type, TypeHelper2>>{member, name, type};
		}

		//There needs to be a workaround for Visual C++ and clang for this to compile. However, this workaround relys on c++14.
#if __cpp_return_type_deduction
#define JSONStruct getJSONStructure()
#define JSONStructStart constexpr static auto JSONStruct { return
#define JSONStructEnd }
#else
#define JSONStruct JSONStructure
#define JSONStructStart constexpr static auto JSONStruct =
#define JSONStructEnd
#endif

		template<class ResultingObject, size_t i = 0>
		inline typename std::enable_if<i == std::tuple_size<decltype(ResultingObject::JSONStruct)>::value, void>::type
			fromJSON(ResultingObject&, const Value&) {
		}

		template<class ResultingObject, size_t i = 0>
		inline typename std::enable_if<i < std::tuple_size<decltype(ResultingObject::JSONStruct)>::value, void>::type
			fromJSON(ResultingObject& object, const Value& value)
		{
			constexpr auto field = std::get<i>(ResultingObject::JSONStruct);
			using Helper = typename decltype(field)::Helper;
			Value::ConstMemberIterator iterator = value.FindMember(field.name);
			if (iterator != value.MemberEnd()) {
				if (!iterator->value.IsNull()) //ignore if null
					object.*(field.member) = Helper::toType(iterator->value);
			} else if (field.type == REQUIRIED_FIELD) {
				//error
				std::cout << 
				"JSON Parse Error: "
				"variable #" << i << ": \"" << field.name << "\" not found. "
				"Please look at call stack from your debugger for more details.";
			}
			fromJSON<ResultingObject, i + 1>(object, value);
		}

		template<class ResultingObject>
		inline ResultingObject fromJSON(const Value& value) {
			ResultingObject object;
			fromJSON(object, value);
			return object;
		}

		template<class ResultingObject>
		inline ResultingObject fromJSON(const nonstd::string_view& json) {
			rapidjson::Document doc;
			doc.Parse(json.data(), json.length());
			//note: some objects have different value consturctors
			//so we need to call the Object's value constructor
			return ResultingObject(doc);
		}

		constexpr std::size_t stringLength(const char*const& string, std::size_t i = 0) {
			return string[i] == '\0' ? i : stringLength(string, i + 1);
		}

		template<class SourceObject, size_t i = 0>
		inline typename std::enable_if<i == std::tuple_size<decltype(SourceObject::JSONStruct)>::value, void>::type
			toJSON(const SourceObject& /*object*/, Value& /*value*/, Value::AllocatorType& /*allocator*/) {
		}

		template<class SourceObject, size_t i = 0>
		inline typename std::enable_if < i < std::tuple_size<decltype(SourceObject::JSONStruct)>::value, void>::type
			toJSON(const SourceObject& object, Value& value, Value::AllocatorType& allocator) {
			constexpr auto field = std::get<i>(SourceObject::JSONStruct);
			using Helper = typename decltype(field)::Helper;
			//value[field.name] = object.*(field.member);
			if (!(field.type & OPTIONAL_NULLABLE_FIELD) || !Helper::empty(object.*(field.member))) {
				constexpr std::size_t nameLength = stringLength(field.name);
				value.AddMember(
					Value::StringRefType(field.name, nameLength),
					Helper::fromType(object.*(field.member), allocator),
					allocator
				);
			}
			toJSON<SourceObject, i + 1>(object, value, allocator);
		}

		template<class Object>
		inline Value toJSON(const Object& object, Value::AllocatorType& allocator) {
			Value obj(rapidjson::kObjectType);
			toJSON(object, obj, allocator);
			return obj;
		}

		template<class Object>
		inline rapidjson::Document toJSON(const Object& object) {
			rapidjson::Document doc;
			doc.SetObject();
			toJSON(object, doc, doc.GetAllocator());
			return doc;
		}

		inline std::string stringify(const Value& value) {
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			value.Accept(writer);
			return std::string(buffer.GetString(), buffer.GetSize());
		}

		template<class Object>
		inline std::string stringifyObj(const Object& object) {
			rapidjson::MemoryPoolAllocator<> allocator;
			return stringify(toJSON(object, allocator));
		}
	}
}
