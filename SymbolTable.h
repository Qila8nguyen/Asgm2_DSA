#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Node 
{
public:
    Node* next; 
    string identifier;
    string type;
    string checkstatic;
    int level;

    Node *left, *right, *parent;

public:
    Node (string id, string type_,string cstatic, int lev) : 
        next (nullptr),
        identifier (id), type (type_), checkstatic (cstatic),
        left (nullptr), right(nullptr), parent (nullptr)
        {
            if (cstatic == "true")
            level = 0;

            else
            level = lev;
        }

    void copy (Node *another)
    {
        identifier = another->identifier;
        type = another->type;
        level = another->level;
        checkstatic = another->checkstatic;
    }
};

class LlystforOrder
{
    public:
    Node *head;
    int size;

    public:
    LlystforOrder(): head(nullptr), size(0) {};

    ~LlystforOrder()
    {
        Node *cur = head;
        while (head != nullptr)
        {
            head = head->next;
            delete cur;
            cur = head;
        }
        size = 0;
    }

    void    insert (Node* );
    Node *  findFirstElewithLevel (int);
    Node*    seperateDelNode (Node *);
    void    print ();

};

class SplayTree
{
    public:
    Node *root;
    
    public:
    SplayTree (): root(nullptr) {}
    bool    insertNodeSTree (SplayTree &, Node *, int& , int&); 
    bool    insertNodeComp (Node*& , Node *, int& );
    bool    insertNode (Node*& , Node *); 
    Node*   search (Node *&,Node*, string );
    void    deleteRoot ();
    void    printPreOrder (Node *);

};



class SymbolTable
{
public:
    SplayTree SPtree;

    SymbolTable() {}
    void run(string filename);
};

#endif