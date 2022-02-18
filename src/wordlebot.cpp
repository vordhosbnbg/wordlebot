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

        gi.generateWordmap();
        Word word = gi.getBestWord();
        std::cout << word.c_str() << std::endl;
    }
    return 0;
}
