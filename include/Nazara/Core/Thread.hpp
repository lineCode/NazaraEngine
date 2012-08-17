// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Inspiré du code de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_THREAD_HPP
#define NAZARA_THREAD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Functor.hpp>
#include <Nazara/Core/NonCopyable.hpp>

class NzThreadImpl;

class NAZARA_API NzThread : NzNonCopyable
{
	friend NzThreadImpl;

	public:
		class NAZARA_API Id
		{
			friend NzThread;
			friend NzThreadImpl;

			public:
				Id() = default;
				Id(Id&& rhs) = default;
				~Id();

				Id& operator=(Id&& rhs) = default;
				bool operator==(const Id& rhs) const;
				bool operator!=(const Id& rhs) const;

			private:
				Id(void* handle) : m_handle(handle) {}
				Id(const NzThreadImpl* thread);

				void* m_handle = nullptr;
		};

		template<typename F> NzThread(F function);
		template<typename F, typename... Args> NzThread(F function, Args... args);
		template<typename C> NzThread(void (C::*function)(), C* object);
		~NzThread();

		Id GetId() const;
		bool IsCurrent() const;
		void Launch(bool independent = false);
		void Join();
		void Terminate();

		static Id GetCurrentId();
		static void Sleep(nzUInt32 time);

	private:
		NzFunctor* m_func;
		NzThreadImpl* m_impl = nullptr;
		bool m_independent;
};

#include <Nazara/Core/Thread.inl>

#endif // NAZARA_THREAD_HPP
