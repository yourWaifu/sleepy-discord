#pragma once
#include <list>
#include <utility>
#include <vector>
#include <array>
#include <tuple>
#include <memory>
#include <type_traits>
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
	//this is outside of the json namespace
	//to stop us from hiting some compiler bugs
	template<class Type>
	struct GetDefault {
		static inline const Type get() {
			return static_cast<Type>(0);
		}
	};

	template<class Type>
	struct GetEnumBaseType {
		using Value = int64_t;
	};

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
			if (source.empty())
				return "";
			std::string target;
			target += '[';
			for (std::string value : source) {
				if (!value.empty()) {
					target += value;
					target += ',';
				}
			}
			char& back = target.back();
			if (back == ',') {
				back = ']';
			} else {
				target += ']';
			}
			return target;
		}

		struct ArrayStringWrapper {
			const Value& json;
			ArrayStringWrapper(const Value& json) : json(json) {}
			inline const Value& getDoc() const { return json; }
			operator const Value&() const { return getDoc(); }
			template<class Callback>
			const bool getDoc(const Callback& callback) const {
				callback(json);
				return true;
			}
		};

		template<class TypeToConvertTo, class Base = ArrayStringWrapper>
		struct ArrayWrapper : public Base {
			using Base::Base;
			using DocType = decltype(std::declval<Base>().getDoc());
			template<class Container>
			Container get(DocType doc) {
				Array jsonArray = doc.template Get<Array>();
				return Container(jsonArray.begin(), jsonArray.end());
			}

			template<class Container>
			Container get() {
				return get<Container>(Base::getDoc());
			}

			template<template<class...> class Container, typename Type = TypeToConvertTo>
			Container<Type> get() {
				return get<Container<Type>>();
			}

			template<class Container>
			bool get(Container& value) {
				static const auto getter = [&](DocType& doc) {
					value = get<Container>(doc);
				};
				return Base::getDoc(getter);
			}

			template<template<class...> class Container, typename Type = TypeToConvertTo>
			bool get(Container<Type>& value) {
				return get<Container<Type>>(value);
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
			return static_cast<Type>(value.Get<typename GetEnumBaseType<Type>::Value>());
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

		//ClassTypeHelper needs to know if object has a serialize member
		template<class Object>
		struct hasSerialize {
		private:
			template<typename T>
			static constexpr auto check(T*)
			-> typename
				std::is_same<
					decltype( std::declval<T>().serialize(
						std::declval<Value::AllocatorType&>()
					) ),
					Value
				>::type;
			
			template<typename>
    		static constexpr std::false_type check(...);
		
		public:
			using type = decltype(check<Object>(0));
			static constexpr bool value = type::value;
		};

		template <class Type>
		struct ClassTypeHelper : public EmptyFunction<Type> {
			static inline Type toType(const Value& value) {
				return value;
			}

			template<class T>
			static inline typename std::enable_if<hasSerialize<T>::value, Value>::type
			fromType(const T& value, Value::AllocatorType& allocator) {
				return value.serialize(allocator);
			}

			template<class T>
			static inline typename std::enable_if<hasSerialize<T>::value == false, Value>::type
			fromType(const T& value, Value::AllocatorType& allocator) {
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

		template<class Type, class GetDefault, class BaseType>
		struct BaseEnumTypeHelper {
			static inline Type toType(const Value& value) {
				return toEnum<Type>(value);
			}
			static inline Value fromType(const Type& value) {
				return Value(static_cast<BaseType>(value));
			}
			static inline bool empty(const Type& value) {;
				return value == GetDefault::get();
			}
		};

		template<class Type>
		struct EnumTypeHelper : public
			BaseEnumTypeHelper<Type, GetDefault<Type>, typename GetEnumBaseType<Type>::Value> {};

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
		inline rapidjson::ParseResult fromJSON(ResultingObject& obj, const nonstd::string_view& json) {
			rapidjson::Document doc;
			rapidjson::ParseResult isOK = doc.Parse(json.data(), json.length());
			if (isOK)
				obj = ResultingObject{doc};
			return isOK;
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

		template<class Object, size_t i = 0>
		inline typename std::enable_if<i == std::tuple_size<decltype(Object::JSONStruct)>::value, void>::type
			mergeObj(Object& object, const Object& objectChanges) {
		}

		template<class Object, size_t i = 0>
		inline typename std::enable_if < i < std::tuple_size<decltype(Object::JSONStruct)>::value, void>::type
			mergeObj(Object& object, const Object& objectChanges) {
			constexpr auto field = std::get<i>(Object::JSONStruct);
			using Helper = typename decltype(field)::Helper;
			if (!Helper::empty(objectChanges.*(field.member))) {
				object.*(field.member) = objectChanges.*(field.member);
			}
			mergeObj<Object, i + 1>(object, objectChanges);
		}

		//json optional and null emulation
		struct UndefinedType {};

		template<class Type>
		struct Maybe {
			using FlagType = bool;
			static constexpr FlagType undefined = 0;
			static constexpr FlagType defined = 1;

			using ValueType = Type;
			using element_type = Type;
			using Container = std::shared_ptr<Type>;
			using weak_ptr = typename Container::weak_ptr;

			const bool isUndefined() const {
				return ( flags & defined ) == undefined;
			}
			const bool isDefined() const {
				return !isUndefined();
			}
			const bool isAvaiable() const {
				return isDefined() && static_cast<bool>(value);
			}
			const bool isNull() const {
				return isDefined() && !static_cast<bool>(value);
			}
			 
			Type copy(Type& defaultValue) const {
				return isDefined() ? static_cast<bool>(value) ? *get() : defaultValue : defaultValue;
			}
			
			void copyTo(Type& dest) const {
				if (isAvaiable())
					dest = *get();
			}

			constexpr Maybe() = default;
			constexpr Maybe(std::nullptr_t) noexcept : value(std::nullptr_t{}), flags(defined) {};
			Maybe& operator=(std::nullptr_t) noexcept {
				value = std::nullptr_t{};
				flags |= defined;
			}
			constexpr Maybe(UndefinedType) {};
			template<class TypeY>
			explicit Maybe(TypeY* ptr) noexcept : value(ptr), flags(defined) {}
			template<class TypeY, class Deleter>
			Maybe(Type* ptr, Deleter deleter) noexcept :
				value(ptr, deleter), flags(defined) {}
			template<class TypeY, class Deleter, class Alloc>
			Maybe(Type* ptr, Deleter deleter, Alloc allocator) noexcept :
				value(ptr, deleter, allocator), flags(defined) {}
			template<class Deleter>
			Maybe(std::nullptr_t, Deleter deleter) : value(nullptr, deleter), flags(defined) {}
			template<class Deleter, class Alloc>
			Maybe(std::nullptr_t, Deleter deleter, Alloc allocator) noexcept :
				value(nullptr, deleter, allocator), flags(defined) {}
			template<class Deleter>
			Maybe(UndefinedType, Deleter deleter) : value(nullptr, deleter) {}
			template<class Deleter, class Alloc>
			Maybe(UndefinedType, Deleter deleter, Alloc allocator) noexcept :
				value(nullptr, deleter, allocator) {}
			template<class Type2>
			Maybe(const Maybe<Type2>& right, ValueType* pointer) noexcept :
				value(right.value, pointer), flags(defined) {}
			template<class Type2>
			Maybe(const Maybe<Type2>&& right, ValueType* pointer) noexcept :
				value(std::move(right.value), pointer), flags(defined) {}
			Maybe(const Maybe& other) noexcept : value(other), flags(other.flags) {}
			explicit Maybe(Container&& right) noexcept :
				value(std::move(right)), flags(defined) {}
			Maybe(Maybe&& right) noexcept :
				value(std::move(right.value)), flags(right.flags) {}
			template<class Type2>
			Maybe(Maybe<Type2>&& right) noexcept :
				value(std::move(right.value)), flags(right.value) {}
			template<class Type2>
			explicit Maybe(std::shared_ptr<Type2>&& right) noexcept :
				value(std::move(right)), flags(defined) {}
			template<class Type2>
			explicit Maybe(const weak_ptr& other) :
				value(other), flags(defined) {}
			template<class Type2, class Deleter>
			Maybe(std::unique_ptr<Type2, Deleter>&& other) :
				value(std::move(other)), flags(defined) {}
			

			Maybe& operator=(const Maybe& right) noexcept {
				Maybe(right).swap(*this);
				return *this;
			}

			template<class Type2>
			Maybe& operator=(const Maybe<Type2>& right) noexcept {
				Maybe(right).swap(*this);
				return *this;
			}

			Maybe& operator=(Maybe&& right) noexcept {
				Maybe(std::move(right)).swap(*this);
				return *this;
			}

			template<class Type2>
			Maybe& operator=(Maybe<Type2>&& right) noexcept {
				Maybe(std::move(right)).swap(*this);
				return *this;
			}

			template<class TypeX, class Deleter>
			Maybe& operator=(std::unique_ptr<TypeX, Deleter>&& right) {
				Maybe(std::move(right)).swap(*this);
				return *this;
			}

			Maybe& operator=(const Container& right) noexcept {
				Maybe(right).swap(*this);
				return *this;
			}

			Maybe& operator=(Container&& right) noexcept {
				Maybe(std::move(right)).swap(*this);
				return *this;
			}

			void swap(Maybe& right) noexcept {
				value.swap(right.value);
				std::swap(flags, right.flags);
			}

			ValueType& operator*() const noexcept {
				return *value;
			}

			ValueType* operator->() const noexcept {
				return value.operator->();
			}

			ValueType* get() const noexcept {
				return value.get();
			}

			explicit operator bool() const noexcept {
				return isAvaiable();
			}

			bool empty() const noexcept {
				return isAvaiable();
			}

			ValueType* release() noexcept {
				flags = undefined;
				return value.release();
			}

			void reset() noexcept {
				value.reset();
			}

			template<class TypeX>
			void reset(ValueType* ptr) noexcept {
				value.reset(ptr);
			}

			template<class TypeX, class Deleter>
			void reset(ValueType* ptr, Deleter deleter) noexcept {
				value.reset(ptr, deleter);
			}

			template<class TypeX, class Deleter, class Alloc>
			void reset(ValueType* ptr, Deleter deleter, Alloc allocator) noexcept {
				value.reset(ptr, deleter, allocator);
			}

			long use_count() const noexcept {
				return value.use_count();
			}

			void setToUndefined() noexcept {
				reset();
				flags = undefined;
			}

			Container& getSmartPtr() const noexcept {
				return value;
			}

		private:
			Container value;
			FlagType flags = undefined;
		};

		template<class MaybeType, template<class...> class TypeHelper>
		struct MaybeTypeHelper : public EmptyFunction<MaybeType> {
			static inline MaybeType toType(const Value& value) {
				return TypeHelper<typename MaybeType::element_type>::toType(value);
			}
			static inline Value fromType(const MaybeType& value, Value::AllocatorType& allocator) {
				return TypeHelper<typename MaybeType::element_type>::fromType(*value, allocator);
			}
		};

		template <template<class...> class TypeHelper2 = ClassTypeHelper, class Class, class Type,
			typename = std::enable_if<
				std::is_same<
					Type, Maybe<typename Type::ValueType>
				>::value
			>
		>
		constexpr PairImpl<Class, Type, MaybeTypeHelper<Type, TypeHelper2>> pair(Type Class::*member, const char* name, FieldType type) {
			return PairImpl<Class, Type, MaybeTypeHelper<Type, TypeHelper2>>{member, name, type};
		}		
	}
}
