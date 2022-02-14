#include <iostream>
#include <random>
#include <array>
#include "utils.h"
#include "FixedString.h"

using Word = FixedString<5>;
using Alphabet = FixedString<26>;

std::random_device rd;
std::mt19937_64 re(rd());




int main(int argc, char** argv)
{
    std::string mode;
    bool preloaded = false;
    bool debug = false;
    Word preloadedWord;
    if(argc > 1)
    {
        mode = argv[1];
        if(mode == "preload")
        {
            if(argc == 3)
            {
                preloadedWord = argv[2];
                preloaded = true;
                debug = true;
            }
        }
        else if(mode == "debug")
        {
            debug = true;
        }
    }

    std::vector<Word> allowedWords = parseFile<Word>("../data/wordle_allowed.txt");
    std::vector<Word> targetWords = parseFile<Word>("../data/wordle_list.txt");
    if(allowedWords.size() > 0)
    {
        std::uniform_int_distribution<size_t> dist(0, targetWords.size()-1);
        const Word& word = preloaded ? preloadedWord : targetWords[dist(re)];

        if(debug)
        {
            std::cout << "Target word is: " << word.c_str() << std::endl;
        }
        std::cout << "Enter 5 letter word:" << std::endl;
        uint32_t round = 1;
        Alphabet letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        while(round <= 6)
        {
            std::string userInput;
            std::cin >> userInput;
            if(userInput.size() != 5)
            {
                std::cout << "Please enter a 5 letter word:" << std::endl;
                continue;
            }
            else
            {
                Word userWord(userInput);
                if(std::find(allowedWords.begin(), allowedWords.end(), userWord) == allowedWords.end())
                {
                    std::cout << "Not a valid word!" << std::endl;
                    continue;
                }
                else
                {
                    Word output("-----");
                    std::array<unsigned char, 26> matchedLetters{0};
                    // find all exact matches and store their count
                    for(uint32_t i = 0; i < 5; ++i)
                    {
                        char userChar = userWord[i];
                        if(userChar == word[i])
                        {
                            output[i] = 'O';
                            ++matchedLetters[userChar - 'A'];
                        }
                    }
                    for(uint32_t i = 0; i < 5; ++i)
                    {
                        char userChar = userWord[i];
                        if(userChar != word[i] && word.count(userChar) > matchedLetters[userChar - 'A'])
                        {
                            output[i] = 'o';
                        }
                        else if(word.count(userChar) == 0)
                        {
                            letters[userWord[i]-'A'] = '_';
                        }
                    }
                    std::cout << output.c_str() << "        " << letters.c_str() << std::endl;

                    if(output == "OOOOO")
                    {
                        std::cout << "You won! Score: " << round << std::endl;
                        break;
                    }
                }
            }
            ++round;
        }
        if(round == 6)
        {
            std::cout << "You lost! Word was " << word.c_str() << std::endl;
        }
    }
    return 0;
}
