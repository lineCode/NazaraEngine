// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_CORE_HPP
#define NAZARA_ALGORITHM_CORE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/SerializationContext.hpp>
#include <Nazara/Core/TypeTag.hpp>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>

namespace Nz
{
	class AbstractHash;
	class ByteArray;

	template<typename F, typename Tuple> decltype(auto) Apply(F&& fn, Tuple&& t);
	template<typename O, typename F, typename Tuple> decltype(auto) Apply(O& object, F&& fn, Tuple&& t);
	template<typename T> constexpr std::size_t BitCount();
	template<typename T> ByteArray ComputeHash(HashType hash, const T& v);
	template<typename T> ByteArray ComputeHash(AbstractHash* hash, const T& v);
	template<typename T, std::size_t N> constexpr std::size_t CountOf(T(&name)[N]) noexcept;
	template<typename T> std::size_t CountOf(const T& c);
	template<typename T> void HashCombine(std::size_t& seed, const T& v);
	template<typename T> T ReverseBits(T integer);

	template<typename T>
	struct AlwaysFalse : std::false_type {};

	template<typename... Args>
	struct OverloadResolver
	{
		template<typename R, typename T>
		constexpr auto operator()(R(T::* ptr)(Args...)) const noexcept
		{
			return ptr;
		}

		template<typename R, typename T>
		constexpr auto operator()(R(T::* ptr)(Args...) const) const noexcept
		{
			return ptr;
		}

		template<typename R>
		constexpr auto operator()(R(*ptr)(Args...)) const noexcept
		{
			return ptr;
		}
	};

	template<typename... Args> constexpr OverloadResolver<Args...> Overload = {};

	template<typename T>
	struct PointedType
	{
		using type = void; //< FIXME: I can't make SFINAE work
	};

	template<typename T>
	bool Serialize(SerializationContext& context, T&& value);

	inline bool Serialize(SerializationContext& context, bool value, TypeTag<bool>);
	inline bool Serialize(SerializationContext& context, const std::string& value, TypeTag<std::string>);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Serialize(SerializationContext& context, T value, TypeTag<T>);

	template<typename T>
	bool Unserialize(SerializationContext& context, T* value);

	inline bool Unserialize(SerializationContext& context, bool* value, TypeTag<bool>);
	inline bool Unserialize(SerializationContext& context, std::string* value, TypeTag<std::string>);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Unserialize(SerializationContext& context, T* value, TypeTag<T>);
}

#include <Nazara/Core/Algorithm.inl>

#endif // NAZARA_ALGORITHM_CORE_HPP
