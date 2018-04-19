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
#include <optional>
#include <future>

template <class> struct Trait;

template<class T>
struct Trait<std::list<T>> {
    static constexpr void (std::list<T>::*append)(T&&) = &std::list<T>::push_back;
};

template<class T>
struct Trait<std::vector<T>> {
    static constexpr void (std::vector<T>::*append)(T&&) = &std::vector<T>::push_back;
};

template<class T>
struct Trait<std::set<T>> {
    static constexpr std::pair<typename std::set<T>::iterator,bool> (std::set<T>::*append)(T&&) = &std::set<T>::insert;
};

template<typename Container>
class Stream;

template<typename T, template <class...> typename Container>
class Stream<Container<T>> {
    template <typename Y>
    friend class Stream;

    explicit Stream () : internalContainer(std::make_unique<std::list<std::future<T>>>()) {
    }
public:
    explicit Stream (const Container<T> & original) : internalContainer(std::make_unique<std::list<std::future<T>>>()) {
        for (const auto &e : original) {
            internalContainer->push_back(std::move(std::async(std::launch::deferred, [&] () { return e; })));
        }
    }

    template<typename F>
    auto map(F func) -> Stream<Container<decltype(func(T()))>> {
        using X = decltype(func(T()));
        Stream<Container<X>> s;
        for (auto &e : *internalContainer) {
            auto &cont = *s.internalContainer;
            cont.push_back(std::move(std::async(std::launch::deferred, [&] () { return func(e.get()); })));
        }
        return s;
    }

    Stream<Container<T>> filter(std::function<bool(const T &)> func) {
        Stream<Container<T>> s;
        for (auto &e : *internalContainer) {
            auto &cont = *s.internalContainer;
            auto val = e.get();
            if (func(val))
                cont.push_back(std::async(std::launch::deferred, [=] () { return val; }));
        }
        return s;
    }

    Container<T> collect(int limit = -1) {
        Container<T> cont;
        size_t _limit = limit == -1 ? internalContainer->size() : limit;
        for (auto &e : *internalContainer) {
            (cont.*Trait<Container<T>>::append)(e.get());
            if (--_limit == 0)
                break;
        }
        return cont;
    }

    T sum(T startValue = 0) {
        Container<T> collected = collect();
        return std::accumulate(collected.begin(), collected.end(), startValue);
    }

    std::optional<T> findFirst(std::function<bool(const T &)> func) {
        for (auto& value : *internalContainer ) {
            auto v = value.get();
            if (func(v))
                return v;
        }
        return std::nullopt;
    }

    std::optional<T> findAny() {
        if (internalContainer->empty())
            return std::nullopt;
        return internalContainer->front().get();
    }

    static Stream<Container<T>> makeStream(const Container<T>& original) {
        Stream<Container<T>> oStream(original);
        return std::move(oStream);
    }

    size_t count() {
        return internalContainer->size();
    }

    template <class Res, class BinaryOperation>
    Res reduce(Res init, BinaryOperation op) {
        return accumulate(internalContainer->begin(), internalContainer->end(), init, [&] (Res r, typename Container<std::future<T>>::value_type &v) { return op(r, v.get()); });
    }
private:
    std::unique_ptr<std::list<std::future<T>>> internalContainer;
};


#endif //CPPSTREAMS_STREAM_H
