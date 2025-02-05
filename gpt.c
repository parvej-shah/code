#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 100
#define MAX_LENGTH 20

void initialize_ncurses()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
}

void cleanup_ncurses()
{
    endwin();
}

void load_words(char words[MAX_WORDS][MAX_LENGTH], int *word_count)
{
    *word_count = 0;
    while (scanf("%s", words[*word_count]) != EOF && *word_count < MAX_WORDS)
    {
        (*word_count)++;
    }
}

void display_passage(char words[MAX_WORDS][MAX_LENGTH], int word_count, char *typed, int length)
{
    clear();
    mvprintw(3, 5, "Type the following:");
    int x = 5, y = 5;
    for (int i = 0; i < length; i++)
    {
        mvprintw(y, x, "%s ", words[i]);
        x += strlen(words[i]) + 1;
    }
    mvprintw(10, 5, "Start typing:");
    for (int i = 0; i < length; i++)
    {
        if (typed[i] == words[i][0])
        {
            attron(COLOR_PAIR(1));
        }
        else
        {
            attron(COLOR_PAIR(2));
        }
        mvprintw(12, 5 + i, "%c", typed[i]);
        attroff(COLOR_PAIR(1) | COLOR_PAIR(2));
    }
    refresh();
}

void typing_test(char words[MAX_WORDS][MAX_LENGTH], int word_count)
{
    char typed[MAX_WORDS] = {0};
    int length = (word_count < 10) ? word_count : 10;
    int pos = 0;
    time_t start_time = time(NULL);
    int ch;
    while ((ch = getch()) != 27)
    {
        if (ch == '\n')
            break;
        if (ch == KEY_BACKSPACE && pos > 0)
        {
            pos--;
        }
        else if (ch >= 32 && ch <= 126 && pos < length)
        {
            typed[pos++] = ch;
        }
        display_passage(words, word_count, typed, length);
        if (time(NULL) - start_time > 60)
            break;
    }
}

void show_results(char words[MAX_WORDS][MAX_LENGTH], char *typed, int length, time_t start_time)
{
    int correct_chars = 0, total_chars = 0, correct_words = 0;
    for (int i = 0; i < length; i++)
    {
        if (typed[i] == words[i][0])
        {
            correct_chars++;
        }
        total_chars++;
    }
    double elapsed_time = difftime(time(NULL), start_time);
    double wpm = (correct_words / elapsed_time) * 60.0;
    double accuracy = (correct_chars / (double)total_chars) * 100;
    clear();
    mvprintw(5, 5, "Typing Speed Test Results:");
    mvprintw(7, 5, "WPM: %.2f", wpm);
    mvprintw(8, 5, "Accuracy: %.2f%%", accuracy);
    mvprintw(10, 5, "Press any key to exit...");
    refresh();
    getch();
}

int main()
{
    char words[MAX_WORDS][MAX_LENGTH];
    int word_count;
    initialize_ncurses();
    load_words(words, &word_count);
    mvprintw(5, 5, "Press any key to start...");
    getch();
    typing_test(words, word_count);
    cleanup_ncurses();
    return 0;
}
