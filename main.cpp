//
//  main.cpp
//  LLVM
//
//  Created by Hasan on 17.04.2021.
//

#include <iostream>
#include <stack>
#include <string.h>
#include <sstream>
#include <istream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <list>
#include <set>

using namespace std;

static string outputName = "";
static int ha_oNumber = 1; //name of the temp
static int chooseNumber = 1; // in order to track choose
static int whileNumber = 1; // in order to track while
static int ifNumber = 1; // in order to track if
static bool isWhile = false;
static int bracketCheck = 0; // to track brackets

// operation tracking
void operators(const string &a, const string &b,int x, ofstream* out){
    string type;
    if(x == 0) type = "add";
    if(x == 1) type = "sub";
    if(x == 2) type = "sdiv";
    if(x == 3) type = "mul";
   
    // evaluation of the inputs in terms of being digit or a variable
    if(isdigit(a[0]) && isdigit(b[0])) {
        *out << "\t%ha_o" << ha_oNumber << " = " << type << " i32 " << a << ", " << b << endl;
        ha_oNumber++;
    }
    else if(isdigit(a[0]) && !isdigit(b[0])){
            
        if(!(b.substr(0,4)== "ha_o")){
            
        *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << b << endl;
        ha_oNumber++;
        
        *out << "\t%ha_o" << ha_oNumber << " = " << type << " i32 " << a << ", %ha_o" << ha_oNumber - 1 << endl;
        ha_oNumber++;
        }
        else{
            *out << "\t%ha_o" << ha_oNumber << " = " << type << " i32 " << a << ", %" << b << endl;
            ha_oNumber++;
        }
    }
    
    else if(!isdigit(a[0]) && isdigit(b[0])){
        
        if(!(a.substr(0,4)== "ha_o")){
        *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << a << endl;
        ha_oNumber++;
        
        *out << "\t%ha_o" << ha_oNumber << " = " << type << " i32 %ha_o" << ha_oNumber - 1 << ", " << b << endl;
        ha_oNumber++;
        }
        else{
            *out << "\t%ha_o" << ha_oNumber << " = " << type << " i32 %" << a << ", " << b << endl;
            ha_oNumber++;
        }
    }
    else if(!isdigit(a[0]) && !isdigit(b[0])){
        
       
        if(!(a.substr(0,4) == "ha_o" )){
            *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << a << endl;
            ha_oNumber++;

        }
        else{
            *out << "\t%ha_o" << ha_oNumber << " = add i32 %" << a << ", 0" << endl;
            ha_oNumber++;
        }
        if(!(b.substr(0,4) == "ha_o" )){
            *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << b << endl;
            ha_oNumber++;
        }
        else{
            *out << "\t%ha_o" << ha_oNumber << " = add i32 %" << b << ", 0" << endl;
            ha_oNumber++;
        }
        
        *out << "\t%ha_o" << ha_oNumber << " = " << type << " i32 %ha_o" << ha_oNumber - 2 << ", %ha_o" << ha_oNumber - 1  << endl;
        ha_oNumber++;
    }
}
// necesseary llvm intermediate code are constitued
void operators(const string &a, const string &b, const string &c, const string &d, ofstream* out) {
    
    *out << "\t%choose" << chooseNumber << " = alloca i32" << endl;
    *out << "\tstore i32 0, i32* %choose" << chooseNumber << endl;

    *out << "\tbr label %choose" << chooseNumber << "cond1" << endl << endl << "choose" << chooseNumber << "cond1:" << endl;
    if(!isdigit(d[0])) {
        if(!(d.substr(0,4)== "ha_o")){
        *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << d << endl;
        ha_oNumber++;
        
        *out << "\t%ha_o" << ha_oNumber << " = icmp eq i32 %ha_o" << ha_oNumber - 1 << ", 0" << endl;
        ha_oNumber++;
        }
        else{
            *out << "\t%ha_o" << ha_oNumber << " = icmp eq i32 %" << d << ", 0" << endl;
            ha_oNumber++;
        }
        *out << "\tbr i1 %ha_o" << ha_oNumber - 1 << ", label %choose" << chooseNumber << "body1, label %choose" << chooseNumber << "cond2" << endl << endl;
        *out << "choose" << chooseNumber << "cond2:" << endl;
        if(!(d.substr(0,4)== "ha_o")){
        *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << d  << endl;
        ha_oNumber++;
        
        *out << "\t%ha_o" << ha_oNumber << " = icmp sgt i32 %ha_o" << ha_oNumber - 1 << ", 0" << endl;
        ha_oNumber++;
        }
        else{
            *out << "\t%ha_o" << ha_oNumber << " = icmp sgt i32 %" << d << ", 0" << endl;
            ha_oNumber++;
        }
        *out << "\tbr i1 %ha_o" << ha_oNumber - 1 << ", label %choose" << chooseNumber << "body2, label %choose" << chooseNumber << "body3" << endl << endl;
    }
    else{
        *out << "\t%ha_o" << ha_oNumber << " = icmp eq i32 " << d << ", 0" << endl;
        ha_oNumber++;
        *out << "\tbr i1 %ha_o" << ha_oNumber - 1 << ", label %choose" << chooseNumber << "body1, label %choose" << chooseNumber << "cond2" << endl << endl;
        *out << "choose" << chooseNumber << "cond2:" << endl;
        *out << "\t%ha_o" << ha_oNumber << " = icmp sgt i32 " << d << ", 0" << endl;
        ha_oNumber++;
        *out << "\tbr i1 %ha_o" << ha_oNumber - 1 << ", label %choose" << chooseNumber << "body2, label %choose" << chooseNumber << "body3" << endl << endl;
    }

    *out << "choose" << chooseNumber << "body1:" << endl;
    if(!isdigit(c[0])){
        if(!(c.substr(0,4)== "ha_o")){
        *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << c << endl;
        ha_oNumber++;
            *out << "\tstore i32 %ha_o" << ha_oNumber - 1 << ", i32* %choose" << chooseNumber << endl;
        }
        else{
            *out << "\tstore i32 %" << c << ", i32* %choose" << chooseNumber << endl;
        }
    }
    else{
        *out << "\t%ha_o" << ha_oNumber << " = add i32 " << c << ", 0" << endl;
        ha_oNumber++;
    
    *out << "\tstore i32 %ha_o" << ha_oNumber-1<< ", i32* %choose" << chooseNumber << endl;
    }
    *out << "\tbr label %choose" << chooseNumber << "end" << endl << endl;
    *out << "choose" << chooseNumber << "body2:" << endl;
    if(!isdigit(b[0])){
        if(!(b.substr(0,4)== "ha_o")){
        *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << b << endl;
        ha_oNumber++;
            *out << "\tstore i32 %ha_o" << ha_oNumber - 1 << ", i32* %choose" << chooseNumber << endl;
        }
        else{
            *out << "\tstore i32 %" << b << ", i32* %choose" << chooseNumber << endl;
        }
    }
    else{
        *out << "\t%ha_o" << ha_oNumber << " = add i32 " << b << ", 0" << endl;
        ha_oNumber++;
    
    *out << "\tstore i32 %ha_o" << ha_oNumber-1 << ", i32* %choose" << chooseNumber << endl;
    }
    *out << "\tbr label %choose" << chooseNumber << "end" << endl << endl;
    *out << "choose" << chooseNumber << "body3:" << endl;
    if(!isdigit(a[0])){
        if(!(a.substr(0,4)== "ha_o")){
        *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << a << endl;
        ha_oNumber++;
            *out << "\tstore i32 %ha_o" << ha_oNumber - 1 << ", i32* %choose" << chooseNumber << endl;
        }
        else{
            *out << "\tstore i32 %" <<a << ", i32* %choose" << chooseNumber << endl;
        }
    }
    else{
        *out << "\t%ha_o" << ha_oNumber << " = add i32 " << a << ", 0" << endl;
        ha_oNumber++;
    
    *out << "\tstore i32 %ha_o" << ha_oNumber-1 << ", i32* %choose" << chooseNumber << endl;
    }
    *out << "\tbr label %choose" << chooseNumber << "end" << endl << endl;
    *out << "choose" << chooseNumber << "end:" << endl;

    *out << "\t%ha_o" << ha_oNumber << " = load i32* %choose" << chooseNumber << endl;
    ha_oNumber++;
    chooseNumber++;
}
// used for turning infix to postfix so that we can process operations in a quiet decent way
int prec(char c)
{
    
    if (c == '*' || c == '/')
        return 2;
    else if (c == '+' || c == '-')
        return 1;
    else
        return -1;
}
// The main function to convert infix expression
//to postfix expression
string infixToPostfix(const string& s)
{
    std::stack<char> st;
    st.push('N');
    long l = s.length();

    string ns;
    for (int i = 0; i < l; i++)
    {
        if (s.substr(i, 7) == "choose(")
        {
            int Begin = i + 7;
            int Close = i + 6;
            int strNum = 0;
            int counter = 1;
            string exp1 = "";
            string exp2 = "";
            string exp3 = "";
            string exp4 = "";
            string strarray[4];

            while (counter > 0)
            {
                char c = s[++Close];
                if (c == '(')
                {
                    counter++;
                }
                else if (c == ')')
                {
                    counter--;
                    if (counter == 0)
                    {
                        exp1 = s.substr(Begin, Close - Begin);
                        i = Close;
                        ns += infixToPostfix(exp1);
                        ns += " choose ";
                        strarray[3] = exp1;
                        strNum = 0;
                    }
                }
                else if (c == ',' && counter == 1)
                {

                    exp1 = s.substr(Begin, Close - Begin);
                    Begin = Close + 1;
                    strarray[strNum] = exp1;
                    ns += infixToPostfix(exp1);
                    strNum++;
                }
            }
        }

        bool InHere = false;
        while (s[i] != '+' && s[i] != '/' && s[i] != '-' && s[i] != '*' && s[i] != ')' && s[i] != '(' && s[i] != ' ' && s[i] != '^')
        {
            ns += s[i];
            i++;
            if (i == l)
            {

                ns += ' ';
                InHere = false;
                break;
            }
            InHere = true;
        }
        if (InHere)
        {

            ns += ' ';
        }

        if (s[i] == '(')

            st.push('(');


        else if (s[i] == ')')
        {
            while (st.top() != 'N' && st.top() != '(')
            {

                char c = st.top();

                st.pop();

                ns += c;

                ns += ' ';
            }
            if (st.top() == '(')
            {
                //char c = st.top();
                st.pop();
            }
        }
            // (5 + 2) *3                     ns 5 2 + 3 *          st N *
        else if (s[i] == ' ')
        {

            continue;
        }
            //If an operator is scanned
        else
        {
            while (st.top() != 'N' && prec(s[i]) <=
                                      prec(st.top()))
            {
                char c = st.top();
                st.pop();

                ns += c;

                ns += ' ';
            }
            st.push(s[i]);
        }
    }

    // Pop all the remaining elements from the stack
    while (st.top() != 'N')
    {
        char c = st.top();

        st.pop();

        ns += c;

        ns += ' ';
    }


    return ns;
}
// we obtain the globally accesiable variables
void expressionChecker(string expr, vector<string>* vector){

    list<string> lst;
    stringstream iss(infixToPostfix(expr));
    while (iss >> expr){
        if((int)expr[0]){
            lst.push_back(expr);
        }
    }

    for(auto a: lst) {
        if(a != "+" && a != "-" && a != "*" && a != "/" && a != "choose" && !isdigit(a[0])) {
            bool isContain = false;
            for(int i = 0; i < vector->size(); i++) {
                if(vector->at(i) == a) {
                    isContain = true;
                }
            }
            if(!isContain) vector->push_back(a);
        }
    }
}

// implement the necessary operations and manipulate the postfix notation for further operations
void ha_oCalculator(string exp, ofstream* out,string variable){
    //string exp = "a + b * c * choose(choose(k, l, m + q * w, n), x, y, z)";
    //string exp = "choose(a,b,c,d)";
    //string exp = "a+b + c * choose(k,l,m,n)";
    string word;
    list<string> lst;
    //*out << infixToPostfix(exp) << endl;
    stringstream iss(infixToPostfix(exp));
    while (iss >> word)
    {
        if ((int)word[0])
        {
            lst.push_back(word);
        }
    }

    list<string>::iterator it;
    list<string>::iterator it2;
    it = lst.begin();
    for (; it != lst.end(); it++)
    {
        if (*it == "*")
        {

            it--;
            string a = *it;
            it--;
            string b = *it;
            it2 = it;
            it++;
            it++;
            lst.erase(it2,++it);
            operators(b, a, 3 , out);
            lst.insert(it, "ha_o" + to_string(ha_oNumber - 1));
            it--;
        }

        else if(*it == "+") {

            it--;
            string a = *it;
            it--;
            string b = *it;
            it2 = it;
            it++;
            it++;
            lst.erase(it2,++it);
            operators(b, a, 0 , out);
            lst.insert(it, "ha_o" + to_string(ha_oNumber - 1));
            it--;

        }

        else if(*it == "-") {
            it--;
            string a = *it;
            it--;
            string b = *it;
            it2 = it;
            it++;
            it++;
            lst.erase(it2,++it);
            operators(b, a, 1 , out);
            lst.insert(it, "ha_o" + to_string(ha_oNumber - 1));
            it--;
        }

        else if(*it == "/"){
            it--;
            string a = *it;
            it--;
            string b = *it;
            it2 = it;
            it++;
            it++;
            lst.erase(it2,++it);
            operators(b, a, 2, out);
            lst.insert(it, "ha_o" + to_string(ha_oNumber - 1));
            it--;
        }

        else if(*it == "choose"){
            it--;
            string a = *it;
            it--;
            string b = *it;
            it--;
            string c = *it;
            it--;
            string d = *it;
            it2 = it;
            it++; it++; it++; it++;
            lst.erase(it2, ++it);
            operators(a, b, c, d, out);
            lst.insert(it, "ha_o" + to_string(ha_oNumber - 1));
            it--;
        }

        if(lst.size()==1 && variable != "print" && variable != "ifcondit" ){
            
            if(!isdigit(lst.front()[0])){
                if(lst.front().substr(0,4)== "ha_o"){
                
                *out << "\tstore i32 %" << lst.front() << ", i32* %" << variable << endl ;
                break;
                }
                else{
                    *out << "\t%ha_o" << ha_oNumber << " = load i32* %" << lst.front() << endl;
                    ha_oNumber++;
                    *out << "\tstore i32 %ha_o" << ha_oNumber-1 << ", i32* %"<< variable << endl ;
                    break;
                }
            }
            else{
                *out << "\tstore i32 " << lst.front() << ", i32* %" << variable << endl ;
                break;
            }

        }
    }
}

// to check wheter a variable is apporopriate to declare
bool varCheck(string var) {
    if(isdigit(var[0])) return false;
    else if(var == "if" || var == "while" || var.find("+") != -1 || var.find("*") != -1
    || var.find("-") != -1 || var.find("/") != -1 || var.find("choose(") != -1 || var.find("(") != -1 || var.find(")") != -1) return false;
    else return true;
}
// to print error to the Intermediate code
void syntaxError(int i) {
    ofstream output(outputName);
    output << "; ModuleID = 'mylang2ir'\n"
              "declare i32 @printf(i8*, ...)\n"
              "@print.str = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\"" << endl << endl;

    output << "define i32 @main() {" << endl;
    output << "\tcall i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @print.str, i32 0, i32 0), i32 " << i << " )"
    << endl;
    output << "\tret i32 0" << endl << "}";

}
// to check whether an expression is suitable to proccess else throw syntax error
static bool exprCheck(string expr) {
    stringstream s1(expr);
    string currentEl;
    list<string> elements;
    while(s1 >> currentEl) {
       if ((int) currentEl[0] != 0) elements.push_back(currentEl);
    }
    if(elements.size() == 1){
        if(isdigit(elements.front()[0])){
            for(int i = 0; i < elements.front().length(); i++) {
                if(!isdigit(elements.front().at(i))){
                    return false;
                }
                else return true;
            }
        }
        else if(!varCheck(elements.front())) return false;
        else return true;
    }

    list<string>::iterator it = elements.begin();
    list<string>::iterator it2 = elements.begin();
    for (; it != elements.end(); it++){
        if(*it == "+" || *it == "-" || *it == "*" || *it == "/") {
            if(it == elements.begin()) return false;
            it--;
            string a = *it;
            if(it == elements.begin()) return false;
            it--;
            string b = *it;
            it2 = it;
            if(a == "checked");
            else if(!isdigit(a[0]) && !varCheck(a)) return false;
            else if(a.find("+") != -1 || a.find("-") != -1 || a.find("*") != -1 || a.find("/") != -1) return false;
            else if(isdigit(a[0])) {
                for(int i = 0; i < a.length(); i++) {
                    if(!isdigit(a.at(i))) return false;
                }
            }
            if(b == "checked");
            else if(!isdigit(b[0]) && !varCheck(b)) return false;
            else if(b.find("+") != -1 || b.find("-") != -1 || b.find("*") != -1 || b.find("/") != -1) return false;
            else if(isdigit(b[0])) {
                for(int i = 0; i < b.length(); i++) {
                    if(!isdigit(b.at(i))) return false;
                }
            }
            it++;
            it++;
            elements.erase(it2,++it);
            elements.insert(it, "checked");
            it--;
        }
        if(*it == "choose"){
            if(it == elements.begin()) return false;
            it--;
            string a = *it;
            if(it == elements.begin()) return false;
            it--;
            string b = *it;
            if(it == elements.begin()) return false;
            it--;
            string c = *it;
            if(it == elements.begin()) return false;
            it--;
            string d = *it;
            it2 = it;
            if(a == "checked");
            else if(!isdigit(a[0]) && !varCheck(a)) return false;
            else if(a.find("+") != -1 || a.find("-") != -1 || a.find("*") != -1 || a.find("/") != -1) return false;
            else if(isdigit(a[0])) {
                for(int i = 0; i < a.length(); i++) {
                    if(!isdigit(a.at(i))) return false;
                }
            }
            if(b == "checked");
            else if(!isdigit(b[0]) && !varCheck(b)) return false;
            else if(b.find("+") != -1 || b.find("-") != -1 || b.find("*") != -1 || b.find("/") != -1) return false;
            else if(isdigit(b[0])) {
                for(int i = 0; i < b.length(); i++) {
                    if(!isdigit(b.at(i))) return false;
                }
            }
            if(c == "checked");
            else if(!isdigit(c[0]) && !varCheck(c)) return false;
            else if(c.find("+") != -1 || c.find("-") != -1 || c.find("*") != -1 || c.find("/") != -1) return false;
            else if(isdigit(c[0])) {
                for(int i = 0; i < c.length(); i++) {
                    if(!isdigit(c.at(i))) return false;
                }
            }
            if(d == "checked");
            else if(!isdigit(d[0]) && !varCheck(d)) return false;
            else if(d.find("+") != -1 || d.find("-") != -1 || d.find("*") != -1 || d.find("/") != -1) return false;
            else if(isdigit(d[0])) {
                for(int i = 0; i < d.length(); i++) {
                    if(!isdigit(d.at(i))) return false;
                }
            }
            it++; it++; it++; it++;
            elements.erase(it2, ++it);
            elements.insert(it, "checked");
            it--;
        }
    }
    if(elements.size() == 1 && elements.front().find("checked") != -1) return true;
    else return false;
}
// to check whether the parantehesis match
bool paranthesisCheck(string exp) {
    int open = 0;
    for(int i = 0; i < exp.length(); i++){
        if(exp.at(i) == '(') open++;
        if(exp.at(i) == ')') open--;
        if(open < 0) return false;
    }
    return true;
}


int main(int argc, const char *argv[])
{
    outputName = argv[1];
    outputName = outputName.substr(0, outputName.length() - 2) + "ll";
    bool syntax = true;
    ifstream syntaxInput(argv[1]);
    string syntaxLiner;
    vector<string> syntaxLines;
    
    // in this while, we try to detect syntax errors.
    while(syntaxInput.peek() != EOF) {
        getline(syntaxInput, syntaxLiner);
        if(syntaxLiner.find("#") != -1) {
            syntaxLiner = syntaxLiner.substr(0, syntaxLiner.find("#"));
        }
        syntaxLines.push_back(syntaxLiner);
    }

    for(int i = 0; i < syntaxLines.size(); i++) {
        string currentLine = syntaxLines.at(i);
        stringstream s1(currentLine);
        if(currentLine.find("}") != -1) {
            stringstream s7(currentLine);
            string x;
            list<string> bracketer;
            while(s7 >> x) {
                if((int) x[0]) bracketer.push_back(x);
            }
            if(bracketer.size() != 1){
                syntax = false;
                syntaxError(i);
                break;
            }
            else{
                bracketCheck--;
                if (bracketCheck < 0) {
                    syntax = false;
                    syntaxError(i);
                    break;
                }
            }
        }
        else if(currentLine.find("=") != -1) {
            string var = currentLine.substr(0, currentLine.find("="));
            string expr = currentLine.substr(currentLine.find("=") + 1);
            stringstream s1(var);
            s1 >> var;
            if(!varCheck(var)){
                syntax = false;
                syntaxError(i);
                break;
            };
            if(paranthesisCheck(expr)) {
                if (!exprCheck(infixToPostfix(expr))) {
                    syntax = false;
                    syntaxError(i);
                    break;
                }
            }
            else{
                syntax = false;
                syntaxError(i);
                break;
            }
        }

        else if(currentLine.find("print") != -1) {
            string expr = currentLine.substr(currentLine.find("print(") + 6,
                                             currentLine.find_last_of(")") - 6 - currentLine.find("print"));
            if (expr.find("+") != -1 || expr.find("-") != -1 || expr.find("*") != -1 || expr.find("/") != -1 || expr.find("choose(") != -1) {
                if (paranthesisCheck(expr)) {
                    if (!exprCheck(infixToPostfix(expr))) {
                        syntax = false;
                        syntaxError(i);
                        break;
                    }
                } else {
                    syntax = false;
                    syntaxError(i);
                    break;
                }
            }
            else{
                
                int index0;
                int index1;
                if(expr.find('(' )!= -1) {
                    index0= expr.find_last_of("(");
                    index1 = expr.find(")");
                    expr = expr.substr(index0+1,index1-index0-1);
                    
                    
                    
                }
                if(isdigit(expr[0])) {
                    for(int i = 0; i < expr.length(); i++) {
                        if(!isdigit(expr.at(i))){
                            syntax = false;
                            syntaxError(i);
                            break;
                        }
                    }
                }
                else{
                    if(!varCheck(expr)){
                        syntax = false;
                        syntaxError(i);
                        break;
                    }
                }
            }
        }
        else if(currentLine.find("if") != -1 || currentLine.find("while") != -1){
            int index = 0;
            int length = 5;
            int opening = currentLine.find("(");
            int closing = currentLine.find_last_of(")");
            int bracket = currentLine.find("{");;

            if(currentLine.find("if") != -1) {
                index = currentLine.find("if");
                length = 2;
            }
            else index = currentLine.find("while");
            if(index + length > opening || opening > closing || closing > bracket) {
                syntax = false;
                syntaxError(i);
                break;
            }

            string expr = currentLine.substr(opening + 1, closing - opening - 1);
            if (expr.find("+") != -1 || expr.find("-") != -1 || expr.find("*") != -1 || expr.find("/") != -1 || expr.find("choose(") != -1) {
                if (!paranthesisCheck(expr)) {
                    syntax = false;
                    syntaxError(i);
                    break;
                } else {
                    if (!exprCheck(infixToPostfix(expr))) {
                        
                        syntax = false;
                        syntaxError(i);
                        break;
                    }
                }
            }
            else{
                int index0;
                int index1;
                if(expr.find('(' )!= -1) {
                    index0= expr.find_last_of("(");
                    index1 = expr.find(")");
                    expr = expr.substr(index0+1,index1-index0-1);
                    
                    
                }
                if(isdigit(expr[0])) {
                    for(int i = 0; i < expr.length(); i++) {
                        if(!isdigit(expr.at(i))){
                            syntax = false;
                            syntaxError(i);
                            break;
                        }
                    }
                }
                else{
                    if(!varCheck(expr)){
                        syntax = false;
                        syntaxError(i);
                        break;
                    }
                }
            }
            bracketCheck++;
            if(bracketCheck > 1) {
                syntax = false;
                syntaxError(i);
                break;
            }
        }
        else {
            stringstream s189(currentLine);
            string q;
            s189 >> q;
            if((int)q[0]) {
                syntax = false;
                syntaxError(i);
                break;
            }
        }
        if(i == syntaxLines.size() - 1) {
            if(bracketCheck == 1) {
                syntaxError(i);
                syntax = false;
            }
        }
    }

    // if there is not a syntax error, we procceed to the next while loop
    if(syntax) {
        vector<string> realReal;

        ifstream input(argv[1]);
        string x;
        ofstream output(outputName);
// in this while we allocate memory for all globally accessiable variables
        while (input.peek() != EOF) {
            getline(input, x);
            int indexOfComment = x.find("#");
            if (indexOfComment != -1) {
                x = x.substr(0, indexOfComment);
            }
            stringstream s(x);
            int index = x.find("=");
            if (index != -1) {
                string var = x.substr(0, index);
                stringstream s1(var);
                while (s1 >> var) {
                    if ((int) var[0]) {
                        bool isContain = false;
                        for (int i = 0; i < realReal.size(); i++) {
                            if (realReal.at(i) == var) {
                                isContain = true;
                            }
                        }
                        if (!isContain) realReal.push_back(var);
                    }
                }
                string expr = x.substr(index + 1);
                expressionChecker(expr, &realReal);
            }

            

          else  if (x.find("while") != -1) {
                int indexOfWhile = x.find("while");
                int last = x.find_last_of(")");
                int first = x.find("(");
                string expr = x.substr(first + 1, last - first - 1);
                expressionChecker(expr, &realReal);
            }

            

          else  if (x.find("if") != -1) {
               int indexOfIf = x.find("if");
                int last = x.find_last_of(")");
                int first = x.find("(");
                string expr = x.substr(first + 1, last - first - 1);
                expressionChecker(expr, &realReal);
            }

            

           else if (x.find("print") != -1) {
                int indexOfPrint = x.find("print");
                int last = x.find_last_of(")");
                int first = x.find("(");
                string expr = x.substr(first+1, last-first-1);
                
                expressionChecker(expr, &realReal);
            }
        }

        output << "; ModuleID = 'mylang2ir'\n"
                  "declare i32 @printf(i8*, ...)\n"
                  "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl << endl;

        output << "define i32 @main() {" << endl;


        for (auto a : realReal) {
            output << "\t" << "%" << a << " = alloca i32" << endl;
        }
        output << endl;

        for (auto a : realReal) {
            output << "\tstore i32 0, i32* %" << a << endl;
        }
        output << endl;

        ifstream input2(argv[1]);

        string line;
       
        // in this while loop we process the file and turn it into an intermediate code with the help of operations function
        while (input2.peek() != EOF) {
            getline(input2, line);
            int indexOfComment = line.find("#");
            if (indexOfComment != -1) {
                line = line.substr(0, indexOfComment);

            }
            stringstream s2(line);
            int closed = line.find("}");

            if (closed != -1) {
                if (!isWhile) {
                    output << "\tbr label %ifend" << ifNumber << endl;
                    output << "ifend" << ifNumber << ":" << endl;
                    ifNumber++;
                } else {
                    output << "\tbr label %whcond" << whileNumber << endl;
                    output << "whend" << whileNumber << ":" << endl;
                    whileNumber++;
                }
            }
            
         else   if (line.find("=") != -1) {
                int index = line.find("=");
                string var = line.substr(0, index);

                stringstream s2(var);
                // string var1=var;
                while (s2 >> var) {
                    if ((int) var[0]) {


                        ha_oCalculator(line.substr(index + 1), &output, var);
                        break;
                    }

                }
            }

            


          else  if (line.find("if") != -1) {
               int indexOfIf = line.find("if");
                isWhile = false;
                int last = line.find_last_of(")");
                string expr = line.substr(indexOfIf + 3, last - 3 - indexOfIf);


                if (expr.find("+") != -1 || expr.find("-") != -1 || expr.find("*") != -1 || expr.find("/") != -1 ||
                    expr.find("choose") != -1) {
                    output << "\tbr label %ifcond" << ifNumber << endl << endl;
                    output << "ifcond" << ifNumber << ":" << endl;
                    ha_oCalculator(expr, &output, "ifcondit");
                    output << "\t%ha_o" << ha_oNumber << " = icmp ne i32 %ha_o" << ha_oNumber - 1 << ", 0" << endl;
                    output << "\tbr i1 %ha_o" << ha_oNumber << ", label %ifbody" << ifNumber << ", label %ifend"
                           << ifNumber << endl;
                    output << "ifbody" << ifNumber << ":" << endl;
                    ha_oNumber++;
                } else {

                    if (expr.find("(") != 1) {
                        int first = expr.find_last_of("(");
                        int second = expr.find_last_of(")");
                        expr = expr.substr(first + 1, second - first - 1);
                    }
                    if (isdigit(expr[0])) {
                        output << "\tbr label %ifcond" << ifNumber << endl << endl;
                        output << "ifcond" << ifNumber << ":" << endl;

                        output << "\t%ha_o" << ha_oNumber << " = icmp ne i32 " << expr << ", 0" << endl;
                        output << "\tbr i1 %ha_o" << ha_oNumber << ", label %ifbody" << ifNumber << ", label %ifend"
                               << ifNumber << endl;
                        output << "ifbody" << ifNumber << ":" << endl;
                        ha_oNumber++;
                    } else {
                        output << "\tbr label %ifcond" << ifNumber << endl << endl;
                        output << "ifcond" << ifNumber << ":" << endl;
                        output << "\t%ha_o" << ha_oNumber << " = load i32* %" << expr << endl;
                        ha_oNumber++;
                        output << "\t%ha_o" << ha_oNumber << " = icmp ne i32 %ha_o" << ha_oNumber - 1 << ", 0" << endl;
                        output << "\tbr i1 %ha_o" << ha_oNumber << ", label %ifbody" << ifNumber << ", label %ifend"
                               << ifNumber << endl;
                        output << "ifbody" << ifNumber << ":" << endl;
                        ha_oNumber++;
                    }
                }
            }

            

          else  if (line.find("while") != -1) {
                int indexOfWhile = line.find("while");
                isWhile = true;
                int last = line.find_last_of(")");
                int first = line.find("(");
                string expr = line.substr(first + 1, last - first - 1);

                if (expr.find("+") != -1 || expr.find("-") != -1 || expr.find("*") != -1 || expr.find("/") != -1 ||
                    expr.find("choose") != -1) {
                    output << "\tbr label %whcond" << whileNumber << endl << endl;
                    output << "whcond" << whileNumber << ":" << endl;
                    ha_oCalculator(expr, &output, "ifcondit");
                    output << "\t%ha_o" << ha_oNumber << " = icmp ne i32 %ha_o" << ha_oNumber - 1 << ", 0" << endl;
                    output << "\tbr i1 %ha_o" << ha_oNumber << ", label %whbody" << whileNumber << ", label %whend"
                           << whileNumber << endl;
                    output << "whbody" << whileNumber << ":" << endl;
                    ha_oNumber++;
                } else {

                    if (isdigit(expr[0])) {


                        output << "\tbr label %whcond" << whileNumber << endl << endl;
                        output << "whcond" << whileNumber << ":" << endl;
                        output << "\t%ha_o" << ha_oNumber << " = icmp ne i32 " << expr << ", 0" << endl;
                        output << "\tbr i1 %ha_o" << ha_oNumber << ", label %whbody" << whileNumber << ", label %whend"
                               << whileNumber << endl;
                        output << "whbody" << whileNumber << ":" << endl;
                        ha_oNumber++;
                    } else {
                        int first = expr.find_last_of("(");
                        int second = expr.find_last_of(")");
                        expr = expr.substr(first + 1, second - first - 1);

                        output << "\tbr label %whcond" << whileNumber << endl << endl;
                        output << "whcond" << whileNumber << ":" << endl;
                        output << "\t%ha_o" << ha_oNumber << " = load i32* %" << expr << endl;
                        ha_oNumber++;
                        output << "\t%ha_o" << ha_oNumber << " = icmp ne i32 %ha_o" << ha_oNumber - 1 << ", 0" << endl;
                        ha_oNumber++;
                        output << "\tbr i1 %ha_o" << ha_oNumber - 1 << ", label %whbody" << whileNumber
                               << ", label %whend" << whileNumber << endl;
                        output << "whbody" << whileNumber << ":" << endl;
                    }
                }
            }



            

           else if (line.find("print") != -1) {
                int indexOfPrint = line.find("print");
                int last = line.find_last_of(")");
                string expr = line.substr(indexOfPrint + 6, last - 6 - indexOfPrint);

                if (expr.find("+") != -1 || expr.find("-") != -1 || expr.find("*") != -1 || expr.find("/") != -1 ||
                    expr.find("choose") != -1) {
                    ha_oCalculator(expr, &output, "print");
                    //output << "\t%ha_o" << ha_oNumber << " = load i32 %ha_o" << ha_oNumber - 1 << endl;
                    output
                            << "\tcall i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 %ha_o"
                            << ha_oNumber - 1 << " )" << endl;
                    ha_oNumber++;
                } else {
                    int first = expr.find_last_of("(");
                    int second = expr.find_last_of(")");
                    expr = expr.substr(first + 1, second - first - 1);
                    if (!isdigit(expr[0])) {
                        output << "\t%ha_o" << ha_oNumber << " = load i32* %" << expr << endl;
                        output
                                << "\tcall i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 %ha_o"
                                << ha_oNumber << " )" << endl;
                        ha_oNumber++;
                    } else {
                        output
                                << "\tcall i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 "
                                << expr << " )" << endl;
                    }
                }
            }


        }


        output << "\tret i32 0" << endl << "}";
    }
    return 0;
}
