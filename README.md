# Java 8 Streams C++ port

This is an e implementation of the behaviour of Java 8 Streams.

The idea is to have a stream class that can handle **filter**, **map** and **collect** methods. It should operate over STL containers. And it would let you chain operations in a efficient way by pipelining them.

This STL containers have been tested:
* list
* vector
* set

## Usage

To use it just create a stream object, it needs two template arguments. First the stream contained object type, then the original container type. here is an example using a list of ints:

```c++ 
Stream<int, std::list<int> > oStream(testList); 
```

or you can just use makeStream static method:

```c++ 
Stream<int, std::list<int> >::makeStream(testList) 
```

Then chain as many *map* and/or *filter* as needed:

```c++ 
oStream.filter([](const int &iValue) { return iValue % 2 == 0; })
       .map([](const int &iValue) { return iValue * 2; })
       .map([](const int &iValue) { return iValue + 3; })
```

And finally use the method *collect*. It receives an optional limit parameter to get a restricted set of the original list

```c++ 
.collect() 
```

*collect* will return a new list with the applied functions.

There are all the methods available

| Method        | Desc        |
| ------------- |-------------|
| filter(*&lt;lambda_expression&gt;*) | Filter stream elements |
| map(*&lt;lambda_expression&gt;*) | Transforms stream elements |
| collect(limit = 0) | Process pipelined stream operations and return first *limit* elements |
| sum(startValue = 0) | Accumulate the objects of the stream |
| findFirst(*&lt;lambda_expression&gt;*) | Returns the first element |

There are several other methods like *sum* to accumulate the objects of the stream, *findFirst* to find first occurrence given a predicate. And more are coming.

## Motivation

For the full story check this [Medium post](https://medium.com/@lopez.fernando.damian/java-8-streams-c-port-9aaaed28b81a#.qml1he9ez).

## TODO

There are a lot of things still to do:

* Test it with containers of complex objects and structs
* Add *reduce* method
* Add *forEach*
* Add *findAny*
* Infinite streams
* Performance analysis


