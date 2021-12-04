#include "SymbolTable.h"

string getFirstSplitString (string line);
string noCommandLine (string line);
string type_checkParameter_Function (string value, SplayTree sptree);

void splayNode (SplayTree &sptree, Node *, int&);

void insertElementToTable (string, SplayTree&, string, int, LlystforOrder &);
void assignExistedElement (string, SplayTree&, string);
void lookupElementInScope (string, SplayTree&, string);
void removeAllNodeInScope (SplayTree&, LlystforOrder&, int);

void SymbolTable::run(string filename)
{
    ifstream inputFile (filename);
    string line;
    int scope = 0;
    LlystforOrder lystorder;

    while ( getline (inputFile,line) )
    {
        // cout <<"------ line = " << line <<endl;
        string command = getFirstSplitString (line);
        string noCommand =  noCommandLine(line);

        if (command == "INSERT") 
            insertElementToTable (noCommand, SPtree, line, scope, lystorder);
        else if (command == "ASSIGN") 
            assignExistedElement (noCommand, SPtree, line); // assign in same scope nếu kh thì kiếm scope giảm
        else if (command == "LOOKUP") 
            lookupElementInScope (noCommand, SPtree, line);
        else if (command == "BEGIN") 
            {
                scope++;
                if (noCommand != "")
                {
                    cout <<noCommand<<endl;
                    throw InvalidInstruction(line);
                }
            }
        else if (command == "END") 
            {
                // cout <<"lyst = " ;lystorder.print(); cout <<endl;
                if (noCommand != "")
                {
                    throw InvalidInstruction(line);
                }

                --scope;
                if (scope < 0)
                {
                    throw UnknownBlock ();
                }
                removeAllNodeInScope (SPtree,lystorder,scope+1);
            }
        else if (command == "PRINT") 
            {
                // SPtree.printDemo(SPtree.root);
                if (SPtree.root != nullptr)
                {
                    SPtree.printPreOrder(SPtree.root);
                    // cout <<endl;
                    // cout <<"lyst size =" << lystorder.size;
                    cout<<endl;
                }

            }
        else
        {
            throw InvalidInstruction (line);
        }

    }

    if (scope > 0)
    {
        throw UnclosedBlock (scope);
    }

}

///////////////////////////////////////////////////
string noCommandLine (string line)
{
    size_t pos = line.find(' ');
    if (pos == string::npos) return "";
    return line.erase(0,pos+1);
}

string getFirstSplitString (string line)
{
    size_t pos = line.find_first_of (' ');

    if (pos == string::npos) return line;

    return line.substr(0,pos);
}

string getMidString (string line)
{
    size_t pos1 = line.find_first_of (' ');
    if (pos1 == string::npos) return "";

    size_t pos2 = line.find_last_of (' ');
    if (pos1 == pos2) return "";

    return line.substr (pos1 + 1, pos2 - pos1 - 1);
}

bool checkID (string id)
{
    if (id == "number" || id == "string" || id == "false" || id == "true")  return false;
    if (id[0] >= 'a' && id[0] <= 'z') 
    {
        int size = id.size();
        for (int i=1;i<size;i++)
        {
            if (id[i] >= 'a' && id[i] <= 'z') continue;
            else if (id[i] >= '0' && id[i] <= '9') continue;
            else if (id[i] >= 'A' && id[i] <= 'Z') continue;
            else if (id[i] == '_') continue;
            else
            return false;
        }
        return true;
    }

    return false;
}

bool isNumorString (string argumi)
{
    if (argumi == "number") return true;
    if (argumi == "string") return true;

    return false;
}

bool checkType (string type)
{
    if (isNumorString (type))   return true;

    int size = type.size();

    size_t openbracket = type.find ('(');
    size_t closebracket = type.find (')');

    if (openbracket == string::npos || closebracket == string::npos)    return false;


    string para = type.substr (1,closebracket-1);
    string arrow = type.substr (closebracket+1,2);
    string restype = type.substr (closebracket+3, size - para.size() - 2 - 2);

    if (!isNumorString(restype)) return false;
    if (arrow != "->")  return false;
    
    //check para        
    if (para.size() == 0 && isNumorString(restype))   return true;

    string temp = para;
    while (!temp.empty())
    {
        size_t comma = temp.find (',');
        if (comma == string::npos)
        {
            if (!isNumorString (temp))  return false;          
            break;
        }

        string argumi = temp.substr(0,comma);
        if (!isNumorString(argumi)) return false;
        temp.erase (0,7);
    }

    return true;
}

bool isNumber (string value)
{
    int size = value.size();
    for (int i = 0; i<size; i++)
    {
        char c = value[i];
        if (c > '9' || c < '0')
        {
            return false;
        }        
    }
    return true;
}

bool isString (string value)
{
    int size = value.size();
    if (value[0] !='\''|| value[size-1] != '\'' ) return false;

    for (int i = 1; i<size-1; i++)
    {
        char c = value[i];

        if (c <= 'Z' && c >='A')    continue;
        else if (c <= 'z' && c >= 'a') continue;
        else if ( c == ' ') continue;
        else if ( c <= '9' && c >= '0') continue;
        else
        {
            return false;
        }
    }
    return true;
}

// return the true type of func
string typeFunc_node (Node *funct, SplayTree sptree)
{

    string type_func = funct->type;

    size_t openbracket = type_func.find ('(');
    size_t closebracket = type_func.find (')');

    if (openbracket == string::npos || closebracket == string::npos)  
    {
        return type_func;
    }

    return type_func.substr(closebracket+3,6); 
}

string checkParameter (string para, SplayTree sptree)
{
    if (isNumber(para))     return "number";
    if (isString (para))    return "string";
    

    if (!checkID(para))   return "INVALID";
    Node * funct = sptree.search(sptree.root, nullptr, para);
    // return typeFunc_node (funct,sptree);
    if (funct == nullptr)   return "UNDECLARED";
    return funct->type;

}

string type_checkParameter_Function (string value, SplayTree sptree)
{
    size_t openbracket = value.find_first_of ('(');
    size_t closebracket = value.find_last_of (')');

    if (openbracket == string::npos && closebracket == string::npos)    return "NOT FUNCTION";

    string function_id = value.substr (0,openbracket);

    if (function_id.size() != 0 && closebracket == value.size()-1 && openbracket == function_id.size())
    {
            Node *funcNode = sptree.search (sptree.root, nullptr, function_id);
            if (funcNode == nullptr)
            {
                return "UNDECLARED";
            }

            string funcNode_type = funcNode->type;
            string para_function = funcNode_type.substr (1,funcNode_type.find (')') - 1 );
            string para_val = value.substr (openbracket+1,closebracket - openbracket - 1);

            //throw mismatch if wrong para
            while (!para_val.empty() || !para_function.empty())
            {
                size_t comma1 = para_val.find_first_of (',');
                size_t comma2 = para_function.find_first_of (',');

                if (comma1 == string::npos && comma2 != string::npos)
                {
                    return "TYPEMISMATCH";
                }
                
                if (comma2 == string::npos && comma1 != string::npos)
                {
                    return "TYPEMISMATCH";
                }

                if (comma1 == string::npos && comma2 == string::npos)
                {
                    string t = checkParameter(para_val,sptree);
                    if (t == "INVALID")
                    {
                        return "INVALID";
                    }
                    if (t == para_function)
                    {
                        para_val.clear();
                        para_function.clear();
                        break;
                    }

                }

                string para_i = para_val.substr(0,comma1);
                string para_func_i = para_function.substr(0,comma2);
                string temp = checkParameter(para_i, sptree);
                if (temp == "INVALID")
                {
                    return "INVALID";
                }
                if (temp == "UNDECLARED")
                {
                    return "UNDECLARED";
                }
                if (temp !=  para_func_i)
                {
                    return "TYPEMISMATCH";
                }

                para_val.erase(0,comma1+1);
                para_function.erase(0,comma2+1);
            }

            if (para_val.empty() && para_function.empty())
            {
                return funcNode_type.substr (funcNode_type.size()-6);
            }
       
    }

    return "INVALID";
}


/////////////// START LINE CHECK ///////////////
void insertElementToTable (string noCommand, SplayTree& SPtree, string line, int scope, LlystforOrder &order)
{
    string id = getFirstSplitString (noCommand);
    string type = getMidString (noCommand);
    string checkstatic = noCommand.substr (id.length() + type.length() + 2);
    
    if (id.length () == 0 || checkID(id) == false || type.length() == 0 || checkType(type) == false ||
                         checkstatic.length() == 0 || (checkstatic != "true" && checkstatic != "false"))
    {
        throw InvalidInstruction (line);
    }

    if (type[0] == '(' && checkstatic == "false" && scope != 0)
    {
        throw InvalidDeclaration (line);
    }    


    Node *newnode = new Node (id,type,checkstatic,scope);
    if (scope != 0)
    {
        order.insert (newnode);
    }

    int comp =0; int splay =0;
    if (!SPtree.insertNodeSTree (SPtree, newnode, comp, splay))
    {
        throw Redeclared (line);
    }

    // SPtree.printDemo(SPtree.root); cout << endl;
    cout << comp <<" "<< splay <<endl;

}

// return num_comp 
void sumCompSplay_inParameter (int& compare, int& splaynum,string value, SplayTree& sptree)
{
    size_t openbracket = value.find_first_of('(');
    size_t closebracket = value.find_first_of(')');

    string func = value.substr(0,openbracket);
    Node *functi = sptree.search(sptree.root,nullptr,func);
    if (functi != sptree.root)  
    {
        splayNode (sptree, functi,compare);
        splaynum++;
    }
    compare++;

    string temp = value.substr(openbracket+1,closebracket - openbracket -1);
    while (!temp.empty())
    {
        size_t comma = temp.find_first_of(',');

        string para_i = temp;
        if (comma != string::npos )
        {
            para_i = temp.substr(0,comma);
        }
        if (!isNumber(para_i) && !isString(para_i))
        {
            Node *parameter_i = sptree.search(sptree.root, nullptr,para_i);
            
            if (parameter_i != sptree.root)
            {
                splayNode (sptree, parameter_i,compare);
                splaynum++;
            }
            ++compare;
        }
        temp.erase(0, para_i.size()+1);
        if (comma == string::npos)  break;
    }

}

void assignExistedElement (string noCommand, SplayTree& SPtree, string line)
{
    string id = getFirstSplitString (noCommand);
    if (id == noCommand)
    {
        throw   InvalidInstruction (line);
    }
    string value = noCommand.substr(id.size()+1);

    if (checkID (id) == false)
    {
        throw InvalidInstruction(line);
    }

    Node *idNode = SPtree.search (SPtree.root, nullptr, id);
    // valueNode  chứa định danh trước || nếu kh tồn tại thì 
    string checkValue_getType = type_checkParameter_Function (value, SPtree);
    if ( checkValue_getType == "NOT FUNCTION")
    {
        if (value.find(' ') != string::npos)
        {
            throw InvalidInstruction(line);
        }
        
        Node *valueNode = SPtree.search (SPtree.root, nullptr, value);
        if (valueNode == nullptr || idNode == nullptr)
        {
            throw Undeclared (line);
        }

        if (valueNode->type.compare (idNode->type) == 0)
        {
            int comp = 0, splay = 0;
            if (SPtree.root != valueNode)
            {
                splayNode (SPtree, valueNode,comp);
                ++splay;
            }
            comp++;
            if (SPtree.root != idNode)
            {
                splayNode (SPtree, idNode,comp);
                ++splay;
            }
            comp++;

            cout << comp <<" " << splay <<endl;
            return;
        }
        else 
        {
            throw TypeMismatch(line);
        }
    }       
    //value là hàm
    else if (checkValue_getType == "TYPEMISMATCH" )
    {
        throw TypeMismatch (line);
    }
    else if (checkValue_getType == "INVALID")
    {
        throw InvalidInstruction(line);
    }
    else if (checkValue_getType == "UNDECLARED")
    {
        throw Undeclared (line);
    }
    else
    {
        if (idNode == nullptr)
        {
            throw   Undeclared (line);
        }
        string typeOfID = idNode->type;
        if (typeOfID != checkValue_getType)
        {
            throw   TypeMismatch(line);
        }    
    }
    int compare = 0;
    int splaynum = 0;

    sumCompSplay_inParameter (compare, splaynum,value, SPtree);

    if (SPtree.root != idNode)
    {
        splayNode (SPtree, idNode,compare);
        splaynum++;
    }
    ++compare;

    cout << compare  <<" "<<splaynum <<endl;   

}

void lookupElementInScope (string noCommand, SplayTree& sptree, string line)
{
    string id = getFirstSplitString (noCommand);
    if (checkID (id) == false) 
    {
        throw InvalidInstruction (line);
    }

    if (noCommand.compare(id) != 0)
    {
        throw InvalidInstruction (line);
    }

    Node * posOfID = sptree.search (sptree.root,nullptr,id);
    if ( posOfID == nullptr)
    {
        throw Undeclared(line);
    }

    cout << posOfID->level <<endl;
    if (sptree.root != posOfID)
    {
        int comp = 0;
        splayNode (sptree,posOfID,comp);
    }

}



void removeAllNodeInScope (SplayTree& sptree, LlystforOrder& order, int scope)
{
    if (sptree.root == nullptr)
    {
        return;
    }

    Node *delnode = order.findFirstElewithLevel (scope);
    Node *nextto = nullptr;
    int comp = 0;
    while (delnode != nullptr)
    {
       nextto = order.seperateDelNode(delnode);
       splayNode(sptree,delnode,comp);
       sptree.deleteRoot ();/////////////////////////
       delnode = nextto;
    }

}



//LINKLIST MEMBER FUNCTIONS TO KEEP BLOCK ORDER
void LlystforOrder::insert (Node* newnode)
{   
    if (this->head == nullptr)
    {
        head = newnode;
        size = 1;
    }

    else if (newnode->checkstatic != "true")
    {
        Node *cur = head;
        while (cur->next != nullptr)
        {
            cur = cur->next;
        }
        cur->next = newnode;
        ++size;
    }
}

Node* LlystforOrder::findFirstElewithLevel (int scope)
{
    if (this->head == nullptr)
    {
        return nullptr;
    }

    Node *cur = head;
    while (cur != nullptr)
    {
        if (cur->level == scope) return cur;
        cur = cur->next;
    }

    return nullptr;
}

Node* LlystforOrder::seperateDelNode (Node *thenode)
{
    Node *pre = head;
    if (thenode == head)
    {
        --size;
        head = head->next;
        return head;
    }
    while (pre->next != thenode)
    {
        pre = pre->next;
    }
    
    pre->next = thenode->next;
    --size;
    return pre->next;
}

void LlystforOrder::print ()
{
    Node *cur = head;
    while (cur != nullptr)
    {
        cout <<"(" <<cur->identifier <<"-" <<cur->level <<")  ";
        cur = cur->next;
    }
}

// SPLAYTREE MEMBER FUNCTIONS START HERE

bool stringhigher (string id1, string id2)
{
    if (id1.compare(id2) == 0) return false;

    int i = 0;
    while (id1[i] != '\0' && id2[i] != '\0')
    {
        if (id1[i] > id2[i])   return true;
        if (id1[i] < id2[i])   return false;
        i++;
    }

    if (id1[i] != '\0') return true;
    
    return false;
}

bool higherthan (Node *a, Node *b)
{
    if (a->level > b->level) return true;
    if (a->level < b->level) return false;

    string id1 = a->identifier;
    string id2 = b->identifier;

    return stringhigher (id1,id2);
}

bool equal (Node *a, Node *b)
{
    if ( higherthan(a,b) || higherthan (b,a) ) return false;
    if ( a->level == b->level || a->identifier.compare (b->identifier) == 0) return true;
    return false;
}

void left_rotate(SplayTree &t, Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if(y->left != NULL) {
    y->left->parent = x;
    }
    y->parent = x->parent;

    if(x->parent == NULL) 
    { //x is root
        t.root = y;
    }
    else if(x == x->parent->left) 
    { //x is left child
        x->parent->left = y;
    }
    else 
    { //x is right child
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void right_rotate(SplayTree &t, Node *x) 
{
    Node *y = x->left;
    x->left = y->right;
    if(y->right != NULL) 
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if(x->parent == NULL) 
    { //x is root
        t.root = y;
    }
    else if(x == x->parent->right) 
    { //x is left child
        x->parent->right = y;
    }
    else 
    { //x is right child
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void splayNode(SplayTree &t, Node *n, int& comp) {
    while(n->parent != NULL) 
    { //Node is not root
        if(n->parent == t.root) 
        { //Node is child of root, one rotation
            ++comp;
            if(n == n->parent->left) 
            {
                right_rotate(t, n->parent);
            }
            else 
            {
                left_rotate(t, n->parent);
            }
        }

        else 
        {
            Node *p = n->parent;
            Node *g = p->parent; //grandparent
            comp+=2;
            if(n->parent->left == n && p->parent->left == p) 
            { //both are left children
                right_rotate(t, g);
                right_rotate(t, p);
            }
            else if(n->parent->right == n && p->parent->right == p) 
            { //both are right children
                left_rotate(t, g);
                left_rotate(t, p);
            }
            else if(n->parent->right == n && p->parent->left == p) 
            {
                left_rotate(t, p);
                right_rotate(t, g);
            }
            else if(n->parent->left == n && p->parent->right == p) 
            {
                right_rotate(t, p);
                left_rotate(t, g);
            }
        }
    }
}

bool SplayTree::insertNode (Node* &root, Node *newnode)
{
    if (root == nullptr)
    {
        root = newnode;
        return true;
    }

    if (root->left && equal (root->left, newnode))  return false;
    if (root->right && equal (root->right, newnode))  return false;

    if (root->left == nullptr && higherthan(root,newnode))
    {
        root->left = newnode;
        newnode->parent = root;
        return true;
    }

    if (root->right == nullptr && higherthan(newnode,root))
    {
        root->right = newnode;
        newnode->parent = root;
        return true;
    }

    if (higherthan(root,newnode))
    {

        return insertNode(root->left,newnode);
    }
    if (higherthan(newnode,root))
    {
        return insertNode (root->right, newnode);
    }

    return false;
}

bool SplayTree::insertNodeSTree (SplayTree &sptree, Node *newnode, int& comp,int &splay)
{
    if (root == nullptr)
    {
        splay = 0;
        root = newnode;
        return true;
    }

   if (!insertNode(root, newnode))  return false;

    splayNode (sptree, newnode, comp);
    splay = 1;

    return true;
}


Node* SplayTree::search (Node* &troot, Node *found, string id)
{
    if (troot == nullptr)
    {
        return nullptr;
    }

    if (troot->identifier.compare(id) == 0)
    {
        if (found == nullptr)
        {
            found = troot;
        }
        else
        {
            if (higherthan(troot, found))   found = troot;
        }
        return found;
    }

    Node *find1 = search (troot->left,found,id);
    Node *find2 = search (troot->right,found,id);

    if (find1 == nullptr && find2 == nullptr)   return nullptr;
    if (find1 && find2 == nullptr)  return find1;
    if (find2 && find1 == nullptr)  return find2;
    return (find2->level > find1->level)?   find2:find1;
        
}


void SplayTree::printPreOrder (Node * troot )
{
    if (troot == nullptr)
    {
        return;
    }
    if (troot == this->root)
    {
        cout <<troot->identifier<<"//" <<troot->level;
    }

    else
    {
    cout <<" " <<troot->identifier<<"//" <<troot->level;
    }

    printPreOrder (troot->left);
    printPreOrder (troot->right);
}

Node *premaxLeft (Node *treeroot)
{
    Node *premax = treeroot->left;
    while (premax->right->right != nullptr)
    {
        premax = premax->right;
    }

    return premax;
}

void SplayTree::deleteRoot ()
{
    if (this->root == nullptr)  return;
    if (this->root->left == nullptr && this->root->right == nullptr)
    {
        delete root;
        root = nullptr;
    }
    else if (this->root->left == nullptr)
    {
        Node *tempR = root->right;
        delete root;
        root = tempR;
        root->parent = nullptr;
    }
    else if (this->root->right == nullptr)
    {
        Node *tempL = root->left;
        delete root;
        root = tempL;
        root->parent = nullptr;
    }
    else
    {
        if (root->left->right == nullptr)
        {
            Node *rightRoot = root->right;
            Node *leftRoot = root->left;
            leftRoot->right = rightRoot;
            rightRoot->parent = leftRoot;
            delete root;
            root = leftRoot;
            root->parent = nullptr;
        }
        else
        {
            Node *preLeft = premaxLeft (this->root);
            Node *max = root->left;
            Node *rightRoot = root->right;
            if (preLeft->right == nullptr)
            {
                max->right = rightRoot;
                rightRoot->parent = max;
            }

            else
            {
                max = preLeft->right;
                Node *leftRoot = root->left;
                max->left = leftRoot;
                max->right = rightRoot;
                leftRoot->parent = max;
                rightRoot->parent = max;
                preLeft->right = nullptr;
            }

            delete root;
            root = max;
            root->parent = nullptr;
        }
    }
}

