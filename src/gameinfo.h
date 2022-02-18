#pragma once
#include <vector>
#include <array>
#include <set>
#include <unordered_map>
#include <map>
#include <bitset>
#include <cmath>
#include <execution>
#include "FixedString.h"
#include "boost/container/flat_set.hpp"
#include "boost/container/flat_map.hpp"


using AlphabetSet = boost::container::flat_set<char>;
using LetterMap = std::array<AlphabetSet, 5>;
using Word = FixedString<5>;
using PatternInfoBitMap = std::unordered_map<Word, std::atomic<float>>;


class GameInfo
{
public:
    GameInfo(const std::vector<Word>& allowedList, const std::vector<Word>& targetList) :
        allowedWords(allowedList),
        targetWords(targetList)
    {
        letterMap.fill(fullAlphabet);
    }

    inline bool checkTargetWordAgainstMap(const LetterMap& map, const Word& target)
    {
        for(size_t i=0; i< Word::maxSize(); ++i)
        {
            const AlphabetSet& alphabet = map[i];
            if(alphabet.size() == 26)
                continue;

            if(alphabet.find(target[i]) == alphabet.end())
                return false;
        }
        return true;
    }

    inline Word getMatchPattern(const Word& input, const Word& target)
    {
        Word result("-----");
        std::array<unsigned char, 26> matchedLetters{0};
        for(size_t i = 0; i < 5; ++i)
        {
            const char& c = input[i];
            if(target[i] == c)
            {
                result[i] = 'O';
                ++matchedLetters[c - 'A'];
            }
        }
        for(size_t i = 0; i < 5; ++i)
        {
            const char& c = input[i];
            if(c != target[i] && target.count(c) > matchedLetters[c - 'A'])
            {
                result[i] = 'o';
            }
        }
        return result;
    }

    void generateWordmap()
    {
        wordMap.clear();
        wordMap.reserve(allowedWords.size());
        for(const Word& allowed : allowedWords)
        {
            wordMap[allowed];
        }
        std::for_each(std::execution::seq, allowedWords.begin(), allowedWords.end(), [this](const Word& allowed)
        {
            for(unsigned short tgtIndex = 0; tgtIndex < targetWords.size(); ++tgtIndex)
            {
                const Word& target = targetWords[tgtIndex];
                if(checkTargetWordAgainstMap(letterMap, target))
                {
                    Word matchPattern = getMatchPattern(allowed, target);
                    ++wordMap[allowed][matchPattern];
                }
            }
        });
    }

    Word getBestWord()
    {
        Word result;
        std::vector<std::pair<Word, unsigned int>> bestWords;
//        for(const std::pair<Word, std::unordered_map<Word, std::atomic<unsigned short>>>& umap_pair :  wordMap)
//        {
//            unsigned int matchesPerAllowedWord = 0;
//            for(const std::pair<Word, std::atomic<unsigned short>>& word_count_pair : umap_pair.second)
//            {
//                //matchesPerAllowedWord += word_count_pair.second;
//            }
//        }

        return result;
    }



private:

    static const AlphabetSet fullAlphabet;

    LetterMap letterMap;
    std::unordered_map<Word, std::unordered_map<Word, std::atomic<unsigned short>>> wordMap;
    std::unordered_map<Word, std::unordered_map<Word, float>> infoMap;

    const std::vector<Word>& allowedWords;
    const std::vector<Word>& targetWords;
};


