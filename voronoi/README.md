#Voronoi Computation (C++ version)

A C++11 version of a Javascript class for Voronoi computation written by Raymond Hill.

https://github.com/gorhill/Javascript-Voronoi

This project is practically a C++ version of the original Javascript (including source comments.)
The code uses C++11 conventions.  Where convenient, reference counting of objects (like BeachArc)
is used.  In most cases I've made an effort to minimize memory allocations, and to avoid using
smart pointers.

## Todos

I've run a few tests locally and from an algorithmn standpoint, everything seems good.  But...

This code is not production ready.  Below is a list of tasks to get this code 'production-ready' 

- Abstract memory allocations/deallocations - (i.e. no new/deleting from the heap)
- Orphaned BeachArc objects are not released upon completion (yes, leaks!)  See the above point on memory management
- Reorganize code into hpp/cpp files
- Tests!

## License

This is licensed under the MIT license.  See LICENSE.md and CREDITS.md for details - everything is MIT.
