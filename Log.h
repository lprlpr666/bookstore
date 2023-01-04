//
// Created by zbl on 2021/12/26.
//

#ifndef BOOKSTORE_LOG_H
#define BOOKSTORE_LOG_H

#include <iostream>
#include <iomanip>

struct FinanceLog{//一个对象表示一次交易
    bool ty = 0;//表示buy or import 0表示buy(收入)， 1表示import(支出)
    double tot = 0;//正数，该笔交易额
    FinanceLog() = default;
    FinanceLog(const bool& type, const double& num):ty(type), tot(num){}
    FinanceLog& operator = (const FinanceLog& rhs){
        ty = rhs.ty;
        tot = rhs.tot;
        return *this;
    }
    friend std::ostream & operator << (std::ostream& ou, const FinanceLog& tmp_fin){
        if(tmp_fin.ty){
            ou << "import ";
        }
        else ou << "buy ";
        ou << tmp_fin.tot;
        return ou;
    }
};
struct EmployeeLog{//一个对象表示一次操作
    char op[256];
    EmployeeLog(){
        for(int i = 0; i < 256; ++i) op[i] = '\0';
    }
    EmployeeLog(const string& str){
        strcpy(op, str.c_str());
    }
    EmployeeLog& operator = (const EmployeeLog& rhs){
        strcpy(op, rhs.op);
    }
};
class Log_System{
private:
    MemoryRiver<FinanceLog> Fin_Log_Data;
    MemoryRiver<EmployeeLog> Em_Log_Data;
    BlockList EmIndex;
    int sizeofF = sizeof(FinanceLog);
public:
    Log_System():Fin_Log_Data("fin_log_data"), Em_Log_Data("em_log_data"), EmIndex("em_index"){}
    void show_fin(){
        int tot_time;
        double income = 0, outcome = 0;
        FinanceLog cur;
        Fin_Log_Data.get_info(tot_time, 1);
        for(int i = 8; i <= 8 + (tot_time - 1) * sizeofF; i += sizeofF){
            Fin_Log_Data.read(cur, i);
            if(!cur.ty){
                income += cur.tot;
            }
            else{
                outcome += cur.tot;
            }
        }
        cout << fixed << std::setprecision(2)<< "+ " << income << " - " << outcome << endl;
    }
    void show_fin(const int& num){
        if(!num){
            cout << endl;
            return;
        }
        int tot_time;
        double income = 0, outcome = 0;
        FinanceLog cur;
        Fin_Log_Data.get_info(tot_time, 1);
        if(num > tot_time){
            cout << "Invalid\n";
            return;
        }
        for(int i = 8 + (tot_time - 1) * sizeofF; i >= 8 + (tot_time - num) * sizeofF; i -= sizeofF){
            Fin_Log_Data.read(cur, i);
            if(!cur.ty){
                income += cur.tot;
            }
            else{
                outcome += cur.tot;
            }
        }
        cout << fixed << std::setprecision(2)<< "+ " << income << " - " << outcome << endl;
    }
    void add_finance(const string& type, const double& num){
        FinanceLog cur;
        if(type == "buy"){
            cur = FinanceLog(0, num);
            Fin_Log_Data.write(cur);
        }
        else{
            cur = FinanceLog(1, num);
            Fin_Log_Data.write(cur);
        }
    }
    void add_op(const string& tmp_ID, const string& tmp_op){
        EmployeeLog cur = EmployeeLog(tmp_op);
        int pos = Em_Log_Data.write(cur);
        EmIndex.Insert(Node(tmp_ID, pos));
    }
    void report_myself(const string& tmp_ID){
        vector<int> index;
        EmIndex.Find(tmp_ID, index);
        cout << "-------------------- report " << tmp_ID << " --------------------" << endl << endl;
        if(index.empty()){
            cout << "Have no operation log." << endl << endl;
            return;
        }
        else{
            EmployeeLog cur;
            for(int i = 0; i < index.size(); ++i){
                Em_Log_Data.read(cur, index[i]);
                cout << cur.op << endl;
            }
        }
        cout << "-------------------- end --------------------" << endl << endl;
    }
    void report_employ(vector<string> index){
        cout << "******************** report employ ********************" << endl << endl;
        for(int i = 0; i < index.size(); ++i){
            report_myself(index[i]);
        }
        cout << "******************** end ********************" << endl << endl;
    }
    void report_finance(){
        int tot_time;
        FinanceLog cur;
        Fin_Log_Data.get_info(tot_time, 1);
        cout << "~~~~~~~~~~~~~~~~~~~~ report finance ~~~~~~~~~~~~~~~~~~~~" << endl << endl;
        for(int i = 8; i <= 8 + (tot_time - 1) * sizeofF; i += sizeofF){
            Fin_Log_Data.read(cur, i);
            cout << cur << endl;
        }
        cout << "~~~~~~~~~~~~~~~~~~~~ end ~~~~~~~~~~~~~~~~~~~~" << endl << endl;
    }
    void log(vector<string> tmp_id){
        report_employ(tmp_id);
        report_finance();
    }
};

#endif //BOOKSTORE_LOG_H
