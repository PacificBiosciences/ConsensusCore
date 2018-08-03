// Author: David Alexander

#include <ConsensusCore/Feature.hpp>

#include <string>

namespace ConsensusCore {

#ifndef SWIG
template <>
Feature<char>::operator std::string() const
{
    return std::string(get(), Length());
}

template <>
Feature<float>::operator std::string() const
{
    return "<Float feature>";
}

template <>
Feature<int>::operator std::string() const
{
    return "<Int feature>";
}
#endif  // !SWIG

template class ConsensusCore::Feature<char>;
template class ConsensusCore::Feature<float>;
template class ConsensusCore::Feature<int>;
}
