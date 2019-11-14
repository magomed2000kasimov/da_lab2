#include <iostream>
#include <map>
#include <chrono>
#include <cstring>
#include <algorithm>

const int MAX_SIZE = 258;

class TPatriciaNode {
public:
    TPatriciaNode(char* newKey, unsigned long long newValue, int newBit);
    unsigned long long GetTheValue();
    ~TPatriciaNode();
    TPatriciaNode();
private:
    char* Key;
    unsigned long long Value;
    int Bit;
    int Index;
    TPatriciaNode* Pointers[2];
    friend class TPatriciaTree;
};

class TPatriciaTree {
public:
    TPatriciaTree();
    TPatriciaNode* Search(char* indKey);
    bool Insert(char* insertKey, unsigned long long insertValue);
    bool Delete(char* deleteKey);
    bool IsEmpty();
    bool Save(std::ofstream*);
    bool Load(std::ifstream*);
    ~TPatriciaTree();
    void Print(TPatriciaNode* node,int i);
private:
    TPatriciaNode* Root;
    TPatriciaNode* SearhForDelete(char* key,TPatriciaNode** backPtr);
    TPatriciaNode* Parent(TPatriciaNode* node);
    size_t Size;
    bool CompareKey(char* key1,char* key2);
    int BitLength(char* key);
    int BitGet(char* key,int bit);
    void DeleteTree(TPatriciaNode* node);
    TPatriciaNode* SearchR(TPatriciaNode *node, char *key, int bit);
    void Index(TPatriciaNode* node, TPatriciaNode** nodes, int* depth);
};

TPatriciaNode::TPatriciaNode() {
    Index = 0;
    Key = nullptr;
    Value = 0;
    Bit = 0;
    Pointers[0] = this;
    Pointers[1] = this;
}

TPatriciaNode::TPatriciaNode(char *newKey, unsigned long long newValue, int newBit) {
    Index = 0;
    Key = newKey;
    Value = newValue;
    Bit = newBit;
    Pointers[0] = this;
    Pointers[1] = this;
}

unsigned long long TPatriciaNode::GetTheValue() {
    return Value;
}

TPatriciaNode::~TPatriciaNode() {
    delete[] Key;
}

TPatriciaTree::TPatriciaTree() {
    Size = 0;
    Root = new TPatriciaNode(nullptr,0,-1);
}

TPatriciaTree::~TPatriciaTree() {
    DeleteTree(Root);
}

void TPatriciaTree::DeleteTree(TPatriciaNode *node) {
    if ( node->Pointers[0]->Bit > node->Bit) {
        DeleteTree(node->Pointers[0]);
    }
    if ( node->Pointers[1]->Bit > node->Bit) {
        DeleteTree(node->Pointers[1]);
    }
    delete node;
}

int TPatriciaTree::BitLength(char *key) {
    if ( key == nullptr ) {
        return 0;
    }
    return (std::strlen(key) * 8);
}

int TPatriciaTree::BitGet(char *key, int bit) {
    int index = bit / 8;
    char tmp = key[index];
    return (tmp >> (7 - bit % 8)) & 1;
}

bool TPatriciaTree::CompareKey(char *key1, char *key2) {
    int len1 = BitLength(key1), len2 = BitLength(key2);
    if (len1 != len2) {
        return false;
    }
    for (int i = 0; i < len1 / 8; ++i) {
        if (key1[i] != key2[i]) {
            return false;
        }
    }
    return true;
}

bool TPatriciaTree::IsEmpty() {
    return Size == 0;
}

void TPatriciaTree::Print(TPatriciaNode *node, int i) {
    if (node->Pointers[1]->Bit > node->Bit) {
        Print(node->Pointers[1], i + 1);
    }
    for (int j = 0; j < i*6; ++j) {
        std::cout << ' ';
    }
    for (int k = 0; k < BitLength(node->Key)/8; ++k) {
        std::cout << node->Key[k];
    }
    std::cout << '\n';
    if (node->Pointers[0]->Bit > node->Bit) {
        Print(node->Pointers[0], i + 1);
    }
}

TPatriciaNode* TPatriciaTree::SearchR(TPatriciaNode *node, char *key, int bit) {
    if ((node ->Bit) <= bit) {
        return node;
    }
    if ( BitGet(key,node -> Bit) == 0) {
        return SearchR(node -> Pointers[0],key,node -> Bit);
    }
    else {
        return SearchR(node -> Pointers[1],key,node -> Bit);
    }
}

TPatriciaNode* TPatriciaTree::Search(char *key) {
    if (this -> IsEmpty()) {
        return nullptr;
    }
    else {
        TPatriciaNode* tmp = SearchR(Root -> Pointers[0], key, -1);
        return ( CompareKey(key,tmp->Key)) ? tmp : nullptr;
    }
}

bool TPatriciaTree::Insert(char *key, unsigned long long insertValue) {
    int i;
    char* key2 = SearchR(Root -> Pointers[0] , key, -1) -> Key;
    if (CompareKey(key,key2)) {
        return false;
    }
    if (key && key2) {
        for (i = 0; BitGet(key, i) == BitGet(key2, i); ++i);
    }
    else {
        i = 0;
    }
    TPatriciaNode* prev = Root;
    TPatriciaNode* next = Root -> Pointers[0];
    while ( prev -> Bit < next -> Bit && next -> Bit < i) {
        prev = next;
        next = prev -> Pointers[BitGet(key,prev -> Bit)];
    }
    char* newKey;
    try {
        newKey = new char[BitLength(key)/8 + 1];
    }
    catch ( const std::bad_alloc & ) {
        std::cout << "ERROR : NO FREE MEMORY" << '\n';
        return false;
    }
    std::strcpy(newKey,key);
    TPatriciaNode* newNode = new TPatriciaNode(newKey,insertValue,i);
    prev -> Pointers[BitGet(newKey, prev -> Bit)] = newNode;
    newNode -> Pointers[BitGet(newKey,i)] = newNode;
    newNode -> Pointers[1 - BitGet(newKey,i)] = next;
    Size++;
    return true;
}

TPatriciaNode* TPatriciaTree::SearhForDelete(char *key, TPatriciaNode** backPtr = nullptr) {
    if (this -> IsEmpty()) {
        return nullptr;
    }
    TPatriciaNode* prev = Root;
    TPatriciaNode* cur = Root->Pointers[0];
    while ( cur->Bit > prev->Bit) {
        prev = cur;
        if ( BitGet(key,cur -> Bit) == 0) {
            cur = cur->Pointers[0];
        }
        else {
            cur = cur->Pointers[1];
        }
    }
    if (backPtr != nullptr) {
        *backPtr = prev;
    }
    return cur;
}

TPatriciaNode* TPatriciaTree::Parent(TPatriciaNode* node) {
    if (this -> IsEmpty()) {
        return nullptr;
    }
    if (node == Root->Pointers[0]) {
        return Root;
    }
    TPatriciaNode* cur = Root->Pointers[0];
    while (cur->Pointers[0] != node && cur->Pointers[1] != node) {
        if (BitGet(node->Key,cur->Bit)) {
            cur = cur->Pointers[1];
        }
        else {
            cur = cur ->Pointers[0];
        }
    }
    return cur;
}

bool TPatriciaTree::Delete(char *key) {
    TPatriciaNode* q;
    TPatriciaNode* p = SearhForDelete(key,&q);
    TPatriciaNode* parent;
    if (!p || !CompareKey(key,p->Key)) {
        return false;
    }
    parent = Parent(p);
    if (p->Pointers[0] == p || p->Pointers[1] == p) {
        TPatriciaNode *alien;
        if (p->Pointers[0] == p) {
            alien = p->Pointers[1];
        }
        else {
            alien = p->Pointers[0];
        }
        if (p == parent->Pointers[0]) {
            parent->Pointers[0] = alien;
        }
        else {
            parent->Pointers[1] = alien;
        }
        --Size;
        delete p;
        return true;
    }
    TPatriciaNode* r = nullptr;
    TPatriciaNode* qParent = Parent(q);
    SearhForDelete(q->Key,&r);
    bool flag = BitGet(r->Key,q->Bit);
    std::swap(p->Key,q->Key);
    std::swap(p->Value,q->Value);
    if ( r->Pointers[1] == q) {
        r->Pointers[1] = p;
    }
    else {
        r->Pointers[0] = p;
    }
    if (q == qParent->Pointers[1]) {
        if(flag) {
            qParent->Pointers[1] = q->Pointers[1];
        }
        else {
            qParent->Pointers[1] = q->Pointers[0];
        }
    }
    else {
        if(flag) {
            qParent->Pointers[0] = q->Pointers[1];
        }
        else {
            qParent->Pointers[0] = q->Pointers[0];
        }
    }
    --Size;
    delete q;
    return true;
}


int main() {
    std::chrono::time_point<std::chrono::system_clock> start,end;
    std::map<std::string, unsigned long long> rb_tree;
    TPatriciaTree trie;
    char input[256];
    std::string in;
    unsigned long long rb_time = 0, trie_time = 0;
    unsigned long long value;
    while (std::cin >> input) {
        std::cin >> value;
	std::string str(input);
        
	{
            start = std::chrono::system_clock::now();
            rb_tree.insert({str, value});
            end = std::chrono::system_clock::now();
            rb_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }
        {
            start = std::chrono::system_clock::now();
            trie.Insert(input,value);
            end = std::chrono::system_clock::now();
            trie_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }
    }
    double res;
    res = (double) rb_time / 1000000;
    std:: cout << "RB insert: " << res << " seconds" << std::endl;
    res = (double) trie_time / 1000000;
    std:: cout << "PATRICIA trie insert: " << res << " seconds" << std::endl;
    char inp[257] = "neecinqihvtxekapvpqctefbxhqquuhibmnmaeuhdofhbiyxxpasvhttqkklhswkfkwhqtptlwbmfcmgrpypwukohwcqrybwjbhbuwuivwvebkkvallaivqpvsgnsjnckudityqpxovbaiwavjcehstdmcrfmhjwcphvoynonkppunsryuvgqsmdugkhnvhploncpbqgnjxjyqdaoygfrukpbwwqsggfxvhpxbvlmuwnnanccvjvrtluqjmmrurr";
    std::string st(inp);
    {
	start = std::chrono::system_clock::now();
	rb_tree.find(st);
	end = std::chrono::system_clock::now();
        rb_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
    {  
	start = std::chrono::system_clock::now();
        trie.Search(inp);
        end = std::chrono::system_clock::now();
        trie_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
    res = (double) rb_time;
    std:: cout << "RB find: " << res << " microseconds" << std::endl;
    res = (double) trie_time;
    std:: cout << "PATRICIA trie find: " << res <<" microseconds" << std::endl;
    return 0;
}

