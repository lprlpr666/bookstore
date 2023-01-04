//
// Created by zbl on 2021/12/18.
//

#ifndef BOOKSTORE_COMMAND_H
#define BOOKSTORE_COMMAND_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include "Account.h"
#include "Books.h"
#include "Log.h"
#include "Exception.h"

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

class Command{//中枢系统，完成以下任务 1.指令的合法性的检查。2.指令的解析。 3.指令的运行。
private:
    vector<string> Pars_Op;//记录解析后的操作
    Account_System AccountSystem;
    Book_System BookSystem;
    Log_System LogSystem;
public:
    Command(){//创建超级用户
        AccountSystem.init();
    }
    void pre(string& cmd){
        //长度最大为1024
        if(cmd.length() > 1024) throw Exception();
        //不能出现非ascii
        for(int i = 0; i < cmd.length(); ++i)
            if(!isascii(cmd[i])) throw Exception();
        int num = 0;
        for(int i = 0; i < cmd.length(); ++i){
            if(cmd[i] == ' ' || cmd[i] == '\n' || cmd[i] == '\r') num++;
        }
        if(num == cmd.length()){
            cmd = "";
            return;
        }
        while(!cmd.empty() && (cmd.back() == '\r' || cmd.back() == '\n' || cmd.back() == ' '))
            cmd.pop_back();
        //1.若全为空白(多个空格)
        bool Is_Blank = 1;
        for(int i = 0; i < cmd.length(); ++i)
            if(cmd[i] != ' '){
                Is_Blank = 0;
                break;
            }
        if(Is_Blank){
            cmd = " ";
            return;
        }

        //2.不全为空白，需要处理行首行末的空格以及多余的空格
        int len = cmd.length(), l, r;
        bool *book = new bool[len];
        for(int i = 0; i < len; ++i)
            book[i] = 0;
        for(int i = 0; i < len; ++i)
            if(cmd[i] != ' '){
                l = i;
                break;
            }
        for(int i = len - 1; i >= 0; --i)
            if(cmd[i] != ' '){
                r = i;
                break;
            }
        for(int i = l + 1; i <= r; ++i)
            if(cmd[i] == ' ' && cmd[i - 1] == ' '){
                book[i] = 1;
            }
        string new_cmd = "";
        for(int i = l; i <= r; ++i){
            if(!book[i]) new_cmd += cmd[i];
        }
        delete []book;
        cmd = new_cmd;
        return;
    }
    void get_cmd(const string& cmd){
        Pars_Op.clear();
        string tmp_op = "";
        int len = cmd.length();
        for(int i = 0; i < len; ++i){
            if(cmd[i] == ' '){
                if(!tmp_op.empty()){
                    Pars_Op.push_back(tmp_op);
                    tmp_op.clear();
                }
                continue;
            }
            tmp_op.push_back(cmd[i]);
        }
        if(!tmp_op.empty()){
            Pars_Op.push_back(tmp_op);
        }
    }
    bool Check_Userid_and_Password(const string& str){
        if(str.empty() || str.length() > 30) return false;
        for(int i = 0; i < str.length(); ++i){
            if(!isalnum(str[i]) && str[i] != '_') return false;
        }
        return true;
    }
    bool Check_UserName(const string& str){
        if(str.empty() || str.length() > 30) return false;
        for(int i = 0; i < str.length(); ++i){
            if(!isprint(str[i])) return false;
        }
        return true;
    }
    bool Check_Priority(const string& str){
        if(str.length() != 1) return false;
        if(str[0] != '1' && str[0] != '3' && str[0] != '7') return false;
        return true;
    }
    bool Check_ISBN(const string& str){
        if(str.empty() || str.length() > 20) return false;
        for(int i = 0; i < str.length(); ++i){
            if(!isprint(str[i])) return false;
        }
        return true;
    }
    bool Check_Name_Author(const string& str){
        if(str.empty() || str.length() > 60) return false;
        for(int i = 0; i < str.length(); ++i){
            if(!isprint(str[i]) || str[i] == '\"') return false;
        }
        return true;
    }
    bool Check_Index_Keyword(const string& str){
        if(str.empty() || str.length() > 60) return false;
        for(int i = 0; i < str.length(); ++i){
            if(!isprint(str[i]) || str[i] == '\"' || str[i] == '|') return false;
        }
        return true;
    }
    bool Check_Quantity(const string& str){
        if(str.empty() || str.length() > 10) return false;
        for(int i = 0; i < str.length(); ++i)
            if(!isdigit(str[i])) return false;
        long long num = 0;
        for(int i = 0; i < str.length(); ++i){
            num = num * 10 + str[i] - '0';
        }
        if(num > 2147483647) return false;
        return true;
    }
    bool Check_Modify_Keyword(const string& str){
        if(str.empty() || str.length() > 60) return false;
        if(str[0] == '|' || str.back() == '|') return false;
        for(int i = 0; i < str.length(); ++i)
            if(!isprint(str[i]) || str[i] == '\"') return false;
        string single_key = "";
        set<string> key_set;
        for(int i = 0; i <= str.length(); ++i){
            if(i == str.length() || str[i] == '|'){
                if(key_set.find(single_key) != key_set.end()) return false;
                else {
                    if(!Check_Index_Keyword(single_key)) return false;
                    key_set.insert(single_key);
                    single_key.clear();
                }
            }
            else single_key += str[i];
        }
        return true;
    }
    bool Check_Price(const string& str){
        if(str.empty() || str.length() > 13) return false;
        int num = 0;
        for(int i = 0; i < str.length(); ++i){
            if(!isdigit(str[i]) && str[i] != '.') return false;
            if(str[i] == '.') num++;
        }
        //小数点不能在头或者尾
        if(str[0] == '.' || str.back() == '.') return false;
        if(num > 1) return false;
        if(str != "0" && str[0] == '0' && str[1] != '.') return false;
        return true;
    }
    bool Check_Cmd(const string& str){
        return (str == "myself" || str == "employee" || str == "finance");
    }
    bool Check_Is_Right(const string& str, vector<string>& ans){
        string tmp = "";
        for(int i = 0; i <= str.length(); ++i){
            if(i == str.length() || str[i] == '=')
                break;
            else tmp += str[i];
        }
        if(!tmp.empty()) ans.push_back(tmp);
        return (tmp == "-ISBN" || tmp == "-name" ||
                tmp == "-author" || tmp == "-keyword" ||
                tmp == "-price");
    }
    bool Check_Imf(vector<string>& ty){
        string tmp_imf = "", Par;
        for(int i = 0; i < ty.size(); ++i){
            tmp_imf.clear();
            Par = Pars_Op[i + 1];
            if(ty[i] == "-ISBN"){
                for(int j = 6; j < Par.length(); ++j) tmp_imf += Par[j];
                if(!Check_ISBN(tmp_imf)) return false;
                if(BookSystem.Is_Exist(tmp_imf)) return false;
            }
            else if(ty[i] == "-name"){
                if(Par.length() <= 8) return false;
                if(Par[6] != '\"' || Par.back() != '\"') return false;
                for(int j = 7; j < Par.length() - 1; ++j) tmp_imf += Par[j];
                if(!Check_Name_Author(tmp_imf)) return false;
            }
            else if(ty[i] == "-author"){
                if(Par.length() <= 10) return false;
                if(Par[8] != '\"' || Par.back() != '\"') return false;
                for(int j = 9; j < Par.length() - 1; ++j) tmp_imf += Par[j];
                if(!Check_Name_Author(tmp_imf)) return false;
            }
            else if(ty[i] == "-keyword"){
                if(Par.length() <= 11) return false;
                if(Par[9] != '\"' || Par.back() != '\"') return false;
                for(int j = 10; j < Par.length() - 1; ++j) tmp_imf += Par[j];
                if(!Check_Modify_Keyword(tmp_imf)) return false;
            }
            else{
                for(int j = 7; j < Par.length(); ++j) tmp_imf += Par[j];
                if(!Check_Price(tmp_imf)) return false;
            }
        }
        return true;
    }
    bool Check_Q_A(const string& str){
        if(str.length() > 60) return false;
        for(int i = 0; i < str.length(); ++i){
            if(!isprint(str[i])) return false;
        }
        return true;
    }
    bool Split_Index(const string& str, vector<string>& ans){
        string tmp = "";
        int pos;
        for(int i = 0; i <= str.length(); ++i){
            if(i == str.length() || str[i] == '='){
                pos = i;
                ans.push_back(tmp);
                break;
            }
            else tmp += str[i];
        }
        if(tmp == "-ISBN"){
            tmp.clear();
            for(int i = pos + 1; i < str.length(); ++i) tmp += str[i];
            ans.push_back(tmp);
            return true;
        }
        else if(tmp == "-name" || tmp == "-author" || tmp == "-keyword"){
            tmp.clear();
            if(str.length() < pos + 3) return false;
            if(str[pos + 1] != '\"' || str.back() != '\"') return false;
            for(int i = pos + 2; i < str.length() - 1; ++i) tmp += str[i];
            ans.push_back(tmp);
            return true;
        }
        return false;//索引不对
    }

    void run(bool& flag){//解析完命令开始运行命令
        // #基础指令
        if(Pars_Op.empty()){//仅有空白
            return;
        }
        if(Pars_Op[0] == "quit" || Pars_Op[0] == "exit"){
            if(Pars_Op.size() != 1){//格式
                throw Exception();
            }
            else{
                AccountSystem.Quit();
                flag = 1;
                return;
            }
        }

        // #账户系统指令
        if(Pars_Op[0] == "su"){
            if(Pars_Op.size() != 2 && Pars_Op.size() != 3){//格式
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[1])){//id
                throw Exception();
            }
            if(Pars_Op.size() == 3 && !Check_Userid_and_Password(Pars_Op[2])){//password
                throw Exception();
            }
            if(Pars_Op.size() == 3)
                AccountSystem.Login(Pars_Op[1], Pars_Op[2]);
            else
                AccountSystem.Login(Pars_Op[1], "");
            return;
        }
        if(Pars_Op[0] == "logout"){
            if(Pars_Op.size() != 1){//格式
                throw Exception();
            }
            AccountSystem.Logout();
            return;
        }
        if(Pars_Op[0] == "register"){
            if(Pars_Op.size() != 4){//格式
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[1])){//用户名
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[2])){//密码
                throw Exception();
            }
            if(!Check_UserName(Pars_Op[3])){//姓名
                throw Exception();
            }
            string op;
            for(int i = 0; i < Pars_Op.size(); ++i) op += Pars_Op[i], op += ' ';
            AccountSystem.Register(Pars_Op[1], Pars_Op[2], Pars_Op[3]);
            LogSystem.add_op(AccountSystem.Get_Now_User(), op);
            return;
        }
        if(Pars_Op[0] == "passwd"){
            if(Pars_Op.size() != 3 && Pars_Op.size() != 4){//格式
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 1){
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[1])){//id
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[2])){//password 1
                throw Exception();
            }
            if(Pars_Op.size() == 4 && !Check_Userid_and_Password(Pars_Op[3])){//password 2
                throw Exception();
            }

            if(Pars_Op.size() == 3){//无密码
                AccountSystem.Passwd(Pars_Op[1], Pars_Op[2], "");
            }
            else{//需要密码
                AccountSystem.Passwd(Pars_Op[1], Pars_Op[3], Pars_Op[2]);
            }
            if(AccountSystem.Get_Now_Pri() >= 3){
                string op;
                for(int i = 0; i < Pars_Op.size(); ++i) op += Pars_Op[i], op += ' ';
                LogSystem.add_op(AccountSystem.Get_Now_User(), op);
            }
            return;
        }
        if(Pars_Op[0] == "useradd"){
            if(Pars_Op.size() != 5){//格式
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 3){
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[1])){//id
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[2])){//密码
                throw Exception();
            }
            if(!Check_Priority(Pars_Op[3])){//权限
                throw Exception();
            }
            if(!Check_UserName(Pars_Op[4])){//姓名
                throw Exception();
            }
            int pri = 0;
            for(int i = 0; i < Pars_Op[3].length(); ++i){
                pri = pri * 10 + Pars_Op[3][i] - '0';
            }
            string op;
            for(int i = 0; i < Pars_Op.size(); ++i) op += Pars_Op[i], op += ' ';
            AccountSystem.Useradd(Pars_Op[1], Pars_Op[2], pri, Pars_Op[4]);
            LogSystem.add_op(AccountSystem.Get_Now_User(), op);
            return;
        }
        if(Pars_Op[0] == "delete"){
            if(Pars_Op.size() != 2){//格式
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 7){
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[1])){//id
                throw Exception();
            }
            string op;
            for(int i = 0; i < Pars_Op.size(); ++i) op += Pars_Op[i], op += ' ';
            AccountSystem.Delete(Pars_Op[1]);
            LogSystem.add_op(AccountSystem.Get_Now_User(), op);
            return;
        }
        if(Pars_Op[0] == "security"){//为当前账户设置密保问题
            //1
            if(Pars_Op.size() != 3){
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 1){
                throw Exception();
            }
            if(!Check_Q_A(Pars_Op[1]) || !Check_Q_A(Pars_Op[2])){
                throw Exception();
            }
            AccountSystem.Security(Pars_Op[1], Pars_Op[2]);
            return;
        }
        if(Pars_Op[0] == "find"){//找回密码
            //0
            if(Pars_Op.size() != 2){
                throw Exception();
            }
            if(!Check_Userid_and_Password(Pars_Op[1])){
                throw Exception();
            }
            AccountSystem.Find(Pars_Op[1]);
            return;
        }
        // #图书系统指令
        if(Pars_Op[0] == "show" && ( Pars_Op.size() == 1 || Pars_Op[1] != "finance")){
            if(Pars_Op.size() != 1 && Pars_Op.size() != 2){//格式
                throw Exception();
            }
            vector<string> par_index;
            if(Pars_Op.size() == 2 && !Split_Index(Pars_Op[1], par_index)){//更强的格式
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 1){
                throw Exception();
            }
            if(Pars_Op.size() == 2){//检查索引是否合法
                if(par_index[0] == "-ISBN" && !Check_ISBN(par_index[1]))
                    throw Exception();
                else if((par_index[0] == "-name" || par_index[0] == "-author") && !Check_Name_Author(par_index[1]))
                    throw Exception();
                else if(par_index[0] == "-keyword" && !Check_Index_Keyword(par_index[1]))
                    throw Exception();
            }
            if(Pars_Op.size() == 1){
                BookSystem.Show(AccountSystem);
                return;
            }
            else{
                BookSystem.Show(AccountSystem, Pars_Op[1]);
                return;
            }
        }
        if(Pars_Op[0] == "buy"){
            if(Pars_Op.size() != 3){//格式
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 1){
                throw Exception();
            }
            if(!Check_ISBN(Pars_Op[1])){//isbn
                throw Exception();
            }
            if(!Check_Quantity(Pars_Op[2])){//quantity
                throw Exception();
            }
            int num = 0;
            for(int i = 0; i < Pars_Op[2].length(); ++i){
                num = num * 10 + Pars_Op[2][i] - '0';
            }
            double tot;
            bool flag = BookSystem.Buy(AccountSystem, Pars_Op[1], num, tot);
            if(flag){
                if(AccountSystem.Get_Now_Pri() >= 3){
                    string op;
                    for(int i = 0; i < Pars_Op.size(); ++i) op += Pars_Op[i], op += ' ';
                    LogSystem.add_op(AccountSystem.Get_Now_User(), op);
                }
                LogSystem.add_finance(Pars_Op[0], tot);
            }
            return;
        }
        if(Pars_Op[0] == "select"){
            if(Pars_Op.size() != 2){//格式
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 3){
                throw Exception();
            }
            if(!Check_ISBN(Pars_Op[1])){//isbn
                throw Exception();
            }
            string op;
            for(int i = 0; i < Pars_Op.size(); ++i) op += Pars_Op[i], op += ' ';
            BookSystem.Select(AccountSystem, Pars_Op[1]);
            LogSystem.add_op(AccountSystem.Get_Now_User(), op);
            return;
        }
        if(Pars_Op[0] == "modify"){
            if(Pars_Op.size() <= 1){//格式
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 3){//权限
                throw Exception();
            }
            int cur_id = AccountSystem.Get_Now_ID();
            if(!cur_id){//未选中图书
                throw Exception();
            }
            vector<string> vec;
            for(int i = 1; i < Pars_Op.size(); ++i){//格式是否合法
                if(!Check_Is_Right(Pars_Op[i], vec))
                    throw Exception();
            }
            set<string> imf_ty;
            for(int i = 0; i < vec.size(); ++i){//判断是否重复
                if(imf_ty.find(vec[i]) != imf_ty.end()){
                    throw Exception();
                }
                else imf_ty.insert(vec[i]);
            }
            if(!Check_Imf(vec)){//检查附加参数
                throw Exception();
            }
            for(int i = 1; i < Pars_Op.size(); ++i){
                BookSystem.Modify(AccountSystem, Pars_Op[i]);
            }
            string op;
            for(int i = 0; i < Pars_Op.size(); ++i) op += Pars_Op[i], op += ' ';
            LogSystem.add_op(AccountSystem.Get_Now_User(), op);
            return;
        }
        if(Pars_Op[0] == "import"){
            if(Pars_Op.size() != 3){
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 3){
                throw Exception();
            }
            int cur_id = AccountSystem.Get_Now_ID();
            if(!cur_id){
                throw Exception();
            }
            if(!Check_Quantity(Pars_Op[1])){
                throw Exception();
            }
            if(!Check_Price(Pars_Op[2])){
                throw Exception();
            }
            int q;
            double p;
            q = std::stoi(Pars_Op[1]);
            p = std::stod(Pars_Op[2]);
            bool flag = BookSystem.Import(AccountSystem, q, p);
            if(flag){
                string op;
                for(int i = 0; i < Pars_Op.size(); ++i) op += Pars_Op[i], op += ' ';
                LogSystem.add_op(AccountSystem.Get_Now_User(), op);
                LogSystem.add_finance(Pars_Op[0], p);
            }
            return;
        }

        // #日志系统指令
        if(Pars_Op[0] == "report"){//myself, employee, finance
            if(Pars_Op.size() != 2){//格式
                throw Exception();
            }
            if(!Check_Cmd(Pars_Op[1])){
                throw Exception();
            }
            if(Pars_Op[1] == "myself" && AccountSystem.Get_Now_Pri() < 3){
                throw Exception();
            }
            else if(AccountSystem.Get_Now_Pri() < 7){
                throw Exception();
            }
            switch (Pars_Op[1][0]) {
                case 'm' :{
                    LogSystem.report_myself(AccountSystem.Get_Now_User());
                    break;
                }
                case 'e' :{
                    vector<string> user_id;
                    AccountSystem.Find_All_User(user_id);
                    LogSystem.report_employ(user_id);
                    break;
                }
                case 'f' :{
                    LogSystem.report_finance();
                    break;
                }
            }
            return;
        }
        if(Pars_Op[0] == "show" && Pars_Op[1] == "finance"){
            if(Pars_Op.size() != 2 && Pars_Op.size() != 3){
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 7){
                throw Exception();
            }
            if(Pars_Op.size() == 2){
                LogSystem.show_fin();
            }
            else{
                if(!Check_Quantity(Pars_Op[2])){
                    throw Exception();
                }
                int t = std::stoi(Pars_Op[2]);
                LogSystem.show_fin(t);
            }
            return;
        }
        if(Pars_Op[0] == "log"){
            if(Pars_Op.size() != 1){
                throw Exception();
            }
            if(AccountSystem.Get_Now_Pri() < 7){
                throw Exception();
            }
            vector<string> user_id;
            AccountSystem.Find_All_User(user_id);
            LogSystem.log(user_id);
            return;
        }
        //仍然不返回说明指令不合法
        throw Exception();
    }
};

#endif //BOOKSTORE_COMMAND_H
