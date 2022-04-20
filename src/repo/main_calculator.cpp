// Online C++ compiler to run C++ program online
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

/*
{(1+2) * (4 /10)+ (9 + 7)} / (10 +3)
*/
/*
        empty   1   {   (   +-/*%   )   }
1       y       y   y   y   y       n   n
{       y       n   y   y   y       n   n
(       y       n   y   y   y       n   n
+-/*%   n       y   n   n   n       y   y
)       n       y   n   n   n       y   y --> also need matching (
}       n       y   n   n   n       y   y --> also need matching {

*/
bool validation(string& input)
{
    stack<char> ele;
    stack<char> brackets;
    istringstream is(input);
    input = "";
    string str;
    while(is >> str)
    {
        input += str;
    }
    for(char c : input)
    {
        if(isdigit(c))
        {
            if(!ele.empty() && (ele.top() == ')' || ele.top() == '}'))
                return false;
        }
        else if(c == '(' || c == '{')
        {
            if(!ele.empty() && (isdigit(ele.top()) || ele.top() ==')' || ele.top() == '}'))
                return false;
            brackets.push(c);
        }
        else
        {
            if(!(!ele.empty() && (isdigit(ele.top()) || ele.top() == ')' || ele.top() == '}')))
                return false;
            if(c == ')' || c == '}')
            {
                char cur = c == '}' ? '{' : '(';
                if(cur != brackets.top())
                    return false;
                brackets.pop();
            }
        }
        ele.push(c);
    }
    return true;
}
int calculator(const string& input)
{
    return 0;
}
int main() {
    string input = "{(1+2) * (4 /10) + (9 + 7)} / (10 +3)";
    cout << validation(input) << endl;
    return 0;
}
