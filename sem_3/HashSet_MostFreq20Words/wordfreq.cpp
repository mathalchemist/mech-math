// Поиск 20 слов. алгоритм работы:
// делаем 2 таблицы (массива) на 20 элементов для слов и их частот,
// соответственно Words[] и Freqs[].
// Ещё храним две переменные: текущая длина таблиц (Length, первоначально = 0),
// и Индекс_Минимального_Элемента (MinIndex) в таблице частот.
//
// ход работы: Идём по множеству, вынимаем из него очередное слово (w) и его
// частоту (number).
// 1. если таблица ещё не заполнилась, т.е. Length < 20, то просто добавляем
// найденное слово
// в таблицу слов и его частоту -- в таблицу частот на первое свободное место,
// т.е. по индексу Length,
// При этом мы обновляем индекс минимального элемента, т.е. если новое слово
// имеет меньшую частоту,
// чем Freqs[MinIndex], то MinIndex = Length. После всего этого увеличивем
// Length на 1 (расширяем таблицу).
// 2. если таблица заполнена, и новое слово имеет бОльшую частоту, чем самое
// редкое из имеющихся в таблице
// (именно для этого мы и храним MinIndex), то нужно заменить это слово и его
// частоту в таблицах,
// после чего заново найти минимальный элемент в таблице частот.
//
// Таким образом, сначала таблица просто заполнится первыми 20-ю словами из
// множества,
// а потом самые редкие слова будут замещаться новыми. В общем, всё не так уж
// сложно,
// остается вывести их в порядке убывания частот. Сортировку делать лень,
// поэтому поступим проще:
// организуем цикл по таблице частот, в нём (с помощью вложенного цикла) находим
// максимальный элемент
// и его индекс MI, печатаем MI-тое слово из таблицы, а затем обнуляем частоту
// MI-того слова (Freqs[MI]=0;).
// Фактически, мы при этом "вычёркиваем" слово из таблицы, и при следующем
// проходе его учитывать не будем.

#include <stdio.h>
//#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "HashSet.h"

static void printHelp();

class Word : public HashSetKey {
  char *str;
  int len;
  int capacity;

public:
  Word();
  Word(const char *s, int l);
  Word(const Word &w);
  virtual ~Word() { delete[] str; }
  virtual Word *clone() const { return new Word(*this); }

  int length() const { return len; }
  int size() const { return len; }
  void initialize();

  // Add a character to the end of word
  Word &operator+=(int c);

  // Convertor to C-string
  operator const char *() { return str; }
  const char *getString() const { return str; }

  virtual bool operator==(const HashSetKey &s) const {
    return (strcmp(str, ((Word &)s).str) == 0);
  }

  virtual int hashValue() const;
};

// Implementation of class Word
Word::Word() : str(0), len(0), capacity(0) {}

Word::Word(const char *s, int l)
    : str(new char(l + 1)), len(l), capacity(len + 1) {
  memmove(str, s, len);
  str[len] = 0;
}

Word::Word(const Word &w) : str(0), len(w.len), capacity(w.capacity) {
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
Word &Word::operator+=(int c) {
  if (capacity <= len + 1) {
    // Allocate additional space
    int extent = capacity / 8;
    if (extent < 16)
      extent = 16;
    char *new_str = new char[capacity + extent];
    memmove(new_str, str, len);
    delete[] str;
    capacity += extent;
    str = new_str;
  }
  str[len] = (char)c;
  ++len;
  str[len] = 0;
  return *this;
}

int Word::hashValue() const {
  const int HASH_FACTOR = 1021; // Prime number

  // Calculate polynomial (...(s0*x + s1)*x + ...)*x + sn, where x = HASH_FACTOR
  int hash = 0;
  for (int i = 0; i < len; i++) {
    hash *= HASH_FACTOR;
    hash += (int)str[i];
  }

  // Make the hash value positive: clear the sign bit
  hash &= 0x7fffffff;
  return hash;
}

// class Integer represents the number of inclusions of word in text
class Integer : public HashSetValue {
public:
  int number;

public:
  Integer() : number(0) {}
  Integer(const Integer &i) : number(i.number) {}
  Integer(int n) : number(n) {}
  virtual ~Integer() {}
  virtual Integer *clone() const { return new Integer(*this); }
};

int main(int argc, char *argv[]) {
  HashSet set(5009); // 5009 is a prime number (the hashtable size)
  FILE *input;
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
  } else
    input = stdin;

  bool wasLetter = false;
  bool endOfFileDetected = false;
  Word CurrentWord;
  while (!endOfFileDetected) {
    int c = fgetc(input);
    if (isalpha(c)) {
      CurrentWord += c;
      wasLetter = true;
    } else {
      if (wasLetter) {
        if (set.contains(&CurrentWord)) {
          Integer *val = (Integer *)set.value(&CurrentWord);
          ++(val->number);
        } else {
          Integer unit(1);
          set.add(&CurrentWord, &unit);
        }
        CurrentWord.initialize();
      }
      wasLetter = false;
      endOfFileDetected = (c == EOF);
    }
  }
  printf("File reading completed.\n");

  // Изменения начинаются с этой строки
  // =================================================
  const int NW = 20; // Количество самых частых слов
  int Freqs[NW];  // Массив частот самых частых слов
  Word Words[NW]; // Массив самих этих слов
  int Length = 0; // Текущая длина массивов Freq и Words
  int MinIndex = 0;

  for (int j = 0; j < NW; j++)
    Freqs[j] = 0; // Инициализация нулями

  HashSet::const_iterator i = set.begin();
  HashSet::const_iterator e = set.end();

  while (i != e) {
    const HashSet::Pair &pair = *i;
    // Вынимаем слово и его частоту из множества
    const Word *w = (const Word *)pair.key;             // слово...
    int number = ((const Integer *)pair.value)->number; // ...и его частота
    if (Length < NW)                                    // Если массив ещё не заполнен, заполняем ешё одну ячейку
    { // Length указывает на первую пустую ячейку
      Words[Length] = *w;
      Freqs[Length] = number;
      // теперь обновим минимальную частоту (она может, конечно, и остаться
      // прежней)
      if (number < Freqs[MinIndex])
        MinIndex = Length;
      Length++; // увеличиваем текущую длину
    } else      //  А если массив заполнен, то "вытесняем" самое редкое слово из уже
                //  найденных,
    {           //  если новое слово встречается чаще, чем самое редкое из имеющихся.
      if (number > Freqs[MinIndex]) {
        Words[MinIndex] = *w;
        Freqs[MinIndex] = number;
      }
      // а теперь надо заново найти самое редкое слово в обновлённом массиве:
      int NewMinIndex = 0;
      int NewMinValue = 2147483647;
      for (int j = 0; j < NW; j++)
        if (Freqs[j] < NewMinValue) {
          NewMinValue = Freqs[j];
          NewMinIndex = j;
        }
      MinIndex = NewMinIndex;
    }
    ++i;
  }
  // Упорядоченный вывод
  for (int j = 0; j < Length; j++) {
    int MI = 0; // индекс максимума
    int M = 0;  // максимум
    for (int k = 0; k < Length; k++)
      if (Freqs[k] > M) {
        MI = k;
        M = Freqs[k];
      } // поиск максимального
    printf("Word: %s \t\t\tfrequency: %d.\n", Words[MI].getString(), Freqs[MI]);
    Freqs[MI] = 0; // вычёркиваем
  }
  //  getch();

  return 0;
}

static void printHelp() {
  printf("Calculate the set of all words in a text,\n"
         "and for every word calculate a number of its inclusions\n"
         "in the text.\n"
         "Usage:\n"
         "    wordfreq [input_file]\n");
}
