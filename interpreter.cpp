#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cassert>
#include <map>

void print(std::vector<std::string> &v){
    for(int i=0;i<v.size();i++){
        if(i>0)std::cout<<" ";
        std::cout<<v[i];
    }
    std::cout<<std::endl;
}

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
    Expression(){}
    Expression(Expression *exp):exptype(exp->exptype),next(exp->next),head(exp->head),xn(exp->xn),xs(exp->xs),xb(exp->xb),si(exp->si),xc(exp->xc){}
};
//self->next
//->head->next

Expression* copylst(Expression *head){
    if(head==nullptr)return nullptr;
    Expression *res=new Expression();
    res->exptype=head->exptype;
    res->next=copylst(head->next);
    res->head=copylst(head->head);
    res->xn=head->xn;
    res->xs=head->xs;
    res->xb=head->xb;
    res->si=head->si;
    res->xc=head->xc;
    return res;
}
std::vector<Expression*> lst2vec(Expression *head){
    std::vector<Expression*> v;
    while(head!=nullptr){
        v.push_back(head);
        head=head->next;
        v.back()->next=nullptr;
    }
    return v;
}

Expression* vec2lst(std::vector<Expression*> &v){
    if(v.size()==0)return nullptr;
    for(int i=0;i<v.size()-1;i++)v[i]->next=v[i+1];
    return v[0];
}

Expression* str2lst(std::string &s){
    Expression* res=new Expression();
    for(int i=s.size()-1;i>=0;i--){
        Expression* newhead=new Expression();
        newhead->next=res->head;
        res->head=newhead;
        newhead->exptype="char";
        newhead->xc=s[i];
    }
    return res;
}

std::string lst2str(Expression *head){
    std::string res="";
    while(head!=nullptr){
        res+=std::string(1,head->xc);
    }
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

void setline(std::vector<std::string> &v,int i){
    if(v.size()<=i){
        std::cout<<"test["<<i<<"] :not exist"<<std::endl;
        return;
    }
    for(int j=0;j<=v[i].size();j++){
        if(j==v[i].size())row[j]='\0';
        else row[j]=v[i][j];
    }
    return;
}

bool isexit(){
    return strcmp(row,"exit") == 0;
}
bool istest(){
    return strcmp(row,"test") == 0;
}

bool iscommentout(std::vector<std::string> &token){
    return token.size()>0&&token[0]==";";
}

bool isescaped(int i){
    return i>0&& row[i-1]=='\\';
}

bool isnegativeval(std::string &s){
    return s.size()>=2&&s[0]=='-'&&'0'<=s[1]&&s[1]<='9';
}

void tokenize(std::vector<std::string> &token,std::vector<std::string> &tokentype){
    for(int i=0;i<MAXLENGTH;){
        if(row[i]=='\0'||row[i]==';')break;
        if(row[i]=='('){
            token.push_back("(");
            tokentype.push_back("");
        }
        else if(row[i]==')'){
            token.push_back(")");
            tokentype.push_back("");
        }
        
        else if('0'<=row[i]&&row[i]<='9'){
            tokentype.push_back("num");
            token.push_back("");
            while(row[i]!=' '&&row[i]!='('&&row[i]!=')'&&row[i]!='\0'&&row[i]!=';'){
                token.back()+=std::string(1,row[i]);
                i++;
            }
            continue;
        }
        else if(row[i]=='#'){
            tokentype.push_back("boolean");
            token.push_back(std::string(1,row[i+1]));
            i+=2;
            continue;
        }
        else if(row[i]=='\''&&!isescaped(i)){
            token.push_back("");
            while(row[i]!=' '&&row[i]!='('&&row[i]!=')'&&row[i]!='\0'&&(row[i]!='\''||isescaped(i))&&row[i]!=';'){
                token.back()+=std::string(1,row[i]);
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
        else if(row[i]!=' '){
            tokentype.push_back("name");
            token.push_back("");
            while(row[i]!=' '&&row[i]!='('&&row[i]!=')'&&row[i]!='\0'&&row[i]!=';'){
                token.back()+=std::string(1,row[i]);
                i++;
            }
            if(isnegativeval(token.back())){
                tokentype.back()="num";
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
    //for(;i<token.size();i++){
        //std::cout<<i<<std::endl;
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
    //}
    return exp;
}

//copy next
Expression* wrap(Expression *exp,Expression *res){
    res->next=exp->next;
    return res;
}

//print group
void print(Expression *exp){
    if(exp==nullptr){
        std::cout<<"()"<<std::endl;
    }
    else{
        std::string type=exp->exptype;
        if(type=="num"){
            std::cout<<exp->xn<<std::endl;
        }
        else if(type=="name"){
            std::cout<<exp->xs<<std::endl;
        }
        else if(type=="char"){
            std::cout<<exp->xc<<std::endl;
        }
        else if(type=="symbol"){
            std::cout<<"id: "<<exp->si<<", string"<<exp->xs<<std::endl;
        }
        else if(type=="boolean"){
            if(exp->xb){
                std::cout<<"#t"<<std::endl;
            }
            else{
                std::cout<<"#f"<<std::endl;
            }
        }
        else if(type=="func"){
            std::cout<<"procedure "<<exp<<std::endl;
        }
        else if(type=="list"){
            Expression *now=exp->head;
            std::cout<<"( ";
            while(now!=nullptr){
                print(now);
                now=now->next;
            }
            std::cout<<" )"<<std::endl;
        }


    }
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
            return wrap(exp,res);
        }
    }
    else if(exp->exptype=="list"){
        if(exp->head==nullptr)return exp;
        Expression* op=eval(exp->head,&dict);
        if(op->exptype=="func"){
            //std::cout<<"func start"<<std::endl;
            assert(op->head!=nullptr);
            std::vector<Expression*> arg=lst2vec(copylst(op->next)),dummy=lst2vec(copylst(op->head->head)),process=lst2vec(copylst(op->head->next));
            assert(arg.size()==dummy.size());
            for(int i=0;i<arg.size();i++){
                assert(dummy[i]->exptype=="name");
                insertdict(&dict,dummy[i]->xs,eval(arg[i],upperdict));
            }
            
            for(int i=0;i<process.size();i++){
                if(i<process.size()-1)eval(process[i],&dict);
                else return wrap(exp,eval(process[i],&dict));   
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
                    //
                    item->head->head=copylst(op->next->head->next);
                    item->head->next=op->next->next;
                    //
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
                assert(updatedict(*upperdict,op->next->xs,eval(op->next->next,&dict)));
            }
            else if(fn=="let"){
                res->exptype="num";
                res->xn=0;
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                std::vector<Expression*> binds,process;
                if(op->next->exptype=="name"){
                    if(op->next->next->exptype=="list"){
                        assert(op->next->next->next!=nullptr);
                        binds=lst2vec(copylst(op->next->next->head));
                        process=lst2vec(copylst(op->next->next->next));
                        Dictionary *temp=dict;
                        for(int i=0;i<binds.size();i++){
                            assert(binds[i]->exptype=="list");
                            assert(binds[i]->head!=nullptr);
                            assert(binds[i]->head->next!=nullptr);
                            insertdict(&temp,binds[i]->head->xs,eval(binds[i]->head->next,&dict));
                        }
                        dict=temp;
                    }
                    else{
                        process=lst2vec(copylst(op->next->next));
                    }
                    for(int i=0;i<process.size();i++){
                        if(i==process.size()-1){
                            insertdict(upperdict,op->next->xs,eval(process[i],&dict));
                            return wrap(exp,eval(process[i],&dict));
                        }
                        else{
                            eval(process[i],&dict);
                        }
                    }
                }
                else{
                    if(op->next->exptype=="list"){
                        assert(op->next->next!=nullptr);
                        binds=lst2vec(copylst(op->next->head));
                        process=lst2vec(copylst(op->next->next));
                        Dictionary *temp=dict;
                        for(int i=0;i<binds.size();i++){
                            assert(binds[i]->exptype=="list");
                            assert(binds[i]->head!=nullptr);
                            assert(binds[i]->head->next!=nullptr);
                            insertdict(&temp,binds[i]->head->xs,eval(binds[i]->head->next,&dict));
                        }
                        dict=temp;
                    }
                    else{
                        process=lst2vec(op->next);
                    }
                    for(int i=0;i<process.size();i++){
                        if(i==process.size()-1){
                            return wrap(exp,eval(process[i],&dict));
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
                std::vector<Expression*> binds,process;
                if(op->next->exptype=="name"){
                    if(op->next->next->exptype=="list"){
                        assert(op->next->next->next!=nullptr);
                        binds=lst2vec(copylst(op->next->next->head));
                        process=lst2vec(copylst(op->next->next->next));
                        for(int i=0;i<binds.size();i++){
                            assert(binds[i]->exptype=="list");
                            assert(binds[i]->head!=nullptr);
                            assert(binds[i]->head->next!=nullptr);
                            insertdict(&dict,binds[i]->head->xs,eval(binds[i]->head->next,&dict));
                        }
                        
                    }
                    else{
                        process=lst2vec(copylst(op->next->next));
                    }
                    for(int i=0;i<process.size();i++){
                        if(i==process.size()-1){
                            insertdict(upperdict,op->next->xs,eval(process[i],&dict));
                            return wrap(exp,eval(process[i],&dict));
                        }
                        else{
                            eval(process[i],&dict);
                        }
                    }
                }
                else{
                    if(op->next->exptype=="list"){
                        assert(op->next->next!=nullptr);
                        binds=lst2vec(copylst(op->next->head));
                        process=lst2vec(copylst(op->next->next));
                        for(int i=0;i<binds.size();i++){
                            assert(binds[i]->exptype=="list");
                            assert(binds[i]->head!=nullptr);
                            assert(binds[i]->head->next!=nullptr);
                            insertdict(&dict,binds[i]->head->xs,eval(binds[i]->head->next,&dict));
                        }
                     
                    }
                    else{
                        process=lst2vec(op->next);
                    }
                    for(int i=0;i<process.size();i++){
                        if(i==process.size()-1){
                            return wrap(exp,eval(process[i],&dict));
                        }
                        else{
                            eval(process[i],&dict);
                        }
                    }
                }
            }
            else if(fn=="if"){
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                Expression *ifcond=eval(op->next,&dict);
                assert(ifcond->exptype=="boolean");
                if(ifcond->xb){
                    return wrap(exp,eval(op->next->next,&dict));
                }
                else if(op->next->next->next!=nullptr){
                    return wrap(exp,eval(op->next->next->next,&dict));
                }
            }
            else if(fn=="cond"){
                std::vector<Expression*> procs=lst2vec(copylst(op->next));
                for(int i=0;i<procs.size();i++){
                    assert(procs[i]->head!=nullptr);
                    assert(procs[i]->head->next!=nullptr);
                    Expression *ifcond=eval(procs[i]->head,&dict);
                    if(ifcond->exptype=="name"&&ifcond->xs=="else"){
                        std::vector<Expression*> process=lst2vec(copylst(procs[i]->head->next));
                        for(int j=0;j<process.size();j++){
                            if(j==process.size()-1)return wrap(exp,eval(process[j],&dict));
                            else eval(process[j],&dict);
                        }
                    }
                    else{
                        assert(ifcond->exptype=="boolean");
                        //std::cout<<ifcond->xb<<std::endl;
                        if(ifcond->xb){
                            //std::cout<<"inner"<<std::endl;
                            std::vector<Expression*> process=lst2vec(copylst(procs[i]->head->next));
                            for(int j=0;j<process.size();j++){
                                std::cout<<"inner"<<std::endl;
                                if(j==process.size()-1)return wrap(exp,eval(process[j],&dict));
                                else eval(process[j],&dict);
                            }
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
                        return wrap(exp,eval(v[i],&dict));
                    }
                    else{
                        eval(v[i],&dict);
                    }
                }
            }
            else if(fn=="do"){
                assert(op->next!=nullptr);
                assert(op->next->exptype=="list");
                assert(op->next->next!=nullptr);
                std::vector<Expression*> binds=lst2vec(copylst(op->next->head)),pred=lst2vec(copylst(op->next->next));
                std::vector<std::vector<Expression*>> bind(binds.size());
                for(int i=0;i<bind.size();i++){
                    bind[i]=lst2vec(copylst(binds[i]->head));
                }
                Dictionary *temp=dict;
                for(int i=0;i<binds.size();i++){
                    assert(bind[i].size()==3);
                    assert(bind[i][0]->exptype=="name");
                    insertdict(&temp,bind[i][0]->xs,eval(bind[i][1],&dict));
                }            
                dict=temp;
                std::cout<<"binds end"<<std::endl;
                while(true){
                    for(int i=0;i<pred.size();i++){
                        if(pred[i]->exptype=="list"){
                            Expression *p=eval(pred[i]->head,&dict);
                            assert(p!=nullptr);
                            assert(p->exptype=="boolean");
                            
                            if(p->xb){
                                return eval(pred[i]->head->next,&dict);
                            }
                        }
                        else eval(pred[i],&dict);

                    }
                    temp=dict;
                    for(int i=0;i<bind.size();i++){
                        
                    }

                    for(int i=0;i<bind.size();i++){
                        //std::cout<<i<<std::endl;
                        //print(vec2lst(bind[i]));
                        //assert(binds[i]->head->next->next!=nullptr);
                        //Expression* t=binds[i]->head->next->next;
                        insertdict(&temp,bind[i][0]->xs,eval(bind[i][2],&dict)); 
                        //binds[i]->head->next->next=t;
                    }
                    dict=temp;
                }

            }
            else if(fn=="lambda"){
                res->exptype="func";
                res->head=new Expression();
                res->head->xs="list";
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                res->head->next=op->next->next;
                if(op->next->exptype=="list")res->head->head=copylst(op->next->head);
                else {
                    res->head->head=new Expression();
                    res->head->head->exptype="name";
                    res->head->head->xs=op->next->xs;
                }
            }
            else if(fn=="number?"){
                res->exptype="boolean";
                assert(op->next!=nullptr);
                res->xb=(eval(op->next,&dict)->exptype=="num");
            }
            else if(fn=="+"){
                res->exptype="num";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                res->xn=0;
                for(int i=0;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xn+=v[i]->xn;
                }
            }
            else if(fn=="-"){
                res->exptype="num";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                v[0]=eval(v[0],&dict);
                assert(v[0]->exptype=="num");
                res->xn=v[0]->xn;
                for(int i=1;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xn-=v[i]->xn;
                }
            }
            else if(fn=="*"){
                res->exptype="num";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                res->xn=1;
                for(int i=0;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xn*=v[i]->xn;
                }
            }
            else if(fn=="/"){
                res->exptype="num";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                v[0]=eval(v[0],&dict);
                assert(v[0]->exptype=="num");
                res->xn=v[0]->xn;
                for(int i=1;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xn/=v[i]->xn;
                }
            }
            else if(fn=="="){
                res->exptype="boolean";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                v[0]=eval(v[0],&dict);
                assert(v[0]->exptype=="num");
                res->xb=true;
                for(int i=1;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xb&=(v[i]->xn==v[0]->xn);
                }
            }
            else if(fn=="<"){
                res->exptype="boolean";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                v[0]=eval(v[0],&dict);
                assert(v[0]->exptype=="num");
                res->xb=true;
                for(int i=1;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xb&=(v[i-1]->xn<v[i]->xn);
                }
            }
            else if(fn=="<="){
                res->exptype="boolean";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                v[0]=eval(v[0],&dict);
                assert(v[0]->exptype=="num");
                res->xb=true;
                for(int i=1;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xb&=(v[i-1]->xn<=v[i]->xn);
                }
            }
            else if(fn==">"){
                res->exptype="boolean";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                v[0]=eval(v[0],&dict);
                assert(v[0]->exptype=="num");
                res->xb=true;
                for(int i=1;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xb&=(v[i-1]->xn>v[i]->xn);
                }
            }
            else if(fn==">="){
                res->exptype="boolean";
                std::vector<Expression*> v=lst2vec(copylst(op->next));
                assert(v.size()>=2);
                v[0]=eval(v[0],&dict);
                assert(v[0]->exptype=="num");
                res->xb=true;
                for(int i=1;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                    assert(v[i]->exptype=="num");
                    res->xb&=(v[i-1]->xn>=v[i]->xn);
                }
            }
            else if(fn=="null?"){
                res->exptype="boolean";
                res->xb=(op->next==nullptr);
                if(!res->xb){
                    Expression*r=eval(op->next,&dict);
                    res->xb=(r->exptype=="list"&&r->head==nullptr);
                }
            }
            else if(fn=="pair?"){
                res->exptype="boolean";
                Expression*r=eval(op->next,&dict);
                res->xb=(r->head!=nullptr&&r->head->next!=nullptr&&r->head->next->next==nullptr);
            }
            else if(fn=="list?"){
                res->exptype="boolean";
                Expression*r=eval(op->next,&dict);
                res->xb=(r->exptype=="list");
            }
            else if(fn=="symbol?"){
                res->exptype="boolean";
                Expression*r=eval(op->next,&dict);
                res->xb=(r->exptype=="symbol");
            }
            else if(fn=="car"){
                assert(op->next->exptype=="list");
                return eval(op->next->head,&dict);
            }
            else if(fn=="cdr"){
                assert(op->next->exptype=="list");
                assert(op->next->head!=nullptr);
                res->exptype="list";
                res->head=op->next->head->next;
            }
            else if(fn=="cons"){
                assert(op->next->head!=nullptr);
                assert(op->next->head->next!=nullptr);
                res->exptype="list";
                res->head=eval(op->next->head,&dict);
                res->head->next=eval(op->next->head->next,&dict);
            }
            else if(fn=="list"){
                res->exptype="list";
                std::vector<Expression*> v=lst2vec(op->next);
                for(int i=0;i<v.size();i++){
                    v[i]=eval(v[i],&dict);
                }
                res->head=vec2lst(v);
            }
            else if(fn=="length"){
                res->exptype="num";
                std::vector<Expression*> v=lst2vec(op->next);
                res->xn=v.size();
            }
            else if(fn=="memq"){
                res->exptype="boolean";
                assert(op->next!=nullptr);
                assert(op->next->exptype=="symbol");
                assert(op->next->next!=nullptr);
                assert(op->next->next->exptype=="list");
                res->xb=false;
                for(Expression *now=op->next->next->head;now!=nullptr;now=now->next){
                    if(now->exptype=="symbol"&&op->next->si==now->si){
                        res->exptype="list";
                        res->head=now;
                        break;
                    }
                    else if(now->exptype=="string"&&op->next->si==sm[now->xs]){
                        res->exptype="list";
                        res->head=now;
                        break;
                    }
                }
            }
            else if(fn=="last"){
                assert(op->next!=nullptr);
                assert(op->next->exptype=="list");
                Expression *r=nullptr,*n=op->next->head;
                for(;n!=nullptr;r=eval(n,&dict),n=n->next){}
                res=r;
            }
            else if(fn=="append"){//(append lst1 lst2)
                res->exptype="list";
                assert(op->next!=nullptr);
                assert(op->next->exptype=="list");
                assert(op->next->next!=nullptr);
                assert(op->next->next->exptype=="list");
                Expression *lst1=eval(op->next,&dict),*lst2=eval(op->next->next,&dict);
                
                if(lst1->head==nullptr){
                    res->head=lst2->head;
                }
                else{
                    res->head=lst1->head;
                    Expression *r=nullptr,*n=res->head;
                    for(;n!=nullptr;r=new Expression(n),n=n->next){}
                    r->next=lst2->head;
                }
            }
            else if(fn=="set-car!"){//(set-car! item lst)
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                op->next=eval(op->next,&dict);
                op->next->next=eval(op->next->next,&dict);
                assert(op->next->next->exptype=="list");
                assert(op->next->next->head!=nullptr);
                Expression *temp=op->next->next->head;
                op->next->next->head=op->next;
                op->next->next=temp;
                res->exptype="num";
                res->xn=0;
            }
            else if(fn=="set-cdr!"){//(set-cdr! lst newcdr)
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                op->next=eval(op->next,&dict);
                op->next->next=eval(op->next->next,&dict);
                assert(op->next->exptype=="list");
                assert(op->next->head!=nullptr);
                assert(op->next->next->exptype=="list");
                op->next->head->next=op->next->next->head;
                res->exptype="num";
                res->xn=0;
            }
            else if(fn=="boolean?"){
                assert(op->next!=nullptr);
                res->exptype="boolean";
                op->next=eval(op->next,&dict);
                res->xb=(op->next->exptype=="boolean");
            }
            else if(fn=="not"){
                assert(op->next!=nullptr);
                res->exptype="boolean";
                op->next=eval(op->next,&dict);
                res->xb=!op->next->xb;
            }
            else if(fn=="string?"){
                assert(op->next!=nullptr);
                res->exptype="boolean";
                op->next=eval(op->next,&dict);
                res->xb=(op->next->exptype=="string");
            }
            else if(fn=="string-append"){// (string-append str1 str2)
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                op->next=eval(op->next,&dict);
                op->next->next=eval(op->next->next,&dict);
                assert(op->next->exptype=="string");
                assert(op->next->next->exptype=="string");
                Expression *r=nullptr,*n=op->next;
                for(;n!=nullptr;r=new Expression(),r->exptype="char",r->xc=n->xc,n=n->next){}
                r->next=op->next->next->head;
            }
            else if(fn=="symbol->string"){
                assert(op->next!=nullptr);
                op->next=eval(op->next,&dict);
                assert(op->next->exptype=="symbol");
                res->exptype="string";
                res->head=str2lst(op->next->xs)->head;
            }
            else if(fn=="string->symbol"){
                assert(op->next!=nullptr);
                op->next=eval(op->next,&dict);
                assert(op->next->exptype=="string");
                res->exptype="symbol";
                res->xs=lst2str(op->next->head);
                if((res->si=sm[res->xs])==0){
                    res->si=nextid;
                    sm[res->xs]=nextid;
                    nextid++;
                }
            }
            else if(fn=="string->number"){
                assert(op->next!=nullptr);
                op->next=eval(op->next,&dict);
                assert(op->next->exptype=="string");
                res->exptype="num";
                res->xn=std::stod(op->next->xs);
            }
            else if(fn=="number->string"){
                assert(op->next!=nullptr);
                op->next=eval(op->next,&dict);
                assert(op->next->exptype=="num");
                res->exptype="string";
                res->xs=std::to_string(op->next->xn);
            }
            else if(fn=="procedure?"){
                assert(op->next!=nullptr);
                op->next=eval(op->next,&dict);
                res->exptype="boolean";
                res->xb=(op->next->exptype=="func");
            }
            else if(fn=="eq?"){
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                op->next=eval(op->next,&dict);
                op->next->next=eval(op->next->next,&dict);
                assert(op->next->exptype=="symbol");
                assert(op->next->next->exptype=="symbol");
                res->exptype="boolean";
                res->xb=(op->next->si==op->next->next->si);
            }
            else if(fn=="neq?"){
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                op->next=eval(op->next,&dict);
                op->next->next=eval(op->next->next,&dict);
                assert(op->next->exptype=="symbol");
                assert(op->next->next->exptype=="symbol");
                res->exptype="boolean";
                res->xb=(op->next->si!=op->next->next->si);
            }
            else if(fn=="equal?"){
                assert(op->next!=nullptr);
                assert(op->next->next!=nullptr);
                op->next=eval(op->next,&dict);
                op->next->next=eval(op->next->next,&dict);
                assert(op->next->exptype=="string");
                assert(op->next->next->exptype=="string");
                res->exptype="boolean";
                res->xb=(op->next->xs==op->next->next->xs);
            }
            else{
                assert(false);
            }
            res=wrap(exp,res);
            return res;
            
        }
        else{
            return exp;
        }
        
    }
    return nullptr;
}



Dictionary *Top=NULL;



void proc(){
    //tokenize input
    std::vector<std::string> token,tokentype;
    tokenize(token,tokentype);
    //print(token);
    //print(tokentype);
    if(iscommentout(token))return;
    //construct expression tree
    Expression *exp,*result;
    int i=0;
    exp=build(token,tokentype,i);
    //print(exp);
    //std::cout<<"end"<<std::endl;
    //evaluate expression tree
    result=eval(exp,&Top);
    print(result);
    return ;
}

void testmode(){
    std::ifstream reading_file;
    reading_file.open("test.txt",std::ios::in);
    std::string line;
    std::vector<std::string> testlst;
    while(std::getline(reading_file,line)){
        testlst.push_back(line);
        line=std::string();
    }
    std::cout<<"test-mode"<<std::endl;
    std::cout<<testlst.size()<<" tests exist"<<std::endl;
    for(;;){
        getline();
        if(isexit())break;
        std::cout<<"test"<<std::stoi(row)<<" begin"<<std::endl;
        setline(testlst,std::stoi(row)-1);
        proc();
    }
    return;
}

int main(){
    for(;;){
        getline();
        if(isexit())break;
        if(istest()){
            testmode();
        }
        else{
            proc();
        }
    }
    return 0;
}