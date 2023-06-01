#include <algorithm>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <sstream>
#include <vector>
#include "huffmanTree.h"
/* used huffman tree from geeksforgeeks as reference
https://www.geeksforgeeks.org/huffman-decoding/ */

struct argsdata
{
  Node *root;
  char *result;
  std::vector<int> pos;
  std::string binaryCode;
  // mutex
  pthread_mutex_t *bsem;
  pthread_mutex_t *parent;
  pthread_cond_t *waitTurn;
  int *turn;
  int threadturn;
};
void *decodeMSG(void *ptr)
{
  argsdata args = *((argsdata *)ptr);
  pthread_mutex_unlock(args.parent);
  Node *curr = args.root;
  std::string s = args.binaryCode;
  for (auto h : s)
  {
    if (h == '0')
    {
      curr = curr->left;
    }
    else
    {
      curr = curr->right;
    }
  }
  for (int i = 0; i < args.pos.size(); i++)
  {
    args.result[args.pos[i]] = curr->data;
  }
  pthread_mutex_lock(args.bsem);
  while (args.threadturn != *(args.turn))
  {
    pthread_cond_wait(args.waitTurn, args.bsem);
  }
  pthread_mutex_unlock(args.bsem);

  std::cout << "Symbol: " << curr->data << ", Frequency: " << curr->freq << ", Code: " << s << std::endl;

  pthread_mutex_lock(args.bsem);
  *(args.turn) = *(args.turn) + 1;
  pthread_cond_broadcast(args.waitTurn);
  pthread_mutex_unlock(args.bsem);

  return NULL;
}

int main(int argc, char *argv[])
{
  std::string num, temp, temp2;
  int charLength = 0;
  int num_of_char;
  std::vector<std::pair<char, int>> v;
  std::cin >> num;
  std::cin.ignore();
  std::stringstream tt(num);
  tt >> num_of_char;

  for (int i = 0; i < num_of_char; i++)
  {
    getline(std::cin, temp);
    std::string line = temp.substr(0, 1);
    int freq = stoi(temp.substr(2));
    char letter;
    for (int i = 0; i < line.length(); i++)
    {
      letter = line[i];
    }
    v.push_back(std::make_pair(letter, freq));
    charLength += freq;
  }
  huffman h;
  Node *fudge = NULL;
  fudge = h.createTree(v); // creates tree

  // h.printCodes(fudge, "");
  // mutex

  pthread_mutex_t parent;
  pthread_mutex_t bin_sem;
  pthread_cond_t waitTurn1 = PTHREAD_COND_INITIALIZER;
  int t = 0;
  argsdata arg;
  pthread_mutex_init(&bin_sem, NULL);
  pthread_mutex_init(&parent, NULL);

  arg.parent = &parent;
  arg.bsem = &bin_sem;
  arg.waitTurn = &waitTurn1;
  arg.turn = &t;
  //
  std::vector<pthread_t> tid(num_of_char);
  std::vector<char> final(charLength);
  // char *final = new char[charLength];
  int threadSize = tid.size();
  // cin.ignore();
  for (int i = 0; i < threadSize; i++)
  {
    pthread_mutex_lock(&parent);
    getline(std::cin, temp2);
    std::string binary;
    std::stringstream tt(temp2);
    tt >> binary;
    int index;
    std::vector<int> pos;
    while (tt >> index)
    {
      pos.push_back(index);
    }
    arg.root = fudge;
    arg.binaryCode = binary;
    arg.pos = pos;
    arg.result = final.data();
    arg.threadturn = i;
    pthread_create(&tid[i], NULL, decodeMSG, &arg);
  }
  for (int i = 0; i < threadSize; i++)
  {
    pthread_join(tid[i], NULL);
  }
  std::cout << "Original message: ";
  for (int i = 0; i < final.size(); i++)
  {
    std::cout << final[i];
  }
  pthread_mutex_destroy(&parent);
  pthread_mutex_destroy(&bin_sem);
  return 0;
}