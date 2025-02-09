#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <math.h>
// Constants for game configuration
#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 20
#define EASY_WORD_COUNT 20
#define MEDIUM_WORD_COUNT 40
#define HARD_WORD_COUNT 60
// Function prototypes
void initializeNcurses();
void cleanupNcurses();
void load_words();
void create_passage(char *passage, char mood);
void show_progress(double elapsed, int words_typed, int correct, int incorrect);
void game_menu();
void show_results(int total_chars, int correct_chars, int mistakes);
void main_game();
// CHANGE: Fixed array sizes based on word length categories
char easy_words[100][5];   // 1-4 letters
char medium_words[100][8]; // 5-7 letters
char hard_words[100][21];  // 8+ letters
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
    keypad(stdscr, TRUE); // CHANGE: Added for better key handling
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
}

void cleanupNcurses()
{
    endwin();
}

// CHANGE: Fixed word categorization logic
void load_words()
{
    char word[50];
    while (scanf("%49s", word) != EOF)
    {
        int len = strlen(word);
        if (len >= 8)
        {
            strcpy(hard_words[hard_words_len++], word);
        }
        else if (len >= 5 && len <= 7)
        {
            strcpy(medium_words[medium_words_len++], word);
        }
        else
        {
            strcpy(easy_words[easy_words_len++], word);
        }
    }
    fflush(stdin);
    flushinp();
}
void create_passage(char *passage, char mood)
{
    passage[0] = '\0';
    int word_count;
    if (mood == '1')
    {
        word_count = EASY_WORD_COUNT;
    }
    else if (mood == '2')
    {
        word_count = MEDIUM_WORD_COUNT;
    }
    else
    {
        word_count = HARD_WORD_COUNT;
    }

    for (int j = 0; j < word_count; j++)
    {
        int random_index;
        char *word;
        if (mood == '1')
        {
            if (easy_words_len == 0)
                break;
            random_index = rand() % easy_words_len;
            word = easy_words[random_index];
        }
        else if (mood == '2')
        {
            if (medium_words_len == 0)
                break;
            random_index = rand() % medium_words_len;
            word = medium_words[random_index];
        }
        else
        {
            if (hard_words_len == 0)
                break;
            random_index = rand() % hard_words_len;
            word = hard_words[random_index];
        }
        strcat(passage, word);
        strcat(passage, " ");
    }
    // Remove trailing space
    if (passage[0] != '\0')
    {
        int len = strlen(passage);
        passage[len - 1] = '\0';
    }
}
// CHANGE: Added progress display function
void show_progress(double elapsed, int words_typed, int correct, int incorrect)
{
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    move(0, max_x - 80);
    clrtoeol();
    int total_keystrokes = correct + incorrect;
    double accuracy = (total_keystrokes > 0 ? (correct * 100.0 / total_keystrokes) : 100.0);
    double wpm = elapsed > 0 ? ceil(words_typed / (elapsed / 60.0)) : 0;
    printw("Time: %.0f seconds | Correct %d | Incorrect %d | Words: %d | WPM: %.0f | Accuracy: %.0f%%", elapsed, correct, incorrect, words_typed, wpm, accuracy);
    refresh();
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
// CHANGE: Fixed results calculation and display
void show_results(int total_chars, int correct_chars, int mistakes)
{
    double elapsed = difftime(time(NULL), start_time);
    if (correct_chars > total_chars)
        correct_chars = total_chars - mistakes;

    // CHANGE: Improved WPM calculation
    int avg_word_length = (game_mood == '1') ? 4 : (game_mood == '2' ? 6 : 8);
    double words = (double)correct_chars / avg_word_length;
    double wpm = ceil((words / elapsed) * 60.0);
    double accuracy = (correct_chars * 100.0) / (correct_chars + mistakes);
    double cps = ceil((correct_chars) / elapsed);
    int max_y, max_x;
    clear();
    getmaxyx(stdscr, max_y, max_x);
    int start_x = (max_x - 40) / 2;
    int start_y = (max_y - 10) / 2;
    move(start_y, start_x);
    printw("=== Typing Results ===\n");
    move(start_y + 1, start_x);
    printw("Words per Minute: %.0f\n", wpm);
    move(start_y + 2, start_x);
    printw("Accuracy: %.2f%%\n", accuracy);
    move(start_y + 3, start_x);
    printw("Characters per Second: %0.0f\n", cps);
    move(start_y + 4, start_x);
    printw("Total Mistakes: %d\n", mistakes);
    move(start_y + 5, start_x);
    printw("Time Elapsed: %.0fs\n", elapsed);
    move(start_y + 6, start_x);
    printw("\nPress 'r' to restart or any other key to exit\n");
    refresh();

    // CHANGE: Added restart functionality
    int ch = getch();
    if (ch == 'r' || ch == 'R')
    {
        clear();
        game_menu();
        main_game();
    }
    else
    {
        clear();
        move(max_y / 2, max_x / 2 - 4);
        attron(COLOR_PAIR(4));
        printw("Game Ended!");
        move(max_y / 2 + 1, max_x / 2 - 6);
        printw("See You Soon.....");
        attroff(COLOR_PAIR(4));
    }
}

// CHANGE: Completely revamped main_game with fixes
void main_game()
{
    int correct = 0;
    int incorrect = 0;
    char str[MAX_WORDS] = {0};
    int i = 0;
    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);

    move(3, 0);
    clear();
    printw("Type the following text:\n\n");

    // CHANGE: Added word wrapping
    int curr_x = 0;
    for (int j = 0; j < strlen(passage); j++)
    {
        if (curr_x >= max_x - 1)
        {
            printw("\n");
            curr_x = 0;
        }
        printw("%c", passage[j]);
        curr_x++;
    }
    printw("\n\n");
    refresh();

    int input_row, input_col;
    getyx(stdscr, input_row, input_col);
    start_time = time(NULL);

    while (1)
    {
        double elapsed = difftime(time(NULL), start_time);
        int avg_word_length = (game_mood == '1') ? 4 : (game_mood == '2' ? 6 : 8);
        show_progress(elapsed, i / avg_word_length, correct, incorrect);
        if (elapsed >= 60.0)
            break;
        int ch = getch();
        if (ch == 27 || ch == '\n') // ESC or enter
            break;

        // CHANGE: Improved backspace handling
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') && i > 0)
        {
            i--;
            int curr_row = input_row + (i / max_x);
            int curr_col = i % max_x;
            move(curr_row, curr_col);
            attron(COLOR_PAIR(3));
            printw(" ");
            attroff(COLOR_PAIR(3));
            move(curr_row, curr_col);
            // Ihandle correct character clear
            if (str[i] == passage[i])
            {
                correct--;
            }
            else
            {
                incorrect--;
            }
            refresh();
            continue;
        }

        if (ch >= 32 && ch <= 126 && i < strlen(passage))
        {
            int curr_row = input_row + (i / max_x);
            int curr_col = i % max_x;

            move(curr_row, curr_col);
            if (passage[i] == ch)
            {
                correct++;
                attron(COLOR_PAIR(1));
                addch(ch);
                attroff(COLOR_PAIR(1));
            }
            else
            {
                incorrect++;
                attron(COLOR_PAIR(2));
                addch(ch);
                attroff(COLOR_PAIR(2));
            }
            str[i++] = ch;
            refresh();
        }
    }

    str[i] = '\0';
    show_results(strlen(passage), correct, incorrect);
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
