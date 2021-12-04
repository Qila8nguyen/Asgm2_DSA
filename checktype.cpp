#include<iostream>

using namespace std;

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
    size_t i = 0;

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
        cout << "arg = "  <<argumi <<endl;
        if (!isNumorString(argumi)) return false;
        temp.erase (0,7);
    }

    return true;
}



int main ()
{
    string a0 = "()->string";
    string a1 = "(number,number,string,string)->string";
    string a2 = "(number)->number";
    string a3 = "(mi)->string";
    string a4 = "(string,string,string,)->string";
    string a5 = "()->number";

    if (checkType(a0)) cout <<"1" <<endl;
    if (checkType(a1)) cout <<"1" <<endl;
    if (checkType(a2)) cout <<"1" <<endl;

    if (checkType(a4)) cout <<"1" <<endl;
    if (checkType(a5)) cout <<"1" <<endl;


    if (checkType(a3)) cout <<"1" <<endl;
    else    cout <<"fakse";
    return 0;
}