#pragma once

#pragma once

#include <algorithm>
#include <type_traits>

// Me think, why waste time write lot code, when few code do trick.

template <class V>
concept compressed = std::is_empty_v<V> && !std::is_final_v<V>;  // NOLINT

template <class V>
concept ne_compressed = !std::is_empty_v<V> || std::is_final_v<V>;  // NOLINT

template <typename V, typename U>
concept not_based_compress =
    compressed<V> && compressed<U> && !(std::is_base_of_v<U, V> || std::is_base_of_v<V, U>);

template <class F, class S>
class CompressedPair {
public:
    CompressedPair() : first_(), second_() {
    }

    template <class F1, class S1>
    CompressedPair(F1&& first, S1&& second)
        : first_(std::forward<F1>(first)), second_(std::forward<S1>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    S& GetSecond() {
        return second_;
    };

    const F& GetFirst() const {
        return first_;
    }

    const S& GetSecond() const {
        return second_;
    };

private:
    F first_;
    S second_;
};

template <compressed F, class S>
class CompressedPair<F, S> : F {
public:
    CompressedPair() : F(), second_() {
    }

    template <class F1, class S1>
    CompressedPair(F1&& first, S1&& second)
        : F(std::forward<F1>(first)), second_(std::forward<S1>(second)) {
    }

    F& GetFirst() {
        return *this;
    }

    S& GetSecond() {
        return second_;
    };

    const F& GetFirst() const {
        return *this;
    }

    const S& GetSecond() const {
        return second_;
    };

private:
    S second_;
};

template <ne_compressed F, compressed S>
class CompressedPair<F, S> : S {
public:
    CompressedPair() : S(), first_() {
    }

    template <class F1, class S1>
    CompressedPair(F1&& first, S1&& second)
        : S(std::forward<S1>(second)), first_(std::forward<F1>(first)) {
    }

    F& GetFirst() {
        return first_;
    }

    S& GetSecond() {
        return *this;
    };

    const F& GetFirst() const {
        return first_;
    }

    const S& GetSecond() const {
        return *this;
    };

private:
    F first_;
};

template <compressed F, not_based_compress<F> S>
class CompressedPair<F, S> : F, S {
public:
    CompressedPair() : F(), S() {
    }

    template <class F1, class S1>
    CompressedPair(F1&& first, S1&& second)
        : F(std::forward<F1>(first)), S(std::forward<S1>(second)) {
    }

    F& GetFirst() {
        return *this;
    }

    S& GetSecond() {
        return *this;
    };

    const F& GetFirst() const {
        return *this;
    }

    const S& GetSecond() const {
        return *this;
    };
};
