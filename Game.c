#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>

#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 20
#define EASY_WORD_COUNT 20
#define MEDIUM_WORD_COUNT 40
#define HARD_WORD_COUNT 60

char easy_words[100][5];
char medium_words[100][8];
char hard_words[100][21];
int easy_words_len = 0;
int medium_words_len = 0;
int hard_words_len = 0;
char game_mood;
char passage[MAX_WORDS];
time_t start_time;
void initializeNcurses()
{
    initscr();
    cbreak();
    noecho();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Correct keystrokes - Green
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Incorrect keystrokes - Red
    init_pair(3, COLOR_WHITE, COLOR_BLACK); // Default color
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
}

void cleanupNcurses()
{
    endwin();
}

void load_words()
{
    char word[50];
    while (scanf("%49s", word) != EOF)
    {
        int len = strlen(word);
        if (len <= 4)
        {
            strcpy(easy_words[easy_words_len++], word);
        }
        else if (len <= 7)
        {
            strcpy(medium_words[medium_words_len++], word);
        }
        else
        {
            strcpy(hard_words[hard_words_len++], word);
        }
    }
    fflush(stdin);
    flushinp();
}
void create_passage(char *passage, char mood)
{
    passage[0] = '\0';
    srand(time(NULL));
    if (mood == '1')
    {
        for (int j = 0; j < EASY_WORD_COUNT; j++)
        {
            int random_index = rand() % easy_words_len;
            strcat(passage, easy_words[random_index]);
            strcat(passage, " ");
        }
    }
    else if (mood == '2')
    {
        for (int j = 0; j < MEDIUM_WORD_COUNT; j++)
        {
            int random_index = rand() % medium_words_len;
            strcat(passage, medium_words[random_index]);
            strcat(passage, " ");
        }
    }
    else if (mood == '3')
    {
        for (int j = 0; j < HARD_WORD_COUNT; j++)
        {
            int random_index = rand() % hard_words_len;
            strcat(passage, hard_words[random_index]);
            strcat(passage, " ");
        }
    }
}
void game_menu()
{
    attron(COLOR_PAIR(1));
    printw("Enter in which mood you want play.\n   Press 1 for Easy.\n   Press 2 for Medium.\n   Press 3 for Hard.\n");
    attroff(COLOR_PAIR(1));
    char mood = getch();
    if (mood == '1' || mood == '2' || mood == '3')
    {
        game_mood = mood;
        create_passage(passage, mood);
    }
    else
    {
        attron(COLOR_PAIR(2));
        printw("Please choose a valid option !!!\n");
        attroff(COLOR_PAIR(2));
        game_menu();
    }
}
// Show results after the game ends
void show_results(int total_chars, int correct_chars, int mistakes)
{
    double elapsed = difftime(time(NULL), start_time);
    printf("total char %d", total_chars);
    printf("total correncted %d", correct_chars);
    if (correct_chars > total_chars)
        correct_chars = total_chars - mistakes;
    // Calculate statistics
    double wpm;
    if (game_mood == '1')
        wpm = (correct_chars / 5.0) / (elapsed / 60.0);
    else if (game_mood == '2')
        wpm = (correct_chars / 8.0) / (elapsed / 60.0);
    else
        wpm = (correct_chars / 10) / (elapsed / 60);
    double accuracy = (correct_chars * 100.0) / total_chars;
    double cps = total_chars / elapsed;
    clear();
    // Display results
    printw("=== Typing Results ===\n");
    printw("Words per Minute: %.2f\n", wpm);
    printw("Accuracy: %.2f%%\n", accuracy);
    printw("Characters per Second: %.2f\n", cps);
    printw("Total Mistakes: %d\n", mistakes);
    printw("Time Elapsed: %.2fs\n", elapsed);
}
void main_game()
{
    int correct = 0;
    int incorrect = 0;
    char str[100] = {0};
    int i = 0;

    printw("Type the following text: \n");

    // Print the passage character by character
    for (int j = 0; j < strlen(passage); j++)
    {
        printw("%c", passage[j]);
    }
    printw("\n");
    refresh(); // Refresh screen

    int row, col;
    getyx(stdscr, row, col); // Get current cursor position
    start_time = time(NULL);
    while (1)
    {
        freopen("/dev/tty", "r", stdin);
        double elapsed = difftime(time(NULL), start_time);
        if (elapsed >= 60.0)
            break;
        int ch = getch();
        if (ch == '\n' || ch == 27) // Exit on Enter or Escape
            break;

        if ((ch == 8 || ch == 127) && i > 0) // Handle Backspace
        {
            i--;
            move(row, i);             // Move cursor to the original position
            printw("%c", passage[i]); // Overwrite with original character
            move(row, i);             // Move cursor back
            refresh();
        }
        else if (ch >= 32 && ch <= 126) // Printable characters only
        {
            if (i < sizeof(str) - 1) // Prevent overflow
            {
                if (passage[i] == ch)
                {
                    correct++;
                    attron(COLOR_PAIR(1)); // Green for correct
                    addch(ch);
                    attroff(COLOR_PAIR(1));
                }
                else
                {
                    incorrect++;
                    attron(COLOR_PAIR(2)); // Red for incorrect
                    addch(ch);
                    attroff(COLOR_PAIR(2));
                }
                str[i++] = ch;
                refresh();
            }
        }
    }

    str[i] = '\0'; // Null-terminate the string
    int total_char = strlen(passage);
    show_results(total_char, correct, incorrect);
}

int main()
{
    initializeNcurses();

    load_words();
    freopen("/dev/tty", "r", stdin);

    attron(COLOR_PAIR(4));
    printw("Welcome to the Typing Game.");
    printw("Press any key to start the game.\n");
    attroff(COLOR_PAIR(4));

    getch(); // Wait for user input
    clear();
    printw("Game Started \n");
    game_menu();
    clear();
    main_game();
    refresh();

    getch(); // Wait for final key before exiting
    cleanupNcurses();
    return 0;
}
