#include <iostream>
#include <cstring>
#include <vector>

const int MAXLENGTH=200+1;//行ごとの最大入力文字数+1
char row[MAXLENGTH];
int counter=0;//lambda　無名関数にユニークに使用するID

struct Expression{
    Expression *left=NULL,*right=NULL;
};

struct Integer: Expression {
    int x;
};

struct Double: Expression {
    double x;
};

struct Bool: Expression {
    bool x;
};

struct Char: Expression {
    char x;
};

struct Symbol: Expression {
    std::string name;
};

struct Function: Expression {
    std::string name;
};

struct Eval: Expression {
    
};

struct Dictionary{
    Dictionary* upper;
    std::string name;
    Expression *exp;
    Dictionary(Dictionary* dict,std::string name_,Expression *exp_){
        upper=dict;
        name=name_;
        exp=exp_;
    }
};

void getline(){    
    std::cin.getline(row,MAXLENGTH);
    //std::cout<<row<<std::endl;
}

bool endFlag(){
    return strcmp(row,"exit") == 0;
}

int tokenize(std::vector<std::string> &token){
    
    return 0;
}

//build group
Expression* build(Expression *exp,std::vector<std::string> &token,int i){
    //token[i] : symbol
    
    return NULL;
}

//eval group
Expression* eval(Expression *exp,Dictionary **upperDict){
    return NULL;
}

//print group
void print(Expression *exp){

}

Dictionary *Top=NULL;

int proc(){
    //tokenize input
    std::vector<std::string> token;
    if(tokenize(token)==-1){
        std::cout<<"tokenize failed"<<std::endl;
        return -1;
    }
    //construct expression tree
    Expression *exp;
    if((exp=build(exp,token,0))==NULL){
        std::cout<<"expression build failed"<<std::endl;
        return -1;
    }
    //evaluate expression tree
    Expression *result;
    if((result=eval(exp,&Top))==NULL){
        std::cout<<"expression evalation failed"<<std::endl;
        return -1;
    }
    print(result);
    return 0;
}

int main(){
    for(;;){
        getline();
        if(endFlag())break;
        if(proc()==-1){
            std::cout<<"process failed"<<std::endl;
            continue;
        }
    }
    return 0;
}