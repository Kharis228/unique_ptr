#pragma once

#include "compressed_pair.h"
#include <iostream>
#include <cstddef>  // std::nullptr_t
#include <memory>

template <class T>
struct Slug {
    Slug() {
    }
    Slug(const Slug&) = delete;
    template <class S = T>
    Slug(Slug<S>&&) {
    }
    Slug& operator=(const Slug&) = delete;
    template <class S>
    Slug& operator=(Slug<S>&&) {
        return *this;
    }

    void operator()(T* p) const {
        delete p;
    }
};

template <class T>
struct Slug<T[]> {
    Slug() {
    }
    Slug(const Slug&) = delete;
    template <class S = T>
    Slug(Slug<S>&&) {
    }
    Slug& operator=(const Slug&) = delete;
    template <class S>
    Slug& operator=(Slug<S>&&) {
        return *this;
    }

    void operator()(T* p) const {
        delete[] p;
    }
};

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : pair_(ptr, Deleter()){};
    UniquePtr(T* ptr, const Deleter& deleter) : pair_(ptr, deleter) {
    }
    UniquePtr(T* ptr, Deleter&& deleter) : pair_(ptr, std::move(deleter)) {
    }

    template <typename S = T, typename Del = Slug<T>>
    UniquePtr(UniquePtr<S, Del>&& other) noexcept {
        pair_.GetFirst() = other.Release();
        pair_.GetSecond() = std::move(other.GetDeleter());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        pair_.GetSecond() = std::move(other.GetDeleter());
        Reset(other.Release());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (pair_.GetFirst() != nullptr) {
            pair_.GetSecond()(pair_.GetFirst());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        auto res = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return res;
    }
    void Reset(T* ptr = nullptr) {
        auto old_ptr = pair_.GetFirst();
        pair_.GetFirst() = ptr;
        if (old_ptr != nullptr) {
            pair_.GetSecond()(old_ptr);
        }
    }
    void Swap(UniquePtr& other) {
        std::swap(pair_, other.pair_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }

    Deleter& GetDeleter() {
        return pair_.GetSecond();
    };
    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    };
    explicit operator bool() const {
        return pair_.GetFirst() != nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *pair_.GetFirst();
    };
    T* operator->() const {
        return pair_.GetFirst();
    };

private:
    CompressedPair<T*, Deleter> pair_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : pair_(ptr, Deleter()){};
    UniquePtr(T* ptr, const Deleter& deleter) : pair_(ptr, deleter) {
    }
    UniquePtr(T* ptr, Deleter&& deleter) : pair_(ptr, std::move(deleter)) {
    }

    template <typename S = T, typename Del = Slug<T[]>>
    UniquePtr(UniquePtr<S, Del>&& other) noexcept {
        pair_.GetFirst() = other.Release();
        pair_.GetSecond() = std::move(other.GetDeleter());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        pair_.GetSecond() = std::move(other.GetDeleter());
        Reset(other.Release());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    T& operator[](size_t index) {
        return Get()[index];
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (pair_.GetFirst() != nullptr) {
            pair_.GetSecond()(pair_.GetFirst());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        auto res = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return res;
    }

    void Reset(T* ptr = nullptr) {
        auto old_ptr = pair_.GetFirst();
        pair_.GetFirst() = ptr;
        if (old_ptr != nullptr) {
            pair_.GetSecond()(old_ptr);
        }
    }

    void Swap(UniquePtr& other) {
        std::swap(pair_, other.pair_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }

    Deleter& GetDeleter() {
        return pair_.GetSecond();
    };
    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    };
    explicit operator bool() const {
        return pair_.GetFirst() != nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *pair_.GetFirst();
    };
    T* operator->() const {
        return pair_.GetFirst();
    };

private:
    CompressedPair<T*, Deleter> pair_;
};
