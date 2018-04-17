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
    explicit Stream () : internalContainer(std::make_unique<Container<T>>()), originalContainerReference(*internalContainer) {}
public:
    explicit Stream (const Container<T> & original) : originalContainerReference(original) {}

    template<typename F>
    auto map(F func) -> Stream<decltype(func(T())), Container> {
        using X = decltype(func(T()));
        Stream<X, Container> s;
        for (const auto &e : originalContainerReference) {
            auto &cont = *s.internalContainer;
            (cont.*Trait<Container<T>>::append)(func(e));
        }
        return s;
    }

    Stream<T, Container>& filter(std::function<bool(const T &)> func);

    Container<T> collect(int limit = -1);

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
    // Pipeline operation types
    enum class Type {
        Map,
        Filter
    };

    struct Operation {
        int index;
        enum Type type;

        static Operation makeOperation(int index, enum Type type) {
            Operation operation;
            operation.index = index;
            operation.type = type;
            return operation;
        }
    };

    std::vector<std::function<T(const T &)> > _mapOperations;
    std::vector<std::function<bool(const T &)> > _filterOperations;

    std::vector<Operation> _pipeline;
    std::unique_ptr<Container<T>> internalContainer;
    const Container<T> & originalContainerReference;
};

template<typename T, template <class...> typename Container>
Stream<T, Container> & Stream<T, Container>::filter(std::function<bool(const T &)> func) {
    _filterOperations.push_back(func);
    _pipeline.push_back(Operation::makeOperation(_filterOperations.size() - 1, Type::Filter));
    return *this;
}

template<typename T, template <class...> typename Container>
Container<T> Stream<T, Container>::collect(int limit) {

    Container<T> result;
    limit = limit > 0 ? limit : originalContainerReference.size();

    // Loop through each input value (ONLY ONCE!) and operate if needed
    for (const auto & value : originalContainerReference) {

        // if we reach the limit, just break
        if (result.size() == (size_t)limit) break;

        T aux = value; // T object should override operator equals
        auto wasFiltered = true;

        // Go through each operation on the pipeline and execute the lambdas
        for (const auto& operation : _pipeline) {

            switch (operation.type) {
                case Type::Map:
                    aux = _mapOperations[operation.index](aux);
                    break;
                case Type::Filter:
                    wasFiltered = _filterOperations[operation.index](aux);
                    break;
            }

        }
        // So if the filter filters there is no reason to keep this value on the output
        if (!wasFiltered)
            continue;

        result.insert(result.end(), aux);
    }
    return result;
}


#endif //CPPSTREAMS_STREAM_H
