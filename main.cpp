#include <fstream>
#include <iostream>
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

void TPatriciaTree::Index(TPatriciaNode* node, TPatriciaNode** nodes, int* index) {
    node->Index = *index;
    nodes[*index] = node;
    ++(*index);
    if (node->Pointers[0]->Bit > node->Bit) {
        Index(node->Pointers[0], nodes, index);
    }
    if (node->Pointers[1]->Bit > node->Bit) {
        Index(node->Pointers[1], nodes, index);
    }
}

bool TPatriciaTree::Save(std::ofstream *file) {
    file->write((char*) &(Size), sizeof(int));
    int index = 0;
    TPatriciaNode** nodes = new TPatriciaNode*[this->Size + 1];
    TPatriciaNode* node;
    Index(Root, nodes, &index);
    for (int i = 0; i < (Size + 1); ++i) {
        node = nodes[i];
        file->write((char*) &(node->Value), sizeof(unsigned long long));
        file->write((char*) &(node->Bit), sizeof(int));
        int len = BitLength(node->Key)/8;
        file->write((char*) &(len), sizeof(int));
        file->write(node->Key, len);
        file->write((char*) &(node->Pointers[0]->Index), sizeof(int));
        file->write((char*) &(node->Pointers[1]->Index), sizeof(int));
    }
    delete[] nodes;
    return true;
}

bool TPatriciaTree::Load(std::ifstream* file) {
    int size;
    TPatriciaNode** nodes;
    unsigned long long value;
    int bit;
    int len;
    char* key;
    int indLeft, indRight;
    file->read((char*) &size, sizeof(int));
    this->Size = size;
    if (!size) {
        return true;
    }
    try {
        nodes = new TPatriciaNode*[size + 1];
    } 
    catch (const std::bad_alloc &) {
        return false;
    }
    nodes[0] = Root;
    int j;
    try {
        for (j = 1; j < (size + 1); ++j)
            nodes[j] = new TPatriciaNode();
    } 
    catch (const std::bad_alloc &) {
        std::cout << "ERROR: not free memory\n";
        for (int i = 0; i < j; ++i)
            delete nodes[i];
        delete[] nodes;
        return false;
    }
    try {
        for (int i = 0; i < (size + 1); ++i) {
            file->read((char*) &(value), sizeof(unsigned long long));
            file->read((char*) &(bit), sizeof(int));
            file->read((char*) &(len), sizeof(int));
            key = new char[len + 1];
            key[len] = 0;
            file->read(key, len);
            file->read((char*) &(indLeft), sizeof(int));
            file->read((char*) &(indRight), sizeof(int));
            nodes[i]->Index = 0;
            nodes[i]->Key = key;
            nodes[i]->Value = value;
            nodes[i]->Bit = bit;
            nodes[i]->Pointers[0] = nodes[indLeft];
            nodes[i]->Pointers[1] = nodes[indRight];
        }
    }
    catch (const std::bad_alloc &) {
        std::cout << "ERROR: not free memory\n";
        for (int i = 0; i < this->Size; i++)
            delete nodes[i];
        delete[] nodes;
        return false;
    }
    file->peek();
    if (file->fail() || !file->eof()) {
        for (int i = 0; i < this->Size; i++)
            delete nodes[i];
        delete[] nodes;
        return false;
    }
    delete[] nodes;
    return true;
}

void Lower(char *str) {
    int len = std::strlen(str);
    for (int i = 0; i < len; ++i) {
        str[i] = std::tolower(str[i]);
    }
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::ofstream out;
    std::ifstream in;
    char input[MAX_SIZE];
    unsigned long long value;
    TPatriciaNode* node;
    TPatriciaTree* tree = new TPatriciaTree();
    TPatriciaTree* tree2 = nullptr;
    while ((std::cin >> input)) {
        if (!std::cin) {
            std::cout << "ERROR: input option can not be read " << std::endl;
            std::cin.clear();
            std::cin.ignore();
            continue;
        }
        if (!std::strcmp(input, "+")) {
            std::cin >> input;
            Lower(input);
            std::cin >> value;
            if (!std::cin) {
                std::cout << "ERROR: read error\n";
                std::cin.clear();
                std::cin.ignore();
                continue;
            }
            std::cout << (tree->Insert(input, value) ? "OK" : "Exist");
            std::cout << std::endl;
        }
        else if (!std::strcmp(input, "-")) {
            std::cin >> input;
            Lower(input);
            std::cout << (tree->Delete(input) ? "OK" : "NoSuchWord");
            std::cout << std::endl;
        }
        else if (!std::strcmp(input, "!")) {
            std::cin >> input;
            if (!std::strcmp(input, "Save")) {
                std::cin >> input;
                out.open(input, std::ios::out | std::ios::binary | std::ios::trunc);
                if (!out.is_open()) {
                    std::cout << "ERROR: error create file\n";
                    continue;
                }
                if (tree->Save(&out))
                    std::cout << "OK";
                else
                    std::cout << "ERROR: Could not create dictionary\n";
                std::cout << std::endl;
                out.close();
            }
            else if (!std::strcmp(input, "Load")) {
                std::cin >> input;
                in.open(input, std::ios::in | std::ios::binary);
                if (!in.is_open()) {
                    std::cout << "ERROR: error open file\n";
                    continue;
                }
                tree2 = new TPatriciaTree();
                if (tree2->Load(&in)) {
                    std::cout << "OK" << std::endl;
                    delete tree;
                    tree = tree2;
                }
                else {
                    std::cout << "ERROR: Wrong dictionary format\n";
                    delete tree2;
                }
                in.close();
            }
            else {
                std::cout << "ERROR: read error\n";
                continue;
            }
        }
        else {
            Lower(input);
            node = tree->Search(input);
            if (!node) {
                std::cout << "NoSuchWord";
            }
            else {
                std::cout << "OK: " << node->GetTheValue();
            }
            std::cout << std::endl;
        }
    }
    delete tree;
    return 0;
}