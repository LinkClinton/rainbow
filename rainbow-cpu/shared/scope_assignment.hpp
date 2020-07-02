#pragma once

#include "../interfaces/noncopyable.hpp"

namespace rainbow::cpus::shared {

	template <typename T>
	class scope_assignment_t : public interfaces::noncopyable {
	public:
		scope_assignment_t() = default;

		scope_assignment_t(scope_assignment_t&& other) noexcept;
		
		explicit scope_assignment_t(T* origin, const T& value);
		
		~scope_assignment_t();

		scope_assignment_t& operator=(scope_assignment_t&& other) noexcept;
	private:
		T* mOrigin = nullptr, mBackup = T();
	};

	template <typename T>
	scope_assignment_t<T>::scope_assignment_t(scope_assignment_t&& other) noexcept
	{
		mOrigin = other.mOrigin;
		mBackup = other.mBackup;

		other.mOrigin = nullptr;
	}

	template <typename T>
	scope_assignment_t<T>::scope_assignment_t(T* origin, const T& value) :
		mOrigin(origin)
	{
		if (mOrigin == nullptr) return;

		mBackup = *mOrigin;
		*mOrigin = value;
	}

	template <typename T>
	scope_assignment_t<T>::~scope_assignment_t()
	{
		if (mOrigin != nullptr) *mOrigin = mBackup;
	}

	template <typename T>
	scope_assignment_t<T>& scope_assignment_t<T>::operator=(scope_assignment_t&& other) noexcept
	{
		if (mOrigin != nullptr) *mOrigin = mBackup;

		mOrigin = other.mOrigin;
		mBackup = other.mBackup;

		other.mOrigin = nullptr;

		return *this;
	}

}
