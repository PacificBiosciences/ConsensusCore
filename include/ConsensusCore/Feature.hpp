// Author: David Alexander

#pragma once

#include <algorithm>
#include <boost/range.hpp>
#include <boost/shared_array.hpp>
#include <boost/utility.hpp>
#include <cassert>
#include <string>
#include <vector>

#include <ConsensusCore/Types.hpp>

namespace ConsensusCore {
// Feature/Features object usage caveats:
//  - Feature and Features objects _must_ be stored by value, not reference
//  - The underlying array must be allocated using new[]
template <typename T>
class Feature : private boost::shared_array<T>
{
public:
    // \brief Allocate a new feature object, copying content from ptr.
    Feature(const T* inPtr, int length) : boost::shared_array<T>(new T[length]), length_(length)
    {
        assert(length >= 0);
        std::copy(inPtr, inPtr + length, get());
    }

    // Our features are typically stored in unsigned char[] or short[].
    // Here are constructors to make it easier to stuff those guys into
    // a FloatFeature.
    Feature(const unsigned char* inPtr, int length)
        : boost::shared_array<T>(new T[length]), length_(length)
    {
        assert(length >= 0);
        std::copy(inPtr, inPtr + length, get());
    }

    // \brief Allocate and zero-fill a new feature object of given length.
    explicit Feature(int length) : boost::shared_array<T>(new T[length]()), length_(length)
    {
        assert(length >= 0);
    }

    int Length() const { return length_; }

    const T& operator[](int i) const { return this->boost::shared_array<T>::operator[](i); }

    T& operator[](int i) { return this->boost::shared_array<T>::operator[](i); }

    T ElementAt(int i) const { return (*this)[i]; }

    std::string ToString() const { return std::string{*this}; }

private:
    int length_;

#ifndef SWIG
public:
    T* get() { return this->boost::shared_array<T>::get(); }

    const T* get() const { return this->boost::shared_array<T>::get(); }

    operator std::string() const;
#endif  // !SWIG
};

#ifndef SWIG
//
// Support for boost::foreach
//
template <typename T>
inline const T* range_begin(const Feature<T>& f)
{
    return f.get();
}

template <typename T>
inline const T* range_end(const Feature<T>& f)
{
    return f.get() + f.Length();
}

template <typename T>
inline T* range_begin(Feature<T>& f)  // NOLINT
{
    return f.get();
}

template <typename T>
inline T* range_end(Feature<T>& f)  // NOLINT
{
    return f.get() + f.Length();
}
#endif  // !SWIG

typedef Feature<float> FloatFeature;
typedef Feature<char> CharFeature;
typedef Feature<int> IntFeature;
}

#ifndef SWIG
namespace boost {
template <typename T>
struct range_const_iterator<ConsensusCore::Feature<T>>
{
    typedef const T* type;
};

template <typename T>
struct range_mutable_iterator<ConsensusCore::Feature<T>>
{
    typedef T* type;
};
}
#endif  // !SWIG
