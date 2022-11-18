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

Expression* copylst(Expression *head){
    if(head==nullptr)return nullptr;
    Expression *res=new Expression();
    res->exptype=head->exptype;
    res->next=copylst(head->next);
    res->head=head->head;
    res->xn=head->xn;
    res->xs=head->xs;
    res->xb=head->xb;
    res->si=head->si;
    res->xc=head->xc;
    return res;
}
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

bool updatedict(Dictionary*dict,std::string name,Expression* exp){
    if(dict!=nullptr){
        if(dict->name==name){
            dict->exp=exp;
            return true;
        }
        else{
            return updatedict(dict->upper,name,exp);
        }
    }
    return false;
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

bool iscommentout(std::vector<std::string> &token){
    return token.size()>0&&token[0]==";";
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
                if(i<process.size()-1)eval(process[i],&dict);
                else return eval(process[i],&dict);   
            }
            
        }
        else if(op->exptype=="name"){
            std::string fn=op->xs;
            Expression* res=new Expression();
            res->xs=fn;
            if(fn=="define"){
                res->exptype="num";
                res->xn=0;
                assert(op->next!=nullptr);
                if(op->next->exptype=="name"){
                    insertdict(upperdict,op->next->xs,eval(op->next->next,&dict));
                }
                else{
                    assert(op->next->exptype=="list");
                    assert(op->next->head!=nullptr);
                    Expression* item=new Expression();
                    item->exptype="func";
                    item->xs=op->next->head->xs;
                    item->head=new Expression();
                    item->head->exptype="list";
                    item->head->next=copylst(op->next->head->next);
                    item->head->next->next=op->next->next;
                    insertdict(upperdict,op->next->head->xs,item);
                }
            }
            else if(fn=="quote"){
                res->exptype="symbol";
                Expression *item=eval(op->next,&dict);
                assert(item->exptype=="string");
                res->xs=item->xs;
                if((res->si=sm[item->xs])==0){
                    res->si=nextid;
                    sm[item->xs]=nextid;
                    nextid++;
                }
            }
            else if(fn=="set!"){
                res->exptype="num";
                res->xn=0;
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                assert(updatedict(upperdict,op->next->xs,eval(op->next->next,&dict)));
            }
            else if(fn=="let"){
                res->exptype="num";
                res->xn=0;
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                if(op->next->exptype=="name"){
                    std::vector<Expression*> process=lst2vec(op->next->next);
                    for(int i=0;i<process.size();i++){
                        if(i==process.size()-1){
                            insertdict(&dict,op->next->xs,eval(process[i],&dict));
                        }
                        else{
                            eval(process[i],&dict);
                        }
                    }
                }
                else{
                    assert(op->next->exptype=="list");
                    Dictionary *temp=dict;
                    std::vector<Expression*> binds=lst2vec(op->next->head);
                    for(int i=0;i<binds.size();i++){
                        assert(binds[i]->exptype=="list");
                        assert(binds[i]->head!=nullptr);
                        assert(binds[i]->head!=nullptr);
                        insertdict(&temp,bins[i]->head->xs,eval(binds[i]->head->next,&dict));
                    }
                    dict=temp;
                    std::vector<Expression*> process=lst2vec(op->next->next);
                    for(int i=0;i<process.size();i++){
                        if(i==process.size()-1){
                            insertdict(&dict,op->next->xs,eval(process[i],&dict));
                        }
                        else{
                            eval(process[i],&dict);
                        }
                    }
                }
            }
            else if(fn=="let*"){
                res->exptype="num";
                res->xn=0;
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                assert(op->next->exptype=="list");
                std::vector<Expression*> binds=lst2vec(op->next->head);
                for(int i=0;i<binds.size();i++){
                    assert(binds[i]->exptype=="list");
                    assert(binds[i]->head!=nullptr);
                    assert(binds[i]->head!=nullptr);
                    insertdict(&dict,bins[i]->head->xs,eval(binds[i]->head->next,&dict));
                    }
                    std::vector<Expression*> process=lst2vec(op->next->next);
                    for(int i=0;i<process.size();i++){
                        if(i==process.size()-1){
                            insertdict(&dict,op->next->xs,eval(process[i],&dict));
                        }
                        else{
                            eval(process[i],&dict);
                        }
                    }
            }
            else if(fn=="if"){
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                Expression *ifcond=eval(op->next,&dict);
                assert(ifcont->exptype=="boolean");
                if(ifcond->xb){
                    return eval(op->next->next,&dict);
                }
                else if(op->next->next->next!=nullptr){
                    return eval(op->next->next->next,&dict);
                }
            }
            else if(fn=="cond"){
                std::vector<Expression*> procs=lst2vec(op->next);
                for(int i=0;i<procs.size();i++){
                    if(procs[i]->head->xs=="else"){
                        std::vector<Expression*> process=lst2vec(procs[i]->head->next);
                        for(int j=0;j>process.size();j++){
                            if(j==process.size()-1)return eval(process[j],&dict);
                            else eval(process[j],&dict);
                        }
                    }
                    Expression *ifcond=eval(procs[i]->head,&dict);
                    assert(ifcont->exptype=="boolean");
                    if(ifcond->xb){
                        std::vector<Expression*> process=lst2vec(procs[i]->head->next);
                        for(int j=0;j>process.size();j++){
                            if(j==process.size()-1)return eval(process[j],&dict);
                            else eval(process[j],&dict);
                        }
                    }
                }
            }
            else if(fn=="and"){
                res->exptype="boolean";
                res->xb=true;
                std::vector<Expression*> v=lst2vec(op->next);
                for(int i=0;i<v.size();i++){
                    Expression *r=eval(v[i],&dict);
                    assert(r->exptype=="boolean");
                    res->xb&=r->xb;
                }
            }
            else if(fn=="or"){
                res->exptype="boolean";
                res->xb=false;
                std::vector<Expression*> v=lst2vec(op->next);
                for(int i=0;i<v.size();i++){
                    Expression *r=eval(v[i],&dict);
                    assert(r->exptype=="boolean");
                    res->xb|=r->xb;
                }
            }
            else if(fn=="begin"){
                std::vector<Expression*> v=lst2vec(op->next);
                for(int i=0;i<v.size();i++){
                    if(i==v.size()-1){
                        return eval(v[i],&dict);
                    }
                    else{
                        eval(v[i],&dict);
                    }
                }
            }
            else if(fn=="do"){
                assert(op->next!=nullptr);
                assert(op->next->exptype=="list");
                std::vector<Expression*> binds=lst2vec(op->next->head),pred=lst2vec(op->next->next);
                for(int i=0;i<binds.size();i++){
                    assert(binds[i]->exptype=="list");
                    assert(binds[i]->head!=nullptr);
                    assert(binds[i]->head->next!=nullptr);
                    assert(binds[i]->head->next->next!=nullptr);
                    assert(binds[i]->head->exptype=="name");
                    Dictionary *temp=dict;
                    insertdict(&temp,binds[i]->head->xs,eval(binds[i]->head->next,dict));
                }            
                dict=temp;
                while(true){
                    for(int i=0;i<binds.size();++){
                        Dictionary *temp=dict;
                        insertdict(&temp,binds[i]->head->xs,eval(binds[i]->head->next->next,dict)); 
                    }
                    dict=temp;
                    for(int i=0;i<pred.size();++){
                        if(pred[i]->exptype=="list"){
                            Expression *p=eval(pred[i]->head,dict);
                            if(p->exptype=="boolean"&&p->xb){
                                return eval(pred[i]->head->next,dict);
                            }
                            else if(p->exptype!="boolean"){
                                eval(pred[i],dict);
                            }
                        }

                    }
                }

            }
            else if(fn=="lambda"){
                res->exptype="func";
                res->head=new Expression();
                res->head->xs="list";
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                res->head->next=op->next->next;
                if(op->next->exptype=="list")res->head->head=op->next->head;
                else {
                    res->head->head=new Expression();
                    res->head->head->exptype="name";
                    res->head->head->xs=op->next->xs;
                }
            }
            

            return res;
            assert(false);
        }
        else{
            return exp;
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
    if(iscommentout(token))return;
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