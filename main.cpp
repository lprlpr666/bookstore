#include<bits/stdc++.h>
#include "Command.h"
using namespace std;
int main(){
    Command BookstoreSystem;
    string cmd;
    int cnt = 0;
    bool flag = 0;
    while(getline(cin, cmd)){
        try{
            BookstoreSystem.pre(cmd);
            BookstoreSystem.get_cmd(cmd);
            BookstoreSystem.run(flag);
            if(flag) break;
        }
        catch (Exception){
            cout << "Invalid\n";
        }
    }
    return 0;
}