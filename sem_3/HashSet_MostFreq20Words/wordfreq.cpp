// ����� 20 ����. �������� ������:
// ������ 2 ������� (�������) �� 20 ��������� ��� ���� � �� ������,
// �������������� Words[] � Freqs[].
// ��� ������ ��� ����������: ������� ����� ������ (Length, ������������� = 0),
// � ������_������������_�������� (MinIndex) � ������� ������.
//
// ��� ������: ��� �� ���������, �������� �� ���� ��������� ����� (w) � ���
// ������� (number).
// 1. ���� ������� ��� �� �����������, �.�. Length < 20, �� ������ ���������
// ��������� �����
// � ������� ���� � ��� ������� -- � ������� ������ �� ������ ��������� �����,
// �.�. �� ������� Length,
// ��� ���� �� ��������� ������ ������������ ��������, �.�. ���� ����� �����
// ����� ������� �������,
// ��� Freqs[MinIndex], �� MinIndex = Length. ����� ����� ����� ����������
// Length �� 1 (��������� �������).
// 2. ���� ������� ���������, � ����� ����� ����� ������� �������, ��� �����
// ������ �� ��������� � �������
// (������ ��� ����� �� � ������ MinIndex), �� ����� �������� ��� ����� � ���
// ������� � ��������,
// ����� ���� ������ ����� ����������� ������� � ������� ������.
//
// ����� �������, ������� ������� ������ ���������� ������� 20-� ������� ��
// ���������,
// � ����� ����� ������ ����� ����� ���������� ������. � �����, �� �� ��� ��
// ������,
// �������� ������� �� � ������� �������� ������. ���������� ������ ����,
// ������� �������� �����:
// ���������� ���� �� ������� ������, � �� (� ������� ���������� �����) �������
// ������������ �������
// � ��� ������ MI, �������� MI-��� ����� �� �������, � ����� �������� �������
// MI-���� ����� (Freqs[MI]=0;).
// ����������, �� ��� ���� "�����������" ����� �� �������, � ��� ���������
// ������� ��� ��������� �� �����.

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

  // ��������� ���������� � ���� ������
  // =================================================
  const int NW = 20; // ���������� ����� ������ ����
  int Freqs[NW];  // ������ ������ ����� ������ ����
  Word Words[NW]; // ������ ����� ���� ����
  int Length = 0; // ������� ����� �������� Freq � Words
  int MinIndex = 0;

  for (int j = 0; j < NW; j++)
    Freqs[j] = 0; // ������������� ������

  HashSet::const_iterator i = set.begin();
  HashSet::const_iterator e = set.end();

  while (i != e) {
    const HashSet::Pair &pair = *i;
    // �������� ����� � ��� ������� �� ���������
    const Word *w = (const Word *)pair.key;             // �����...
    int number = ((const Integer *)pair.value)->number; // ...� ��� �������
    if (Length < NW)                                    // ���� ������ ��� �� ��������, ��������� ��� ���� ������
    { // Length ��������� �� ������ ������ ������
      Words[Length] = *w;
      Freqs[Length] = number;
      // ������ ������� ����������� ������� (��� �����, �������, � ��������
      // �������)
      if (number < Freqs[MinIndex])
        MinIndex = Length;
      Length++; // ����������� ������� �����
    } else      //  � ���� ������ ��������, �� "���������" ����� ������ ����� �� ���
                //  ���������,
    {           //  ���� ����� ����� ����������� ����, ��� ����� ������ �� ���������.
      if (number > Freqs[MinIndex]) {
        Words[MinIndex] = *w;
        Freqs[MinIndex] = number;
      }
      // � ������ ���� ������ ����� ����� ������ ����� � ���������� �������:
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
  // ������������� �����
  for (int j = 0; j < Length; j++) {
    int MI = 0; // ������ ���������
    int M = 0;  // ��������
    for (int k = 0; k < Length; k++)
      if (Freqs[k] > M) {
        MI = k;
        M = Freqs[k];
      } // ����� �������������
    printf("Word: %s \t\t\tfrequency: %d.\n", Words[MI].getString(), Freqs[MI]);
    Freqs[MI] = 0; // �����������
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
