#include "utils.h"
#include "gameinfo.h"

int main(void)
{
    std::vector<Word> allowedWords = parseFile<Word>("../data/wordle_allowed.txt", true);
    std::vector<Word> targetWords = parseFile<Word>("../data/wordle_list.txt", true);
    bool nbGames = 1;
    for(size_t game = 0; game < nbGames; ++game)
    {
        GameInfo gi(allowedWords, targetWords);

        bool finished = false;
        std::string pattern;
        unsigned int round = 0;
        while(!finished)
        {
            ++round;
            if(round > 2)
            {
                gi.targetWordsOnly();
            }
            gi.generateWordmap();
            Word word = gi.getBestWord();
            std::cout << word.c_str() << std::endl;
            std::cin >> pattern;
            if(pattern == "OOOOO" || round == 6)
            {
                finished = true;
            }
            else
            {
                gi.addAttempt(word, pattern);
            }
        }
    }
    return 0;
}
