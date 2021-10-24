#include <iostream>
#include "parseRun.h"
#include <fstream>
#include "lex.cpp"
#include "val.h"
#include <queue>


using namespace std;

int main(int argc, char* argv[]){
    if (argc > 2)
    {
        
        cout << "ONLY ONE FILE NAME ALLOWED" << endl;
        exit(1);
        
    }

    ifstream myFile;
    myFile.open(argv[1]);

    if (!myFile.is_open())
    {
        cout << "CANNOT OPEN THE FILE " << argv[1];
        exit(1);
    }

    istream *in = &myFile;

    int lnum = 1;

    if (Prog(*in, lnum))
    {
        cout << "\n";
        cout << "Successful Execution" << endl;
    }
    else
    {
        cout << "\n";
        cout << "Unsuccessful Interpretation" << endl;
        cout << "Number of Errors: " << error_count <<endl;
    }

}

bool Prog(istream& in, int& ln)
{
  
    if (Parser::GetNextToken(in,ln) == BEGIN)
    {
        bool rex = StmtList(in,ln);
        if (!rex)
        {
            return false;
        }
        LexItem toke = Parser::GetNextToken(in,ln);
        if (toke == END){
            return true;
        }
        else if (toke == DONE)
        {
            ParseError(ln, "Missing END");
            return false;
        }
    }
    else
    {
        ParseError(ln, "Missing BEGIN");
        return false;
    }

}

bool StmtList(istream& in, int& ln){
    //cout << "in StmtList" << endl;
    LexItem toke = Parser::GetNextToken(in,ln);
    if (toke == END)
    {
        Parser::PushBackToken(toke);
        return true;
    }
    else if (toke == DONE)
    {
        Parser::PushBackToken(toke);
        return true;
    }
    Parser::PushBackToken(toke);
    bool rex = Stmt(in,ln);
    if (!rex)
        return false;
    toke= Parser::GetNextToken(in,ln);
    if (toke == ERR){
        ParseError(ln, "Unrecognized input pattern (" + toke.GetLexeme() + ")");
        return false;
    }
    if (toke != SCOMA){
        ParseError(ln, "Missing semicolon");
        return false;
    }
    return StmtList(in, ln);
}

bool Stmt(istream& in, int& ln)
{
 
    LexItem toke = Parser::GetNextToken(in,ln);
    if (toke == PRINT)
        
        return PrintStmt(in,ln);
    
    else if (toke == IF)
        return IfStmt(in, ln);
    else if (toke == IDENT)
    {
        Parser::PushBackToken(toke);
        return AssignStmt(in,ln);
    }
    else if (toke == ERR)
    {
        ParseError(ln, "Unrecognized input pattern (" + toke.GetLexeme() + ")");
        return false;
    }
    else if (toke == END || toke == DONE)
        return true;
    else{
        ParseError(ln, "Invalid statement");
        return false;
    }
}

bool PrintStmt(istream& in, int& ln){
    
    ValQue = new queue<Value>;
    bool rex = ExprList(in, ln);
    if (!rex){
        ParseError(ln, "Missing expression after print");
        while (!(*ValQue).empty())
            ValQue->pop();
        delete ValQue;
        return false;
    }
    LexItem toke = Parser::GetNextToken(in,ln);
    if (toke == SCOMA){
        while (!(*ValQue).empty()){
            Value nextVal = (*ValQue).front();
            cout << nextVal;
            ValQue->pop();
        }
        cout << endl;
    }
    Parser::PushBackToken(toke);
    return rex;
}

bool AssignStmt(istream& in, int& ln){
    //cout << "in AssignStmt" << endl;
    Value valn;
    LexItem toke = Parser::GetNextToken(in,ln);
    string varName = toke.GetLexeme();
    bool rex = Var(in,ln,toke);
    if (!rex){
        ParseError(ln, "Missing variable");
        return false;
    }
    toke = Parser::GetNextToken(in,ln);
    if (toke == EQ){
        rex = Expr(in,ln,valn);
        if (!rex){
            ParseError(ln, "Missing expression after =");
            return false;
        }
        if (!symbolTable[varName].IsErr() && !(symbolTable[varName].IsInt() && valn.IsReal()) && !(symbolTable[varName].IsReal() && valn.IsInt())){
            if (symbolTable[varName].GetType() != valn.GetType()){
                ParseError(ln, "Illegal Assignment Operation");
                return false;
            }
            
        }
        
        else if ((symbolTable[varName].IsInt() && valn.IsReal())){
            int val = (int) valn.GetReal();
            symbolTable[varName] = Value(val);
            return true;
        }
        

        else if ((symbolTable[varName].IsReal() && valn.IsInt())){
            float val = (float) valn.GetInt();
            symbolTable[varName] = Value(val);
            return true;
        }

        symbolTable[varName] = valn;
        return true;
    }
    else{
        ParseError(ln, "Missing = sign");
        return false;
    }
}

bool IfStmt(istream& in, int& ln){
    //cout << "in IfStmt" << endl;
    Value newval;
    if (Parser::GetNextToken(in,ln) != LPAREN){
        ParseError(ln, "Missing left parentheses");
        return false;
    }

    bool ex = Expr(in,ln,newval);
    if (!ex){
        ParseError(ln, "Missing expression in if statement");
        return false;
    }

    if (Parser::GetNextToken(in,ln) != RPAREN){
        ParseError(ln, "Missing right parentheses");
        return false;
    }

    if (Parser::GetNextToken(in,ln) != THEN){
        ParseError(ln, "Missing then after expression");
        return false;
    }

    if (!newval.IsInt()){
        ParseError(ln, "Expression in If Statement must be of type int");
        return false;
    }

    if(newval.GetInt() != 0){
        ex = Stmt(in,ln);
        if (!ex){
            ParseError(ln, "Missing statement after then");
            return false;
        }
        return true;
    }

    LexItem toke = Parser::GetNextToken(in,ln);
    while (toke != SCOMA)
        toke = Parser::GetNextToken(in,ln   );
    Parser::PushBackToken(toke);
    return true;
}

bool ExprList(istream& in, int& ln){
    //cout << "in ExprList" << endl;
    Value newval;
    bool rex = Expr(in,ln,newval);
    if (!rex){
        ParseError(ln, "Missing expression");
        return false;
    }

    ValQue->push(newval);

    LexItem toke = Parser::GetNextToken(in,ln);
    if (toke == COMA){
        rex = ExprList(in,ln);
        if (!rex){
            ParseError(ln, "Missing expression after comma");
            return false;
        }
        return rex;
    }
    else{
        Parser::PushBackToken(toke);
        return true;
    }
}

bool Expr(istream& in, int& ln, Value& retVal){
    //cout << "in Expr" << endl;
    bool rex = Term(in,ln,retVal);
    if (!rex){
        return false;
    }
    LexItem toke = Parser::GetNextToken(in,ln);
    while (toke == PLUS || toke == MINUS){
        Value nextVal;
        rex = Term(in,ln,nextVal);
        if (!rex){
            ParseError(ln, "Missing term after operator");
            return false;
        }
    if (!retVal.IsInt() && !retVal.IsReal()){
        ParseError(ln, "Illegal statement(s) in operation");
        return false;
    }
    if (!nextVal.IsInt() && !nextVal.IsReal()){
        ParseError(ln, "Illegal statement(s) in operation");
        return false;
    }
    if (toke == PLUS)
        retVal = retVal + nextVal;

    else if (toke == MINUS)
        retVal = retVal - nextVal;
        toke = Parser::GetNextToken(in,ln);
    }
    Parser::PushBackToken(toke);
    return true;
}

bool Term(istream& in, int& ln, Value& retVal)
{
  
    bool rex = Factor(in,ln, retVal);
    if (!rex){
        return false;
    }
    LexItem toke = Parser::GetNextToken(in,ln);
    while (toke == MULT || toke == DIV){
        Value nextVal;
        rex = Factor(in,ln,nextVal);
        if (!rex){
            ParseError(ln, "Missing factor after operator");
            return false;
        }
        if (!retVal.IsInt() && !retVal.IsReal()){
            ParseError(ln, "Illegal statement(s) in operation");
            return false;
        }
        if (!nextVal.IsInt() && !nextVal.IsReal()){
            ParseError(ln, "Illegal statement(s) in operation");
            return false;
        }
        if (toke == MULT)
            retVal = retVal * nextVal;
        else if (toke == DIV)
            retVal = retVal / nextVal;
        toke = Parser::GetNextToken(in,ln);
    }
    Parser::PushBackToken(toke);
    return true;
}

bool Var(istream& in, int& ln, LexItem& toke)
{
 
    string identstr;
    if (toke == IDENT){
        identstr = toke.GetLexeme();

        if (defVar.find(identstr) == defVar.end()){
            defVar[identstr] = true;
    
        }
        return true;
    }
    return false;
}

bool Factor(istream& in, int& ln, Value& retVal)
    {
    //cout << "in Factor" << endl;
    LexItem toke = Parser::GetNextToken(in, ln);

    if( toke == IDENT ) {
       
        string lexe = toke.GetLexeme();
        if (defVar.find(lexe) == defVar.end())
        {
          
            ParseError(ln, "Undefined Variable " + lexe);
            return false;
        }
        retVal = symbolTable.find(lexe)->second;
        
        return true;
    }
    else if( toke == ICONST )
    {
        
        int vale;
        string lexe = toke.GetLexeme();
        vale = stoi(lexe);
        Value newVal(vale);
        retVal = newVal;
        symbolTable[lexe] = retVal;
        return true;
    }
    else if( toke == SCONST )
    {
       
        string lexeme = toke.GetLexeme();
    
        retVal = Value(lexeme);
      
        symbolTable[lexeme] = retVal;
        return true;
    }
    else if( toke == RCONST ) {
       
        float vale;
        string lexe = toke.GetLexeme();
        vale = stof(lexe);
       
        retVal = Value(vale);
        symbolTable[lexe] = retVal;
        return true;
    }
    else if( toke == LPAREN ) {
        Value newvale;
        
        bool rex = Expr(in, ln, newvale);
        if( !rex )
        {
            ParseError(ln, "Missing expression after (");
            return false;
        }
        if( Parser::GetNextToken(in, ln) == RPAREN )
            return rex;

        ParseError(ln, "Missing ) after expression");
        return false;
    }
    else if(toke.GetToken() == ERR)
    {
        ParseError(ln, "Unrecognized Input Pattern");
     
        return false;
    }

    ParseError(ln, "Unrecognized input");
    return 0;
}
