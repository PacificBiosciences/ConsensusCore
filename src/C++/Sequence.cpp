// Author: David Alexander

#include <string>

//
// For testing purposes, N and M are defined as two phony DNA bases
// that are complementary
//

static char ComplementArray[] = {
    3,   2,   1,   0,   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, '-', 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 'T', 127, 'G', 127, 127, 127, 'C', 127, 127, 127, 127,
    127, 'N', 'M', 127, 127, 127, 127, 127, 'A', 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 't', 127, 'g', 127, 127, 127, 'c', 127, 127, 127, 127, 127, 'n', 'm', 127, 127, 127,
    127, 127, 'a', 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127};

namespace ConsensusCore {

char ComplementaryBase(char base) { return ComplementArray[static_cast<int>(base)]; }

std::string Complement(const std::string& input)
{
    std::string output(input.length(), 127);
    for (unsigned int i = 0; i < input.length(); i++) {
        output[i] = ComplementArray[static_cast<int>(input[i])];
    }
    return output;
}

std::string Reverse(const std::string& input) { return std::string(input.rbegin(), input.rend()); }

std::string ReverseComplement(const std::string& input) { return Reverse(Complement(input)); }
}
