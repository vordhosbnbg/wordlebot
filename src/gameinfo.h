#pragma once
#include <vector>
#include <array>
#include <set>
#include <unordered_map>
#include <map>
#include <bitset>
#include <cmath>
#include <iostream>
#include "FixedString.h"
#include "threadpool.h"
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
        mustHaveLetterSet.fill(0);
    }

    inline bool checkTargetWordAgainstMap(const Word& target)
    {
        for(size_t i=0; i< Word::maxSize(); ++i)
        {
            const AlphabetSet& alphabet = letterMap[i];
            if(alphabet.size() == 26)
                continue;

            if(alphabet.find(target[i]) == alphabet.end())
                return false;
        }

        if(!satisfiesMusthaveRules(target))
            return false;

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

    inline bool satisfiesMusthaveRules(const Word& word)
    {
        std::array<unsigned char, 26> mustHaveSet(mustHaveLetterSet);
        if(!mustHaveSet.empty())
        {
            char val;
            for(char i = 0; i < mustHaveSet.size(); ++i )
            {
                if(mustHaveSet[i] > 0)
                {
                    val = i + 'A';
                    if(word.count(val) < mustHaveSet[i])
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void generateWordmap()
    {
        wordMap.clear();
        wordMap.resize(allowedWords.size());
        ThreadPool tPool;
        tPool.start();
        for(unsigned short aIdx = 0; aIdx < allowedWords.size(); ++aIdx)
        {
            tPool.push([this, aIdx]()
            {
                const Word& allowed = getAllowed(aIdx);
                for(unsigned short tgtIndex = 0; tgtIndex < targetWords.size(); ++tgtIndex)
                {
                    const Word& target = targetWords[tgtIndex];
                    if(checkTargetWordAgainstMap(target))
                    {
                        Word matchPattern = getMatchPattern(allowed, target);
                        ++wordMap[aIdx][matchPattern];
                    }
                }
            });
        }
        tPool.waitUntilIdle();
        tPool.start();
        infoMap.clear();
        infoMap.resize(allowedWords.size());
        for(unsigned short aIdx = 0; aIdx < allowedWords.size(); ++aIdx)
        {
            tPool.push([this, aIdx]()
            {
                const std::unordered_map<Word, unsigned short>& allowedWordMap = wordMap[aIdx];
                float totalInfo = 0.0f;

                for(const std::pair<Word, unsigned short>& pattern : allowedWordMap)
                {
                    totalInfo += std::log2f((float)allowedWords.size()/(float)pattern.second);
                }
                totalInfo /= (float)allowedWordMap.size();
                infoMap[aIdx] = totalInfo;
            });
        }
        tPool.waitUntilIdle();
    }

    Word getBestWord()
    {
        Word result("CRANE");
        std::vector<std::pair<Word, unsigned int>> bestWords;
        unsigned short bestIndex = -1;
        float bestValue = 0;
        std::multimap<float, Word> sortedWords;
        for(unsigned short idx = 0; idx < infoMap.size(); ++idx)
        {
            sortedWords.emplace(infoMap[idx], allowedWords[idx]);
        }

        unsigned int nbWords = 5;
        for(auto it = sortedWords.rbegin(); it != sortedWords.rend(); ++it)
        {
            std::cout << it->second.c_str() << " " << it->first << std::endl;
            if(!--nbWords)
                break;
        }

        result = sortedWords.rbegin()->second;
        return result;
    }

    void addAttempt(const Word attempt, const Word& pattern)
    {
        for(unsigned short i = 0; i < 5; ++i)
        {
            const char charA = attempt[i];
            const char charP = pattern[i];
            if(charP == '-')
            {
                for(AlphabetSet& aSet : letterMap)
                {
                    if(aSet.size() != 1)
                        aSet.erase(charA);
                }
            }
            else if(charP == 'o')
            {
                letterMap[i].erase(charA);
                if(mustHaveLetterSet[charA - 'A'] == 0)
                    mustHaveLetterSet[charA - 'A']++;
            }
            else
            {
                letterMap[i] = {charA};
                if(mustHaveLetterSet[charA - 'A'] == 0)
                        mustHaveLetterSet[charA - 'A']++;
            }
        }
        pruneAllowedWords();
    }

    void pruneAllowedWords()
    {
        std::vector<Word> newAllowedWords;
        std::vector<Word> newTargetWords;

        newAllowedWords.reserve(allowedWords.size());
        std::copy_if(allowedWords.begin(), allowedWords.end(), std::back_inserter(newAllowedWords),[this](const Word& allowed)
        {
            return checkTargetWordAgainstMap(allowed);
        });
        allowedWords.swap(newAllowedWords);

        newTargetWords.reserve(targetWords.size());
        std::copy_if(targetWords.begin(), targetWords.end(), std::back_inserter(newTargetWords),[this](const Word& target)
        {
            return checkTargetWordAgainstMap(target);
        });
        targetWords.swap(newTargetWords);

    }

    void targetWordsOnly()
    {
        allowedWords = targetWords;
    }



private:
    inline const Word& getAllowed(unsigned short index)
    {
        return allowedWords[index];
    }

    inline unsigned short getAllowedIndex(const Word& allowed)
    {
        unsigned short result = 0;
        auto it = std::find(allowedWords.begin(), allowedWords.end(), allowed);
        if(it != allowedWords.end())
        {
            result = static_cast<unsigned short>(it - allowedWords.begin());
        }
        return result;
    }

    inline const Word& getTarget(unsigned short index)
    {
        return targetWords[index];
    }

    static const AlphabetSet fullAlphabet;

    LetterMap letterMap;
    std::array<unsigned char, 26> mustHaveLetterSet;
    std::vector<std::unordered_map<Word, unsigned short>> wordMap;
    std::vector<float> infoMap;

    std::vector<Word> allowedWords;
    std::vector<Word> targetWords;
};


