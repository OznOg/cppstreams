//
// Created by FernandoDamian on 7/16/2016.
//

#ifndef CPPSTREAMS_STREAM_H
#define CPPSTREAMS_STREAM_H

#include <vector>
#include <list>
#include <set>
#include <functional>
#include <iostream>
#include <numeric>
#include <memory>

template <class> struct Trait;

template<class T>
struct Trait<std::list<T>> {
    static constexpr void (std::list<T>::*append)(const T&) = &std::list<T>::push_back;
};

template<class T>
struct Trait<std::vector<T>> {
    static constexpr void (std::vector<T>::*append)(const T&) = &std::vector<T>::push_back;
};

template<class T>
struct Trait<std::set<T>> {
    static constexpr std::pair<typename std::set<T>::iterator,bool> (std::set<T>::*append)(const T&) = &std::set<T>::insert;
};

template<typename T, template <class...> typename Container>
class Stream {
    template <typename Y, template <typename...> class Z>
    friend class Stream;

    explicit Stream () : internalContainer(std::make_unique<Container<T>>()), originalContainerReference(*internalContainer) {}
public:
    explicit Stream (const Container<T> & original) : originalContainerReference(original) {}

    template<typename F>
    auto map(F func) -> Stream<decltype(func(T())), Container> {
        using X = decltype(func(T()));
        Stream<X, Container> s;
        for (const auto &e : originalContainerReference) {
            auto &cont = *s.internalContainer;
            (cont.*Trait<Container<X>>::append)(func(e));
        }
        return s;
    }

    Stream<T, Container> filter(std::function<bool(const T &)> func) {
        Stream<T, Container> s;
        for (const auto &e : originalContainerReference) {
            auto &cont = *s.internalContainer;
            if (func(e))
                (cont.*Trait<Container<T>>::append)(e);
        }
        return s;
    }

    Container<T> collect(int limit = -1) {
        Container<T> cont;
        size_t _limit = limit == -1 ? originalContainerReference.size() : limit;
        for (const auto &e : originalContainerReference) {
            (cont.*Trait<Container<T>>::append)(e);
            if (--_limit == 0)
                break;
        }
        return cont;
    }

    T sum(T startValue = 0) {
        Container<T> collected = collect();
        return std::accumulate(collected.begin(), collected.end(), startValue);
    }

    T findFirst(std::function<bool(const T &)> func, T defaultValue) {
        Container<T> lResult = collect();
        for (const auto& value : originalContainerReference ) {
            if (func(value))
                return value;
        }
        return defaultValue;
    }

    static Stream<T, Container> makeStream(const Container<T>& original) {
        Stream<T, Container> oStream(original);
        return oStream;
    }

private:
    std::unique_ptr<Container<T>> internalContainer;
    const Container<T> & originalContainerReference;
};


#endif //CPPSTREAMS_STREAM_H
