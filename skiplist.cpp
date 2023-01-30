#include "skiplist.h"
#include "random.h"
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

using namespace std;

// checks if the value needs to be inserted at a higher level
bool SkipList::shouldInsertAtHigherLevel() const {
  return probability >= Random::random() % 100;
}

// you code goes here

// skiplist ostream
ostream &operator<<(ostream &out, const SkipList &skip) {
  for (int i = skip.levels - 1; i >= 0; i--) {
    out << "[level: " << i + 1 << "] ";
    SNode *curr = skip.head->next[i];
    while (curr != nullptr) {
      out << curr->val << "-->";
      curr = curr->next[i];
    }
    out << "nullptr" << endl;
  }
  return out;
}
// constructor snode
SNode::SNode(int val) : val{val} { next.resize(3); }
// copy constructor snode
SNode::SNode(const SNode &other) {
  val = other.val;
  next = other.next;
}
// constructor skiplist
SkipList::SkipList(int levels, int probability) {
  this->levels = levels;
  this->probability = probability;
  head = new SNode;
  for (int i = 0; i < levels; i++) {
    head->next.push_back(nullptr);
  }
}

// copy constructor skiplist
SkipList::SkipList(const SkipList &other) {
  levels = other.levels;
  probability = other.probability;
  head = new SNode{other.head->val};
  for (int i = 0; i < levels; i++) {
    head->next.push_back(nullptr);
  }
  for (int i = levels - 1; i >= 0; i--) {
    SNode *temp = other.head->next[i];
    SNode *newHead = head;
    while (temp != nullptr) {
      SNode *curr = new SNode{temp->val};
      newHead->next[i] = curr;
      temp = temp->next[i];
      newHead = newHead->next[i];
    }
  }
}
// skiplist destructor
SkipList::~SkipList() {
  SNode *temp = head;
  SNode *next;
  while (temp != nullptr) {
    next = temp->next[0];
    delete temp;
    temp = next;
  }
  head = nullptr;
}
// skiplist getBefore nodes
vector<SNode *> SkipList::getBeforeNodes(int val) const {
  vector<SNode *> prevNode;
  for (int i = 0; i < levels; i++) {
    SNode *start = head;
    while (start != nullptr && start->next[i] != nullptr &&
           start->next[i]->val < val) {
      start = start->next[i];
    }
    prevNode.push_back(start);
  }
  return prevNode;
}
// skiplist add
void SkipList::add(int val) {
  SNode *start = head;
  SNode *insertVal = new SNode{val};
  vector<SNode *> prevNode = getBeforeNodes(val);
  for (int i = 0; i < levels; i++) {
    start = prevNode[i];
    while (start != nullptr) {
      // if (start->val == prevNode[i]->val) {
      insertVal->next[i] = start->next[i];
      start->next[i] = insertVal;
      start = start->next[i]; // at inserted node
      while (i < levels - 1 && shouldInsertAtHigherLevel()) {
        i = i + 1;
        SNode *curr = prevNode[i];
        if (curr->val == prevNode[i]->val && curr->next[i] != nullptr) {
          insertVal->next[i] = curr->next[i];
          curr->next[i] = start;
        } else if (curr->val == prevNode[i]->val && curr->next[i] == nullptr) {
          curr->next[i] = start;
        }
      }
      return;
    }
  }
  delete insertVal;
}
// skiplist add vector
void SkipList::add(const vector<int> &values) {
  for (int i = 0; i < values.size(); i++) {
    add(values[i]);
  }
}
// skiplist remove
bool SkipList::remove(int val) {
  if (!contains(val)) {
    return false;
  }
  SNode *start;
  SNode *removeVal = nullptr;
  vector<SNode *> prevNode = getBeforeNodes(val);
  bool success = false;
  for (int i = levels - 1; i >= 0; i--) {
    start = prevNode[i];
    if (start->val == prevNode[i]->val && start->next[i]->val == val) {
      removeVal = start->next[i];
      start->next[i] = start->next[i]->next[i];
      success = true;
    } else {
      start = start->next[i];
    }
  }
  delete removeVal;
  return success;
}
// skiplist contains
bool SkipList::contains(int val) const {
  for (int i = levels - 1; i >= 0; i--) {
    SNode *start = head;
    start = start->next[i];
    while (start != nullptr && start->val < val) {
      start = start->next[i];
    }
    if (start != nullptr && start->val == val) {
      return true;
    }
  }
  return false;
}