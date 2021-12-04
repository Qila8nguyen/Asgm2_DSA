#include<iostream>
#include<string>

using namespace std;

class Node 
{
public:
    Node* next; 
    string identifier;
    string type;
    string checkstatic;
    int level;

    int comp, splay;

    Node *left, *right;

public:
    Node (string id, string type_,string cstatic, int lev) : 
        next (nullptr),
        identifier (id), type (type_), checkstatic (cstatic),
        left (nullptr), right(nullptr),
        comp (0), splay (0)
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
        // left = another.left;
        // right = another.right;
        comp = another->comp;
        splay = another->splay;
    }
};

int main ()
{
    Node * newnode = new Node ("abc", "number", "true", 2);
    delete newnode;

    string para1 = "";
    string para2 = "";
    if (para1.empty())  cout <<"em";
    return 0;
}