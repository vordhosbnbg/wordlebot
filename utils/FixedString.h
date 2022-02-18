#pragma once
#include <string>
#include <cstring>
#include <cassert>
#include "MurmurHash3.h"

template<int maxLen>
class FixedString
{
    static constexpr int arrSize = maxLen + 1;
public:
    FixedString() : currentLen(0)
    {
        memset(array, 0, arrSize);
    }

    FixedString(const std::string& original) : currentLen(original.size())
    {
        memset(array, 0, arrSize);
        memcpy(array, original.c_str(), original.size());
    }

    FixedString(const char * cStr)
    {
        memset(array, 0, arrSize);
        if(cStr)
        {
            currentLen = strnlen(cStr, maxLen);
            memcpy(array, cStr, currentLen);
        }
        else
        {
            currentLen = 0;
        }
    }

    FixedString(const FixedString& other) = default;
    FixedString(FixedString&& other) = default;
    ~FixedString(){}

    friend struct std::hash<FixedString<maxLen>>;

    inline const char * c_str() const
    {
        return array;
    }

    inline const char& at(size_t offset) const
    {
        assert(offset >= 0);
        assert(offset < maxLen);
        return array[offset];
    }

    inline char& at(size_t offset)
    {
        return array[offset];
    }

    inline const char& operator[](size_t offset) const
    {
        return array[offset];
    }

    inline char& operator[](size_t offset)
    {
        return array[offset];
    }

    inline size_t size() const
    {
        return currentLen;
    }

    struct hasher
    {
        size_t operator()( const FixedString<maxLen>& objToHash ) const
        {
            size_t retVal;
            MurmurHash3_x86_32(&objToHash.array, sizeof(FixedString<maxLen>::array), 0xDEADBEEF, &retVal);
            return retVal;
        }
    };

    inline bool operator==(const FixedString<maxLen>& other) const
    {
        return (memcmp(array, other.array, currentLen) == 0);
    }

    inline void operator=(const FixedString<maxLen> &other )
    {
        memcpy(array, other.array, arrSize);
        currentLen = other.currentLen;
    }

    inline size_t find_first_of(const char val, size_t offset = 0) const
    {
        size_t retVal = npos;
        if(offset <= currentLen)
        {
            for(size_t index = offset; index < currentLen; ++index)
            {
                if(val == array[index])
                {
                    retVal = index;
                    break;
                }
            }
        }
        return retVal;
    }

    inline size_t count(const char val) const
    {
        size_t retVal = 0;
        for(size_t index = 0; index < currentLen; ++index)
        {
            if(val == array[index])
            {
                ++retVal;
            }
        }
        return retVal;
    }

    inline void push_back(char chr)
    {
        array[currentLen] = chr;
        currentLen++;
    }

    inline FixedString<maxLen> substr(size_t begin, size_t len) const
    {
        FixedString<maxLen> retVal;
        assert(begin + len < arrSize);
        memset(retVal.array, 0, arrSize);
        memcpy(retVal.array, &array[begin], len);
        retVal.currentLen = len;
        return retVal;
    }

    inline size_t count(const char val)
    {
        size_t retVal = 0;
        for (size_t index = 0; index < currentLen; ++index)
        {
            if (array[index] == val)
            {
                ++retVal;
            }
        }
        return retVal;
    }

    static const size_t npos = static_cast<size_t>(-1);

    static constexpr size_t maxSize()
    {
        return maxLen;
    }

private:
    size_t currentLen;
    alignas(16) char array[arrSize];
};

namespace std
{

template <int maxLen>
struct hash<FixedString<maxLen>>
{
    size_t operator()( const FixedString<maxLen>& objToHash ) const
    {
        size_t retVal;
        MurmurHash3_x86_32(&objToHash.array, sizeof(FixedString<maxLen>::array), 0xDEADBEEF, &retVal);
        return retVal;
    }
};

}
