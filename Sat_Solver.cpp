#include<iostream>
#include<set>
#include<vector>
#include<string.h>
#include<time.h>

using namespace std;

//Global variables
char a;
string s;
int n;
int c;
bool exist = false;

//Function to remove unnecessary literal from the clauses
set<set<int> > remove(int x , set<set<int> > temp,bool a){
    set<set<int> > clauses = temp;
    set<set<int> > to_delete;
    set<set<int> > in;
    if(x<0)x=-x;
    for(auto r : clauses){
        if( r.count(x)  ){
            if(a==true)to_delete.insert(r);
            else {
                set<int> t = r;
                t.erase(x);
                to_delete.insert(r);
                in.insert(t);
            }
        }
        else if( r.count(-x) !=0 ){
            if(a==false)to_delete.insert(r);
            else {
                set<int> t = r;
                t.erase(-x);
                to_delete.insert(r);
                in.insert(t);
            }
        }
    }
    for(auto y : to_delete){
        clauses.erase(y);
    }
    for(auto y : in){
        clauses.insert(y);
    }
    return clauses;
}

vector<int> ans;                 //List to store the model ans

//Function implementing the DPLL algorithm
void solve(vector<int> model,set<set<int> > clauses){
    if(exist==true)return;          //SAT model already found so return

    if(clauses.size()==0){          //all the clauses have been evaluated to true
        ans = model;
        exist = true;
        return;
    }
    else {                          // checking if there exist an empty clause
                                    //if it exists there can be no answer with the already choosen model
        for(auto y : clauses){
            if(y.size()==0){
                return;
            }
        }
    }

    //now checking for unit literals 
     
    for(auto it : clauses ){
        set<int> y = it;
        if( y.size()==1){   
            auto h = y.begin();
            int g = *h;
            if(g>0){
                model[g] = 1;
                set<set<int> > temp;
                temp = remove(g,clauses,true);
                solve(model,temp);
            }
            else {
                g = -g;
                model[g] = -1;
                set<set<int > > temp;
                temp = remove(g,clauses,false);
                solve(model,temp);
            }
            return;
        }
    }
    //ends here

    //now checking for pure literals
    vector<set<int> > q(n+1);
    for(auto x : clauses){
        for( auto y : x){
            q[abs(y) ].insert(y) ;
        }
    }
    for( int i=1;i<=n;i++){
        if( q[i].size()==1 ){
            auto b = q[i].begin();
            int B = *b;
            if( B < 0 ){
                model[i]=-1;
                set<set<int > > temp;
                temp = remove(i,clauses,false);
                solve(model,temp);
            }
            else if( B > 0 ){
                model[i] = 1;
                set<set<int > > temp;
                temp = remove(i,clauses,true);
                solve(model,temp);
            }
            return;
        } 
    }
    //ends here

    //To further optimize the solution, we choose the literal with the max frequency in the clauses of min length
    int min=10000;
    set<set<int> > optimal;
    for(auto x : clauses){
        if(min==x.size()) optimal.insert(x);
        else if(min > x.size()){
            optimal.clear();
            min = x.size();
            optimal.insert(x);
        }
    }
    vector<int> count(n+1,0);
    for(auto x : optimal){
        for(auto y : x){
            count[abs(y)]++;
        }
    }
    int idx ;
    int max=-1;
    for(int i=1;i<=n;i++){
        if(max < count[i]){
            max = count[i];
            idx = i;
        }
    }
    //ends here

    //Backtracking
    model[idx]=1;
    set<set<int> > t1,t2;
    t1 = remove(idx,clauses,true);       //First assume true and then find solution
    solve(model,t1);
    if(exist==true)return;
    model[idx]=-1;
    t2 = remove(idx,clauses,false);      //Now assume false and then find solution
    solve(model,t2);
    return;
}


int main(int argc, char **argv){

    FILE *in_file  = fopen(argv[1], "r");      //Open the cnf file in read only mode
    if (in_file==NULL){                        //If file does not open show error
        printf("Error! Could not open file\n");
        exit(-1);
    }
    int literals = 0,propositions = 0;         //Number of literals and propositions
    int num,check,count = 0;                   //Helper variables

    char initials[10000];                      //To take input
    fscanf(in_file,"%s",initials);
    if(initials[0]=='c')
        do{
            fgets(initials,10000,in_file);     //taking input of each line if the line is a comment
            fscanf(in_file,"%s",initials);
        }while(initials[0]=='c'); 

    fscanf(in_file, "%s",initials);          //reading "cnf" as string
    fscanf(in_file, "%s",initials);          //reading number of propositions in string format
    int len=strlen(initials);                //length of initials string to calculate number of propositions
    while(count<len){
        propositions=10*propositions+initials[count]-'0';
        count++;
    }

    n=propositions;                         //Global variable n to be used in methods

    count=0;
    fscanf(in_file, "%s",initials);         //reading number of conditions in string format
    len=strlen(initials);                   //length of initials string to calculate number of conditions
    while(count<len){
        literals=10*literals+initials[count]-'0';
        count++;
    }    

    set<set<int> > clauses;                     //Set of Set to take as input all the clauses

    for(int i=0; i<literals; i++){
        set<int> t;
        do{
            count=0;num=0;check=0;
            fscanf(in_file,"%s",initials);      //Taking a number in string format
            len=strlen(initials);
            if(initials[0]=='-'){
                count=1;                        //If first character is - then number starts from next index and we make check=1 which ensures we take care of sign
                check=1;
            }
            while(count<len){
                num=10*num+initials[count]-'0';
                count++;
            }
            if(check==1) num=-num;           //If first sign was - then taking negative of number obtained
            // cout<<num;
            if(num!=0)
                t.insert(num);               //Insert the literals in t
        }while(num!=0);                      //Stop taking input if a 0 is encountered
        clauses.insert(t);                         //Insert the taken proposition in the list of clauses
    }

    vector<int> a(n+1,0);                                 //Empty vector to pass as an argument

    solve(a,clauses);

    FILE *ansfile = fopen("answer.txt","w");               //Text file for printing final output
    if (ansfile==NULL){                                    //If file does not open show error
        printf("Error! Could not open file\n");            //and return
        exit(-1);
    }
    if(exist == true){                                     //If no contradiction is found we print SAT in txt file
        fprintf(ansfile, "SATISFIABLE\n" );
        for(int i = 1;i<=n;i++){
            if(ans[i] ==1){
                //cout << i<<" ";
                fprintf(ansfile, "%d ",i);
            }
            else //cout << -i<<" ";
            fprintf(ansfile, "%d ",-i);
        }
    }
    if(exist== false){                                    //If no possible solution exists then we print UNSAT in txt file
        fprintf(ansfile,"UNSATISFIABLE\n");
    }
    fclose(ansfile); 

    cout << "Time : " << 1000 * ((double)clock()) / (double)CLOCKS_PER_SEC << "ms\n";

    return 0; 
}