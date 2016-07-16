// =====================================
// Red-Black Trees task number 1
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

//
// Test of class TreeSet:
// Calculate the set of all words in a text,
// and for every word calculate a number of its inclusions
// in the text.
//
// The program reads a text either from a file
// or from standard input stream, depending on how it is called:
//
// 1) the command
//        wrdFreq input_file
//    reads the text from "input_file";
//
// 2) the command
//        wrdFreq
//    without arguments reads the text from standard input.
//    It allows, for instance, the following possibility (in Unix):
//    Define the set of words in all Unix standard include files.
//    It can be done by the command
//        cat /usr/include/*.h | ./wordfreq
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "TreeSet.h"

static void printHelp();

//
// class Word represents a word in a text.
// This is the dynamic array of characters.
// The class Word is derived from class TreeSetKey,
// so the object of this class can be used
// as keys in TreeSet
//
// Interface of class Word:
//
class Word: public TreeSetKey {
    char* str;
    int   len;
    int capacity;
public:
    Word();
    Word(const char* s, int l = (-1));  // (-1) means "undefined"
    Word(const Word& w);
    virtual ~Word() { delete[] str; }
    virtual Word* clone() const { return new Word(*this); }

    int length() const { return len; }
    int size() const { return len; }
    void initialize();

    // Add a character to the end of word
    Word& operator+=(int c);

    // Convertor to C-string
    operator const char *() { return str; }
    const char* getString() const { return str; }

    virtual bool operator==(const TreeSetKey& s) const {
        return (strcmp(str, ((Word&) s).str) == 0);
    }

    virtual int compareTo(const TreeSetKey& key) const {
        return strcmp(str, ((const Word&) key).str);
    }
};

// Implementation of class Word
Word::Word():
    TreeSetKey(),
    str(0),
    len(0),
    capacity(0)
{}

Word::Word(const char* s, int l):
    TreeSetKey(),
    str(0),
    len(0),
    capacity(0)
{
    assert(str != 0);
    if (l < 0)
        l = strlen(str);
    len = l;
    capacity = len + 1;
    str = new char[capacity];
    memmove(str, s, len);
    str[len] = 0;
}

Word::Word(const Word& w):
    TreeSetKey(w),
    str(0),
    len(w.len),
    capacity(w.capacity)
{
    if (w.len > 0) {
        str = new char[w.capacity];
        memmove(str, w.str, capacity);
    }
}

void Word::initialize() {
    len = 0;
    if (capacity > 0)
        str[0] = 0;
}

// Add a character to the end of word
Word& Word::operator+=(int c) {
    if (capacity <= len+1) {
        // Allocate additional space
        int extent = capacity / 8;
        if (extent < 16)
            extent = 16;
        char* new_str = new char[capacity + extent];
        memmove(new_str, str, len);
        delete[] str;
        capacity += extent;
        str = new_str;
    }
    str[len] = (char) c;
    ++len;
    str[len] = 0;
    return *this;
}

// class Integer represents the number of inclusions of word in text
class Integer: public TreeSetValue {
public:
    int number;
public:
    Integer(): 
        TreeSetValue(),
        number(0) 
    {}

    Integer(const Integer& i): 
        TreeSetValue(),
        number(i.number) 
    {}

    Integer(int n): 
        TreeSetValue(),
        number(n) 
    {}

    virtual ~Integer() {}

    virtual Integer* clone() const {
        return new Integer(*this);
    }
};

int main(int argc, char *argv[]) {
    TreeSet set;
    FILE* input;
    if (argc > 1) {
        if (*argv[1] == '-') {
            printHelp();
            return 0;
        }
        input = fopen(argv[1], "r");
        if (input == 0) {
            perror("Cannot open an input file");
            return 1;
        }
    } else {
        input = stdin;  // Standard input
    }

    Word currentWord;
    bool wasLetter = false;
    bool endOfFileDetected = false;
    while (!endOfFileDetected) {
        int c = fgetc(input);
        if (isalpha(c)) {       // If this is an English letter
            currentWord += c;   // Add a letter to a word
            wasLetter = true;
        } else {
            if (wasLetter) {
                // The end of current word detected
                if (set.contains(&currentWord)) {
                    Integer* val = (Integer*) set.value(&currentWord);
                    ++(val->number);    // Increment a number of inclusions
                } else {
                    Integer unit(1);
                    set.add(&currentWord, &unit);
                }
                currentWord.initialize();
            }
            wasLetter = false;
            endOfFileDetected = (c == EOF);
        }
    }

    // Print the set of words in the text and
    // define the most frequent word
    printf("The text contains the following words:\n");

    TreeSet::const_iterator i = set.begin();
    TreeSet::const_iterator e = set.end();
    Word mostFrequentWord;
    int numberOfMostFrequentWords = 0;
    while (i != e) {
        const TreeSet::Pair& pair = *i;
        const Word* w = (const Word *) pair.key;
        int number = ((const Integer *) pair.value)->number;
        printf(
            "%d\t%s\n", number, w->getString()
        );

        // Define the most frequent word
        if (number > numberOfMostFrequentWords) {
            numberOfMostFrequentWords = number;
            mostFrequentWord = *w;
        }
        ++i;
    }

    printf(
        "----\n"
        "Number of different words in the text = %d\n",
        set.size()
    );

    printf(
        "The most frequent word is \"%s\", included %d times.\n",
        mostFrequentWord.getString(), numberOfMostFrequentWords
    );

    return 0;
}

static void printHelp() {
    printf(
        "Calculate the set of all words in a text,\n"
        "and for every word calculate a number of its inclusions\n"
        "in the text.\n"
        "Usage:\n"
        "    wordfreq [input_file]\n"
        "The program reads a text either from a file\n"
        "or from standard input stream, depending on how it is called.\n\n"
        "EXAMPLES:\n"
        "1) the command\n"
        "       ./wordfreq input_file\n"
        "   reads the text from \"input_file\";\n"
        "\n"
        "2) the Unix-command\n"
        "       cat /usr/include/*.h | ./wordfreq\n"
        "   defines the set of words in all standard include files.\n"
    );
}
