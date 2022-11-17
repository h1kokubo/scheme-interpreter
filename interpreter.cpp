#include <iostream>
#include <cstring>
#include <vector>

const int MAXLENGTH=200+1;//行ごとの最大入力文字数+1
char row[MAXLENGTH];
int nextid=1;//次の新規シンボルID
std::map<std::string,int> sm;

struct Expression{
    //num,name,string,symbol,boolean,func
    std::vector<Expression*> lst;
    double xn;//num
    std::string xs;//symbol string,string
    bool xb;//bool
    int si;//symbol id

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

Expression* searchdict(Dictionary*dict ,std::string name){
    if(dict!=nullptr){
        if(dict->name==name)return dict->exp;
        else return searchdict(dict->upper,name);
    }
    return nullptr;
}

void insertdict (Dictionary **dict,std::string name,Expression* exp){
    *dict=new Dictionary(*dict,name,exp);
    return;
}

void getline(){    
    std::cin.getline(row,MAXLENGTH);
    //std::cout<<row<<std::endl;
}

bool isexit(){
    return strcmp(row,"exit") == 0;
}

bool isescaped(int i){
    return i>0&& row[i-1]=='\\';
}

void tokenize(std::vector<std::string> &token,std::vector<std::string> &tokentype){
    for(int i=0;i<MAXLENGTH;){
        if(row[i]=='\0')break;
        if(row[i]=='('){
            token.push_back("(");
            tokentype.push_back("");
        }
        else if(row[i]==')'){
            token.push_back(")");
            tokentype.push_back("");
        }
        else if('a'<=row[i]&&row[i]<='z'){
            tokentype.push_back("name");
            token.push_back("");
            while(row[i]!=' '&&row[i]!='('&&row[i]!=')'&&row[i]!='\0'){
                token.back()+=string(1,row[i]);
                i++;
            }
            continue;
        }
        else if('0'<=row[i]&&row[i]<='9'){
            tokentype.push_back("num");
            token.push_back("");
            while(row[i]!=' '&&row[i]!='('&&row[i]!=')'&&row[i]!='\0'){
                token.back()+=string(1,row[i]);
                i++;
            }
            continue;
        }
        else if(row[i]=='#'){
            tokentype.push_back("boolean");
            token.push_back(string(1,row[i+1]));
            i+=2;
            continue;
        }
        else if(row[i]=='\''&&!isescaped(i)){
            token.push_back("");
            while(row[i]!=' '&&row[i]!='('&&row[i]!=')'&&row[i]!='\0'&&(row[i]!='\''||isescaped(i))){
                token.back()+=string(1,row[i]);
                i++;
            }
            if(row[i]!='\''||isescaped(i)){
                tokentype.push_back("symbol");
            }
            else{
                tokentype.push_back("string");
            }
            continue;
        }

        i++;
    }
    
    return ;
}

//build group
Expression* build(std::vector<std::string> &token,std::vector<std::string> &tokentype,int &i){
    Expression *exp=new Expression();
    for(;i<token.size();i++){
        if(token[i]=="("){
            exp->exptype="list";
            for(i++;token[i]!=")";i++){
                exp->lst.push_back(build(token,tokentype,i));    
            }
        }
        else if(tokentype[i]=="name"){
            exp->exptype="name";
            exp->xs=token[i];
        }
        else if(tokentype[i]=="num"){
            exp->exptype="num";
            exp->xn=std::stod(token[i]);
        }
        else if(tokentype[i]=="boolean"){
            exp->exptype="boolean";
            exp->xb=(token[i]=="t");
        }
        else if(tokentype[i]=="symbol"){
            exp->exptype="symbol";
            exp->xs=token[i];
            if((exp->si=sm[token[i]])==0){
                exp->si=nextid;
                sm[token[i]]=nextid;
                nextid++;
            }
        }
        else if(tokentype[i]=="string"){
            exp->exptype="string";
            exp->xs=token[i];
        }
    }
    return exp;
}

//eval group
Expression* eval(Expression *exp,Dictionary **upperdict){
    Expression *res=new Expression();
    Dictionary *dict=upperdict;
    if(exp->exptype!="name"&&exp->exptype!="list"){
        return exp;
    }
    if(exp->exptype=="name"){
        res=searchdict(dict,exp->xs);
        if(res==nullptr){
            return exp;
        }
        else{
            return res;
        }
    }
    if(exp->exptype=="list"){
        if(exp->lst.size()==0)return exp->lst;
        std::vector<Expression*> lst(exp->lst.size());
        for(int i=0;i<lst.size();i++){
            lst[i]=eval(exp->lst[i]);
            assert(!((i>0)&&lst[i]->exptype=="name")); 
        }
        if(lst[0]->exptype!="func"&&lst[0]->exptype!="name"){
            res->lst=lst;
            res->exptype="list";
            return res;
        }
        if(lst[0]->exptype=="func"){
            //関数適用
            assert(lst[0]->lst.size()==lst.size()-1);
            Dictionary* nextdict=dict;
            for(int i=0;i<lst[0]->lst.size();i++){
                assert(lst[0]->lst[i]->exptype=="name");
                insertdict(&nextdict,lst[0]->lst[i]->xs,eval(lst[i+1],&dict));
            }
            for(int i=1;i>lst[0]->lst.size();i++){
                eval(lst[0]->lst[i],&nextdict);
            }
        }
        if(lst[0]->exptype=="name"){
            //基礎関数
            
            //該当なし
            assert(false);
        }
    }
    return ;
}

//print group
void print(Expression *exp){

}

Dictionary *Top=NULL;

void proc(){
    //tokenize input
    std::vector<std::string> token,tokentype;
    tokenize(token,tokentype)
    //construct expression tree
    Expression *exp,*result;
    int i=0;
    exp=build(token,tokentype,i)
    //evaluate expression tree
    result=eval(exp,&Top);
    print(result);
    return 0;
}

int main(){
    for(;;){
        getline();
        if(isexit())break;
        proc()
    }
    return 0;
}