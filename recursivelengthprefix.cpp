#include "recursivelengthprefix.h"
#include <byteswap.h>

// 代码从Python翻译成C++ 根据‘视缘公司’的需要进行调整和修改。
// Code translated from python to C++
// adapted and modified according to the needs of shiyuan company.

// https://ethereum.org/en/developers/docs/data-structures-and-encoding/rlp/

size_t RecursiveLengthPrefix::rlpEncode(char* output, const char* input)
{
    unsigned char prefix = 0x80;
    size_t        length = strlen(input);

    if(length > 55) prefix = 0xb7;

    if(length == 1 && static_cast<unsigned char>(input[0]) < 0x80)
    {
        memcpy(output, input, length);
        return length;
    }
    else if((input[0] & 0xf0) == 0xc0 || static_cast<unsigned char>(input[0]) >= 0xf7)
    {
        memcpy(output, input, length);
        return length;
    }
    else
    {
        size_t prefixlen = encode_length(reinterpret_cast<unsigned char*>(output), length, prefix);

        memcpy(output + prefixlen, input, length);
        return length + prefixlen;
    }
}

size_t RecursiveLengthPrefix::rlpEncode(char* output, const std::list<std::string> *inputLst)
{
    size_t lstPayload    = listPayLoad(*inputLst);
    char  *tmp           = (char*) malloc(lstPayload + 16); // lstPayload + prefix + alligment
    size_t l             = 0, total = 0;
    size_t noItems       = inputLst->size();
    unsigned char prefix = 0;

    if(noItems <= 55)
    {
        prefix = 0xc0;
    }
    else if (noItems > 55)
    {
        prefix = 0xf7;
    }

    size_t lstPrefixlen = encode_length(reinterpret_cast<unsigned char*>(output), noItems, prefix);
    total += lstPrefixlen;
    memcpy(tmp, output, lstPrefixlen);

    for(const auto &item : *inputLst)
    {
        l = rlpEncode(output, item.c_str());
        memcpy(tmp + total, output, l);
        total += l;
    }

    memcpy(output, tmp, total);

    free(tmp);
    return total;
}

size_t RecursiveLengthPrefix::rlpDecode(char* output, const char* input)
{
    unsigned char offset;
    size_t        dataLength;
    int           type;
    size_t        length = len(reinterpret_cast<const unsigned char*>(input));
    char         *tmp    = (char*) malloc(length + 4);

    if(length == 0)
    {
        memcpy(output, "\0\0\0\0", 4);
        free(tmp);
        return 0;
    }

    decode_length(offset, dataLength, type, reinterpret_cast<const unsigned char*>(input));
    if(type == 0)
    {
        memcpy(output, input + offset, dataLength);
        memcpy(output + dataLength, "\0\0\0\0", 4);
    }
    else if(type == 1)
    {
        printf("\nRecursiveLengthPrefix 回答: 出口容器是一个列表\n");
        exit(5);
    }

    free(tmp);
    return dataLength + offset;
}
size_t RecursiveLengthPrefix::rlpDecode(std::list<std::string> *outputList, const char* input)
{
    unsigned char offset;
    size_t        dataLength = strlen(input);
    size_t        pos        = 0;
    int           type;
    size_t        length     = len(reinterpret_cast<const unsigned char*>(input));
    char         *tmp        = (char*) malloc(dataLength);

    decode_length(offset, dataLength, type, reinterpret_cast<const unsigned char*>(input + pos));
    pos += offset;

    if(dataLength == 0)
    {
        free(tmp);
        return 0;
    }

    for (size_t i = 0; i < length; i++)
    {
        decode_length(offset, dataLength, type, reinterpret_cast<const unsigned char*>(input + pos));

        if(dataLength == 0) // 应弃之不顾
        {
            pos++;
            i--;
        }
        else if(type == 0)
        {
            memcpy(tmp, input + offset + pos, dataLength);
            memcpy(tmp + dataLength, "\0\0\0\0", 4);
            outputList->push_back(tmp);
            pos += dataLength + offset;
        }
        else if(type == 1)
        {
            pos += rlpDecode(outputList, input + pos);  // recursive call
        }
    }

    free(tmp);
    return pos;
}

size_t RecursiveLengthPrefix::encode_length(unsigned char* ret, size_t L, unsigned char offset)
{
    if(L < 56)          // BL = 1 byte
    {
        unsigned char BL = static_cast<unsigned char>(L);

        BL += offset;
        memcpy(ret, &BL, 1);

        return 1;       // prefix length
    }
    if(L < 256)     // BL = 1 byte
    {
        unsigned char BL = static_cast<unsigned char>(L);

        offset += 1;
        memcpy(ret, &offset, 1);
        memcpy(ret + 1, &BL, 1);

        return 2;   // prefix length
    }
    else if (L < 65536)           // BL = 2 byte
    {
        unsigned short BL   = static_cast<unsigned short>(L);
        unsigned short BLbe = bswap_16(BL);

        offset += 2;
        memcpy(ret, &offset, 1);
        memcpy(ret + 1, &BLbe, 2);

        return 3;   // prefix length
    }
    else if (L < 0x100000000)    // BL = 4 byte
    {
        unsigned int BL   = static_cast<unsigned int>(L);
        unsigned int BLbe = bswap_32(BL);

        offset += 4;
        memcpy(ret, &offset, 1);
        memcpy(ret + 1, &BLbe, 4);

        return 5;   // prefix length
    }
    else                      // BL = 8 byte
    {
        size_t BLbe = bswap_64(L);

        offset += 8;
        memcpy(ret, &offset, 1);
        memcpy(ret + 1, &BLbe, 8);

        return 9; // prefix length
    }
}

void RecursiveLengthPrefix::decode_length(unsigned char& offset, size_t& dataLength, int& type, const unsigned char* input)
{
    size_t         length   = len(input);
    unsigned char  prefix   = input[0];
    unsigned char  noPrefix [8];

    memset(noPrefix, 0, 8);

    if(length == 0 || input[0] == 0x7f)    // 0x7f = ASCII del
    {
        offset = 0;
        dataLength = 0;
        type = 0;
    }
    else if(prefix <= 0x7f)
    {
        offset = 0;
        dataLength = 1;
        type = 0;                          // type 0 = string
    }
    else if(prefix <= 0xb7)
    {
        offset = 1;
        dataLength = static_cast<size_t>(prefix - 0x80);
        type = 0;                          // type 0 = string
    }
    else if(prefix <= 0xbf)
    {
        size_t lenOfStrLen = static_cast<size_t>(prefix - 0xb7);
        char   chrLen[8];
        size_t length64bit = 0;
        memset(chrLen, 0, 8);

        memcpy(noPrefix, input + 1, lenOfStrLen);
        memcpy(chrLen + 8 - lenOfStrLen, noPrefix, lenOfStrLen);
        memcpy(&length64bit, chrLen, 8);

        length64bit = bswap_64(length64bit);   // endianess correction

        offset = 1 + lenOfStrLen;
        dataLength = length64bit;
        type = 0;                              // type 0 = string
    }
    else if(prefix <= 0xf7)
    {
        unsigned char listLen = prefix - 0xc0;

        offset = 1;
        dataLength = static_cast<size_t>(listLen);
        type = 1;                          // type 1 = List
    }
    else
    {
        char   chrListLen[8];
        size_t listLen64bit = 0;
        size_t lenOfListLen = static_cast<size_t>(prefix - 0xf7);

        memset(chrListLen, 0, 8);

        memcpy(noPrefix, input + 1, lenOfListLen);
        memcpy(chrListLen + 8 - lenOfListLen, noPrefix, lenOfListLen);

        memcpy(&listLen64bit, chrListLen, 8);

        listLen64bit = bswap_64(listLen64bit);   // endianess correction

        offset = 1 + lenOfListLen;
        dataLength = listLen64bit;
        type = 1;                          // type 1 = List
    }
}

size_t RecursiveLengthPrefix::len(const unsigned char* s)
{//返回基于标题的长度，而不是实际字符串的长度。

    if(s[0] == 0x80)
    {
        return 0;
    }
    else if (s[0] > 0x80 && s[0] <= 0xb7)
    {
        return s[0] - 0x80;
    }
    else if (s[0] > 0xb7 && s[0] < 0xc0)
    {
        unsigned char l = s[0] - 0xb7;
        if(l == 1) return s[1];

        size_t tmpnum = 0; // little endian
        char   tmp[8];

        memset(tmp, 0, 8);
        memcpy(tmp + 8 - l, s + 1, l);

        memcpy(&tmpnum, tmp, 8);
        tmpnum = bswap_64(tmpnum); // endianess correction

        return tmpnum;
    }

    if(s[0] == 0xc0)
    {
        return 0;
    }
    else if(s[0] > 0xc0 && s[0] <= 0xf7)
    {
        return s[0] - 0xc0;
    }
    else if(s[0] > 0xf7)
    {
        unsigned char l = s[0] - 0xf7;
        if(l == 1) return s[1];

        size_t tmpnum = 0; // little endian
        char  tmp[8];

        memset(tmp, 0, 8);
        memcpy(tmp + 8 - l, s + 1, l);

        memcpy(&tmpnum, tmp, 8);
        tmpnum = bswap_64(tmpnum); // endianess correction

        return tmpnum;
    }

    return 1;  // s[0] < 0x80
}

size_t RecursiveLengthPrefix::listPayLoad(const std::list<std::string>& list)
{
    size_t payload = 0;

    for(const auto &it : list)
    {
        payload += it.length();
    }

    return payload;
}

