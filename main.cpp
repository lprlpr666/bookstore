#include <iostream>
#include <cstdio>
#include "Command.h"
using namespace std;
int main(){
//    freopen("test.txt","r",stdin);
//    freopen("Myans.txt","w",stdout);
    Command BookstoreSystem;
    string cmd;
    int cnt = 0;
    bool flag = 0;
    while(getline(cin, cmd)){
        //对于每一条指令，先解析，再执行
//        cmd = "  \t  ";
//        cnt++;
//        cout << cnt << endl;
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