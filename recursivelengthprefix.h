#ifndef RECURSIVELENGTHPREFIX_H
#define RECURSIVELENGTHPREFIX_H

#include "RecursiveLengthPrefix_global.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <string>

class RECURSIVELENGTHPREFIX_EXPORT RecursiveLengthPrefix
{
private:
    static size_t encode_length(unsigned char* ret, size_t L, unsigned char offset);
    static void   decode_length(unsigned char& offset, size_t& dataLength, int& type, const unsigned char* input);
    static size_t len          (const unsigned char* s);
    static size_t listPayLoad  (const std::list<std::string>& list);

public:
    static size_t rlpEncode(char* output, const char* input);
    static size_t rlpEncode(char* output, const std::list<std::string> *inputLst);
    static size_t rlpDecode(char* output, const char* input);
    static size_t rlpDecode(std::list<std::string> *outputList, const char* input);
};

#endif // RECURSIVELENGTHPREFIX_H

