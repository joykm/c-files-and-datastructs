# define _POSIX_C_SOURCE 200809L

// Custom colors for printing to console
#define YEL "\x1B[33m"
#define RED "\x1B[31m"
#define BLU "\x1b[34m"
#define WHT "\x1B[0m"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 
* Struct for movie information 
*/
struct movie {
    char* title;
    int year;
    char* languages[5];
    float rating;
    struct movie* next;
};

/* 
* Struct for highest rated movie of each year 
*/
struct highestRating {
    int year;
    float rating;
    char* title;
    struct highestRating* next;
};

/* 
* Parse the current line which is space delimited and create a
* movie struct with the data in this line 
*/
struct movie* createMovie(char* currLine)
{
    struct movie* currMovie = malloc(sizeof(struct movie));
    
    // For use with strtok_r
    char* savePtr;
    char* saveLangPtr;

    // Parse the title
    char* token = strtok_r(currLine, ",", &savePtr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // Parse the year
    token = strtok_r(NULL, ",", &savePtr);
    currMovie->year = atoi(token);

    // Parse the first language
    token = strtok_r(NULL, ",", &savePtr);
    char* langToken = strtok_r(token, "[;]", &saveLangPtr);
    currMovie->languages[0] = calloc(strlen(langToken) + 1, sizeof(char));
    strcpy(currMovie->languages[0], langToken);
    langToken = strtok_r(NULL, ";]", &saveLangPtr);
    int i = 1;

    // Parse the remaining languages
    while (i < 5) {

        if (langToken != NULL) {
            currMovie->languages[i] = calloc(strlen(langToken) + 1, sizeof(char));
            strcpy(currMovie->languages[i], langToken);
            langToken = strtok_r(NULL, ";]", &saveLangPtr);
        } else {
            currMovie->languages[i] = NULL;
        }
        i++;
    }

    // Parse the rating
    token = strtok_r(NULL, "\n", &savePtr);
    currMovie->rating = atof(token);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}

/*
* Return a linked list of movies by parsing data from
* each line of the specified file.
*/
struct movie* processFile(char* filePath)
{

    // Check if we can access the given file path, if not exit program with warning
    if (access(filePath, F_OK) != 0) {
        printf("%sFile path \"%s\" not found, please try again%s\n", RED, filePath, WHT);
        printf("%sExample usage: ./movies movies_sample_1.csv%s\n\n", RED, WHT);
        exit(0);
    }

    // Open the specified file for reading only
    FILE* movieFile = fopen(filePath, "r");
    
    char* currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char* token;
    int count = 0;

    // Initiate the head and tail of the linked list
    struct movie* head = NULL;
    struct movie* tail = NULL;

    // Read the header line of the file in but don't parse
    nread = getline(&currLine, &len, movieFile);

    // Read the remaining file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        // Create a new movie node with parsed info from currLine
        struct movie* newNode = createMovie(currLine);

        // Is this the first node in the linked list?
        if (head == NULL)
        {
            /* This is the first node in the linked link,
            set the head and the tail to this node */
            head = newNode;
            tail = newNode;
        }
        else
        {
            /* this is not the first node, add this node to 
            the list and advance the tail */
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }
    // Free the memory allocated for currLine
    free(currLine);
    fclose(movieFile);
    printf("%s\nProcessed file \"%s\" and parsed data for %d movies%s\n\n", YEL, filePath, count, WHT);
    return head;
}

/*
* Populate the fields of the currRating node
*/
struct highestRating* createRating(struct highestRating* ratings, int year, float rating, char* title) {

    struct highestRating* currRating = malloc(sizeof(struct highestRating));
    
    currRating->year = year;
    currRating->rating = rating;
    currRating->title = calloc(strlen(title) + 1, sizeof(char));
    strcpy(currRating->title, title);
    currRating->next = NULL;

    return currRating;
}

/*
* Iterate each move and keep track of the highest movie 
* per year in a new linked list
*/
struct highestRating* processRatings(struct movie* movie){

    struct highestRating* head = NULL;
    struct highestRating* tail = NULL;
    struct highestRating* ratingsTempHead = NULL;
    struct highestRating* savedRatingsPrevPtr = NULL;
    struct highestRating* savedRatingsPtr = NULL;
    struct highestRating* savedRatingsNextPtr = NULL;
    
    // Loop each movie in the movie linked list
    int count = 0;
    int test = 0;
    while (movie != NULL){
        
        // Create the first highestRating node
        if (count == 0) {
            struct highestRating* newNode = createRating(head, movie->year, movie->rating, movie->title);

            // Is this the first node in the linked list?
            if (head == NULL){

                /* This is the first node in the linked link
                set the head and the tail to this node */
                head = newNode;
                tail = newNode;
            } else {

                /* This is not the first node, add this node to the 
                list and advance the tail */
                tail->next = newNode;
                tail = newNode;
            }
        }

        // Re-initialize variables for each movie
        ratingsTempHead = head;
        int dupe = 0;
        
        // For each movie, loop ratings linked list looking for duplicate years
        while (ratingsTempHead != NULL) {
        
            // Set dupe flag when duplicate is found
            if (ratingsTempHead->year == movie->year) {
                dupe = 1;
                savedRatingsPtr = ratingsTempHead;
            }

            // Advance temp head
            ratingsTempHead = ratingsTempHead->next;

        }

        // Is there a movie in highestRating with the same year as the current movie?
        if (dupe == 0) {
            
            // There is no duplicate, create a new node in highestRating linked list
            struct highestRating* newNode = createRating(head, movie->year, movie->rating, movie->title);

            // Is this the first node in the linked list?
            if (head == NULL){

                /* This is the first node in the linked link,
                set the head and the tail to this node */
                head = newNode;
                tail = newNode;
            } else {

                /* This is not the first node add this node to the list
                and advance the tail */
                tail->next = newNode;
                tail = newNode;
            }
            test++;
        } else {
            
            // There is a duplicate, take highest rated movie between highestRating and current movie
            if (movie->rating > savedRatingsPtr->rating) {
                savedRatingsPtr->year = movie->year;
                savedRatingsPtr->rating = movie->rating;

                // Free the memory allocated for the old title, to reallocate and replace
                free(savedRatingsPtr->title);
                savedRatingsPtr->title = calloc(strlen(movie->title) + 1, sizeof(char));
                strcpy(savedRatingsPtr->title,movie->title);
            }
        }

        // Reinitialize ptr
        savedRatingsPtr = NULL;

        // Iterate count and move node
        count++;
        movie = movie->next;
    }

    // Free temporary memory allocations 
    free(ratingsTempHead);
    free(savedRatingsPtr);
    return head;
}


/*
* Print all movies in a given year, print error
* if there is no movie for given year
*/
void specificYear(int year, struct movie* movie){

    // Iterate the movies linked list printing matches for the specific year
    int count = 0;
    while (movie != NULL) {

        // Match found, print information request
        if (movie->year == year){
            printf("%s** %s%s\n", YEL, movie->title, WHT);
            count++;
        }
        movie = movie->next;
    }

    // No match found, print warning
    if (count == 0) {
        printf("%s** No data about movies released in the year %d%s\n", RED, year, WHT);
    }
}

/*
* Print the highest rated movie of every year in the movie file
*/
void printRatings(struct highestRating* ratings) {

    // Iterate the highestRatings linked list printing the highest rated movie each year
    while (ratings != NULL) {
        printf("%s** %d %.1f %s%s\n", YEL, ratings->year, ratings->rating, ratings->title, WHT);
        ratings = ratings->next;
    }
}

/*
* Print all movies released in a specific language, print an
* error if there are no movies released in given langauge
*/
void specificLanguage(char* language, struct movie* movie){

    // Iterate each movie searching for language matches
    int count = 0;
    while (movie != NULL) {
        int i = 0;

        // Iterate each language of each movie searching language matches
        while (i < 5 && movie->languages[i] != NULL){

            // Is there a match?
            if (strcmp(language, movie->languages[i])){

                // No match, pass
            } else {

                // Match, print information requested
                printf("%s** %d %s%s\n", YEL, movie->year, movie->title, WHT);
                count++;
            };
            i++;
        }
        movie = movie->next;
    }

    // No matches found at all, print warning
    if (count == 0) {
        printf("%s** No data about movies released in %s%s\n", RED, language, WHT);
    }
}

/*
* Loop through the movies linked list deallocating the memory for the 
* languages, titles, and nodes of each movie
*/
void deallocateMovies(struct movie* movie){

    // Loop through the movies linked list deallocating memory of each movie
    while (movie != NULL) {

        // Deallocate the memory of each language for each movie
        int i = 0;
        while (i < 5 && movie->languages[i] != NULL){
            free(movie->languages[i]);
            i++;
        }

        // Deallocate title
        free(movie->title);

        // Save prev node, advance node, deallocate prev node
        struct movie* prevNode = movie;
        movie = movie->next;
        free(prevNode);
    }
}

/*
* Loop through the highestRating linked list deallocating the memory for the 
* titles and nodes of each movie
*/
void deallocateRatings(struct highestRating* ratings){

    // Loop through the movies linked list deallocating memory of each movie
    while (ratings != NULL) {

        // Deallocate title
        free(ratings->title);

        // Save node as prevNode, advance node, deallocate prevNode
        struct highestRating* prevNode = ratings;
        ratings = ratings->next;
        free(prevNode);
    }
}

/*
* Print the UI for movie menu to console
*/
void movieMenu(struct movie* list, struct highestRating* ratings) {

    int choice, year, end = 0;
    char language[20];

    // Display the menu unless the user chooses to end the program
    while (end == 0) {
        printf("1. Show movies released in the specified year\n"
                "2. Show highest rated movie for each year\n"
                "3. Show the title and year of release of all movies in a specific language\n"
                "4. Exit from program\n\n"
                "Enter a choice from 1 to 4: ");

        scanf("%d", &choice);

        // Display error message and menu again if invalid input is submitted
        while (choice < 1 || choice >4) {
            printf("%sInvalid choice, please try again%s\n\n", RED, WHT);

            printf("1. Show movies released in the specified year\n"
            "2. Show highest rated movie for each year\n"
            "3. Show the title and year of release of all movies in a specific language\n"
            "4. Exit from program\n\n"
            "Enter a choice from 1 to 4: ");

            scanf("%d", &choice);
        }

        // User choices upon valid input
        switch(choice) {
            case 1:
                printf("Enter the year for which you want to see movies: ");
                scanf("%d", &year);
                specificYear(year, list);
                break;
            case 2:
                printRatings(ratings);
                break;
            case 3:
                printf("Enter the language for which you want to see movies: ");
                scanf("%s", language);
                specificLanguage(language, list);
                break;
            default:
                deallocateMovies(list);

                if (ratings != NULL){
                    deallocateRatings(ratings);
                }
                end++;
        }
        printf("\n");
    }
}

/*
*   Process the file provided as an argument to the program to
*   create a linked list of student structs and print out the list.
*   Compile the program as follows:
*       gcc --std=c99 -o movies movies.c
*/
int main(int argc, char *argv[]) {

    // If not enough arguments are provided, give warning and exit program
    if (argc < 2)
    {
        printf("%sYou must provide the name of the file to process%s\n", RED, WHT);
        printf("%sExample usage: ./movies movies_sample_1.csv%s\n\n", RED, WHT);
        return EXIT_FAILURE;
    }

    // Process the provided filee
    struct movie* list = processFile(argv[1]);

    // Process the highest ratings per year in the provided file
    struct highestRating* ratings = processRatings(list);

    // Display user menu
    movieMenu(list, ratings);
    return EXIT_SUCCESS;
}