#include <iostream>
#include <cstring>
#include <vector>

const int MAXLENGTH=200+1;//行ごとの最大入力文字数+1
char row[MAXLENGTH];
int nextid=1;//次の新規シンボルID
std::map<std::string,int> sm;

struct Expression{
    //num,name,char,symbol,boolean,func
    std::string exptype;
    Expression *next, *head;//brothers,children
    double xn;//num
    std::string xs;//symbol string
    bool xb;//bool
    int si;//symbol id
    char xc;//char

};
//self->next
//->head->next
std::vector<Expression*> lst2vec(Expression *head)(
    std::vector<Expression*> v;
    for(;;head!=nullptr)(
        v.push_back(head);
        head=head->next;
        v.back()->next=nullptr;
    )
    return v;
)

Expression* vec2lst(std::vector<Expression*> &v){
    if(v.size()==0)return nullptr;
    for(int i=0;i<v.size()-1;i++)v[i]->next=v[i+1];
    return v[0];
}

Expression* str2lst(std::string s){
    Expression* res=new Expression();
    for(int i=s.size()-1;i>=0;i--)(
        Expression* newhead=new Expression();
        newhead->next=res->head;
        res->head=newhead;
        newhead->exptype="char";
        newhead->xc=s[i];
    )
    return res;
}

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
            std::vector<Expression*> v;
            for(i++;token[i]!=")";i++){
                v.push_back(build(token,tokentype,i));    
            }
            exp->head=vec2lst(v);
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
            exp->head=str2lst(token[i]);
        }
    }
    return exp;
}

//eval group
Expression* eval(Expression *exp,Dictionary **upperdict){
    Expression *res=new Expression();
    Dictionary *dict=*upperdict;
    if(exp->exptype!="name"&&exp->exptype!="list"){
        return exp;
    }
    else if(exp->exptype=="name"){
        res=searchdict(dict,exp->xs);
        if(res==nullptr){
            return exp;
        }
        else{
            res->next=exp->next;
            return res;
        }
    }
    else if(exp->exptype=="list"){
        if(exp->head==nullptr)return exp;
        Expression* op=eval(exp->head,dict);
        if(op->exptype=="func"){
            assert(op->head!=nullptr);
            std::vector<Expression*> arg=lst2vec(op->next),dummy=lst2vec(op->head->head),process=lst2vec(op->head->next);
            assert(arg.size()==dummy.size());
            for(int i=0;i<arg.size();i++){
                assert(dummy[i]->exptype=="name");
                insertdict(&dict,dummy[i]->xs,eval(arg[i],upperdict));
            }
            for(int i=0;i<process.size();i++){
                eval(process[i],dict);
            }
            
        }
        else if(op->exptype=="name"){

        }
        else{
            return exp;
        }
        /*std::vector<Expression*> lst(exp->lst.size());
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
        */
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