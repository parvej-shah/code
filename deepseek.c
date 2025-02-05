#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>

#define MAX_WORDS 1000
#define EASY_WORDS 20
#define MEDIUM_WORDS 40
#define HARD_WORDS 60

typedef struct
{
    char **words;
    int count;
} WordList;

typedef enum
{
    EASY,
    MEDIUM,
    HARD
} Difficulty;

// Global variables
WordList easy_words, medium_words, hard_words;
char *passage = NULL;
int passage_length = 0;
time_t start_time;

// Function prototypes
void load_words();
void generate_passage(Difficulty diff);
void run_game();
void show_results(int total_chars, int correct_chars, int mistakes);
Difficulty select_difficulty();

// Load words from stdin and categorize them
void load_words()
{
    easy_words.words = malloc(MAX_WORDS * sizeof(char *));
    medium_words.words = malloc(MAX_WORDS * sizeof(char *));
    hard_words.words = malloc(MAX_WORDS * sizeof(char *));
    easy_words.count = medium_words.count = hard_words.count = 0;

    char word[50];
    while (scanf("%49s", word) != EOF)
    {
        int len = strlen(word);
        if (len <= 4)
        {
            easy_words.words[easy_words.count++] = strdup(word);
        }
        else if (len <= 7)
        {
            medium_words.words[medium_words.count++] = strdup(word);
        }
        else
        {
            hard_words.words[hard_words.count++] = strdup(word);
        }
    }
}

// Generate a passage based on difficulty
void generate_passage(Difficulty diff)
{
    WordList *selected = NULL;
    int word_count = 0;

    switch (diff)
    {
    case EASY:
        selected = &easy_words;
        word_count = EASY_WORDS;
        break;
    case MEDIUM:
        selected = &medium_words;
        word_count = MEDIUM_WORDS;
        break;
    case HARD:
        selected = &hard_words;
        word_count = HARD_WORDS;
        break;
    }

    // Allocate memory for passage
    passage = malloc(word_count * 50 * sizeof(char));
    passage[0] = '\0';

    // Randomly select words
    srand(time(NULL));
    for (int i = 0; i < word_count; i++)
    {
        int index = rand() % selected->count;
        strcat(passage, selected->words[index]);
        strcat(passage, " ");
    }
    passage_length = strlen(passage);
}

// Run the typing game
void run_game()
{
    int ch, pos = 0, correct = 0, mistakes = 0;
    bool running = true;

    // ncurses setup
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);

    // Display passage
    mvprintw(0, 0, "Type the following text:");
    mvprintw(1, 0, "%s", passage);
    refresh();

    start_time = time(NULL);

    while (running)
    {
        // Check time
        double elapsed = difftime(time(NULL), start_time);
        if (elapsed >= 60.0)
            break;

        // Get input
        ch = getch();
        if (ch == ERR)
            continue;

        if (pos >= passage_length)
            continue;

        // Handle input
        if (ch == passage[pos])
        {
            attron(COLOR_PAIR(1));
            correct++;
        }
        else
        {
            attron(COLOR_PAIR(2));
            mistakes++;
        }

        mvaddch(2, pos, ch);
        pos++;
        refresh();
        attroff(COLOR_PAIR(1) | COLOR_PAIR(2));
    }

    endwin();
    show_results(pos, correct, mistakes);
}

// Show results after the game ends
void show_results(int total_chars, int correct_chars, int mistakes)
{
    double elapsed = difftime(time(NULL), start_time);

    // Calculate statistics
    double wpm = (correct_chars / 5.0) / (elapsed / 60.0);
    double accuracy = (correct_chars * 100.0) / total_chars;
    double cps = total_chars / elapsed;

    // Display results
    printf("\n=== Typing Results ===\n");
    printf("Words per Minute: %.2f\n", wpm);
    printf("Accuracy: %.2f%%\n", accuracy);
    printf("Characters per Second: %.2f\n", cps);
    printf("Total Mistakes: %d\n", mistakes);
    printf("Time Elapsed: %.2fs\n", elapsed);
}

// Let the user select difficulty
Difficulty select_difficulty()
{
    int choice;
    printf("Select difficulty:\n");
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("3. Hard\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        return EASY;
    case 2:
        return MEDIUM;
    case 3:
        return HARD;
    default:
        return EASY; // Default to easy
    }
}

// Main function
int main()
{
    load_words();
    Difficulty diff = select_difficulty();
    generate_passage(diff);
    run_game();

    // Cleanup
    free(passage);
    for (int i = 0; i < easy_words.count; i++)
        free(easy_words.words[i]);
    for (int i = 0; i < medium_words.count; i++)
        free(medium_words.words[i]);
    for (int i = 0; i < hard_words.count; i++)
        free(hard_words.words[i]);
    free(easy_words.words);
    free(medium_words.words);
    free(hard_words.words);

    return 0;
}