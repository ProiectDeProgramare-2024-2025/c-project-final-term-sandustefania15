/*
Programming Project: Hangman Game
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 18
#define MAX_NAME_LEN 50
#define MAX_WRONG_GUESSES 5
#define MAX_PLAYERS 10

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define BOLDWHITE "\033[1;37m"

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

void enableVirtualTerminal()
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

char wordList[MAX_WORDS][MAX_NAME_LEN];
int wordCount = 0;

void loadWordsFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open %s\n", filename);
        exit(1);
    }

    wordCount = 0;
    while (fscanf(file, "%s", wordList[wordCount]) != EOF)
    {
        wordCount++;
        if (wordCount >= MAX_WORDS)
            break;
    }

    fclose(file);
}

typedef struct
{
    char name[MAX_NAME_LEN];
    int score;
} Player;

Player leaderboard[MAX_PLAYERS];

int playerCount = 0;

void playGame();
void viewLeaderboard();
void showHistory();
void saveScore(const char *name, int score);
void menu();
void clearScreen();
void header();

void clearScreen()
{
    system("cls");
}


void header()
{
    printf(CYAN
           "  _    _                                            _____                      \n"
           " | |  | |                                          / ____|                     \n"
           " | |__| | __ _ _ __   __ _ _ __ ___   __ _ _ __   | |  __  __ _ _ __ ___   ___ \n"
           " |  __  |/ _` | '_ \\ / _` | '_ ` _ \\ / _` | '_ \\  | | |_ |/ _` | '_ ` _ \\ / _ \\\n"
           " | |  | | (_| | | | | (_| | | | | | | (_| | | | | | |__| | (_| | | | | | |  __/\n"
           " |_|  |_|\\__,_|_| |_|\\__, |_| |_| |_|\\__,_|_| |_|  \\_____|\\__,_|_| |_| |_|\\___|\n"
           "                      __/ |                                                    \n"
           "                     |___/                                                     \n" RESET);

    printf("--------------------\n");
    printf(BLUE "1" RESET " - Play Hangman\n");
    printf(BLUE "2" RESET " - View Leaderboard\n");
    printf(BLUE "3" RESET " - View History\n");
    printf(BLUE "0" RESET " - Exit\n");
    printf("--------------------\n");
}


void saveScore(const char *name, int score)
{
    FILE *file = fopen("leaderboard.txt", "a"); 

    if (file == NULL)
    {
        printf("Error opening file for writing.\n");
        return;
    }

    fprintf(file, "%s,%d\n", name, score);
    fclose(file);
}

void playGame()
{
    clearScreen();
    printf(BOLDWHITE"----" RESET CYAN" Play Hangman " BOLDWHITE"----\n" RESET);
    srand(time(NULL));

    int totalScore = 0;
    int gameOver = 0;

    while (!gameOver)
    {

       if (wordCount == 0)
       {
           printf("Error: No words loaded.\n");
           return;
       }
       int index = rand() % wordCount;
       const char *word = wordList[index];


       int wordLen = strlen(word);
       char guessedWord[wordLen + 1];
       int wrongGuesses = 0;
       int guessedLetters[30] = {0};

       for (int i = 0; i < wordLen; i++)
           guessedWord[i] = '_';
       guessedWord[wordLen] = '\0';

       guessedWord[0] = word[0];
       guessedWord[wordLen - 1] = word[wordLen - 1];

       for (int i = 1; i < wordLen - 1; i++)
           if (word[i] == word[0] || word[i] == word[wordLen - 1])
               guessedWord[i] = word[i];

       int wordCompleted = 0;

       while (!wordCompleted && wrongGuesses < MAX_WRONG_GUESSES)
       {
           printf(BLUE "\nWord:" RESET " %s\n", guessedWord);
           printf(BLUE "Enter a letter: " RESET);
           char guess;
           scanf(" %c", &guess);

           if (guessedLetters[guess - 'a'])
           {
               printf("You already guessed '%c'. Try again.\n", guess);
               continue;
           }
           guessedLetters[guess - 'a'] = 1;

           int found = 0;
           for (int i = 1; i < wordLen - 1; i++)
           {
               if (word[i] == guess)
               {
                   guessedWord[i] = guess;
                   found = 1;
               }
           }

           if (!found)
           {
               wrongGuesses++;
               printf("Incorrect!" RED " %d" RESET " wrong guesses. Guesses left:" GREEN " %d\n" RESET, wrongGuesses, MAX_WRONG_GUESSES - wrongGuesses);
           }

           if (strcmp(guessedWord, word) == 0)
           {
               printf("You guessed the word: %s!\n", word);
               totalScore += wordLen;
               wordCompleted = 1;
           }
        }

        if (wrongGuesses >= MAX_WRONG_GUESSES)
        {
            gameOver = 1;
        }
    }

    printf("\nGame Over! Your total score:" MAGENTA" %d\n" RESET, totalScore);

    char name[MAX_NAME_LEN];
    printf("Enter your name: ");
    scanf(" %s", name);
    saveScore(name, totalScore); 
}

void viewLeaderboard()
{
    clearScreen();
    printf(BOLDWHITE "----" RESET CYAN " Leaderboard " BOLDWHITE "----\n" RESET);

    FILE *file = fopen("leaderboard.txt", "r");

    if (file == NULL)
    {
        printf("No leaderboard data found.\n");
        return;
    }

    Player players[MAX_PLAYERS];
    int count = 0;

    while (fscanf(file, "%[^,],%d\n", players[count].name, &players[count].score) != EOF)
    {
        count++;
        if (count >= MAX_PLAYERS)
            break; 
    }

    fclose(file);

    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (players[i].score < players[j].score)
            {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    for (int i = 0; i < count; i++)
    {
        printf("%d. " BOLDWHITE "%s" RESET "  - " GREEN "%d "RESET "points\n", i + 1, players[i].name, players[i].score);
    }

    printf("\n");
}


void showHistory()
{
    clearScreen();
    printf(BOLDWHITE "----" RESET CYAN " View History " BOLDWHITE "----\n" RESET);

    char searchName[MAX_NAME_LEN];
    printf("Enter name to view history: ");
    scanf(" %s", searchName);

    FILE *file = fopen("leaderboard.txt", "r");

    if (file == NULL)
    {
        printf("No history found.\n");
        return;
    }

    char name[MAX_NAME_LEN];
    int score;
    int found = 0;

    printf("History for %s:\n", searchName);

    while (fscanf(file, "%[^,],%d\n", name, &score) != EOF)
    {
        if (strcmp(name, searchName) == 0)
        {
            printf("%s - %d points\n", name, score);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No history found for %s.\n", searchName);
    }

    fclose(file);
    printf("\n");
}

void menu(int op)
{
    clearScreen();
    switch (op)
    {
    case 1:
        playGame();
        break;
    case 2:
        viewLeaderboard();
        break;
    case 3:
        showHistory();
        break;
    default:
        printf("EXIT!\n");
    }
}

int main()
{
    enableVirtualTerminal();

    loadWordsFromFile("words.txt");

    int option;
    do
    {   
        header();
        printf("Enter option: ");
        scanf("%d", &option);
        menu(option);
    } while (option > 0 && option <= 3);

    return 0;
}