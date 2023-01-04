#ifndef BOOKSTORE_BLOCKS_H
#define BOOKSTORE_BLOCKS_H
#include<bits/stdc++.h>
const int si=sizeof(int);
using namespace std;

template<class T, int info_len = 2>
class MemoryRiver {
    int sizeT = sizeof(T);
    fstream file;
    string file_name;
public:
    MemoryRiver() = default;
    MemoryRiver(const string& s) : file_name(s) {
        initialise();
    }
    void getinfo(int& index, int b) ;
    void writeinfo(int index, int b) ;
    int write(T &t) ;
    void update(T &t, const int index) ;
    void read(T &t, const int index) ;
    void Delete(int index);
    void initialise(string s = "") {
        if (s != "") file_name = s;
        file.open(file_name, std::fstream::in | std::fstream::out);
        if(!file){
            file.open(file_name, std::ofstream::out);
            int tmp = 0;
            for (int i = 0; i < info_len; ++i)
                file.write(reinterpret_cast<char *>(&tmp), si);
        }
        file.close();
    }
};

template<class T, int info_len>
void MemoryRiver<T, info_len>::Delete(int index) {
    int del_head;
    getinfo(del_head, 2);
    file.open(file_name);
    file.seekp(index);
    file.write(reinterpret_cast<char*>(&del_head), si);
    del_head = index;
    file.close();
    writeinfo(del_head, 2);
    return;
}

template<class T, int info_len>
void MemoryRiver<T, info_len>::read(T &t, const int index) {
    file.open(file_name);
    file.seekg(index);
    file.read(reinterpret_cast<char*>(&t), sizeT);
    file.close();
    return;
}

template<class T, int info_len>
void MemoryRiver<T, info_len>::update(T &t, const int index) {
    file.open(file_name);
    file.seekp(index);
    file.write(reinterpret_cast<char*>(&t), sizeT);
    file.close();
    return;
}

template<class T, int info_len>
int MemoryRiver<T, info_len>::write(T &t) {
    int index,num_T, del_head;
    getinfo(num_T, 1);
    getinfo(del_head, 2);
    if(!del_head)//在文件尾部加
        index = info_len * si + num_T * sizeT;
    else {//在释放掉的空间加
        index = del_head;
        file.open(file_name);
        file.seekg(del_head);
        file.read(reinterpret_cast<char*>(&del_head), si);
        file.close();
    }
    file.open(file_name);
    file.seekp(index);
    file.write(reinterpret_cast<char*>(&t), sizeT);
    file.close();
    writeinfo(++num_T, 1);
    writeinfo(del_head, 2);//更新 del_head 的值
    return index;
}

template<class T, int info_len>
void MemoryRiver<T, info_len>::writeinfo(int index, int b) {
    if (b > info_len) return;
    file.open(file_name);
    file.seekp((b - 1) * si);
    file.write(reinterpret_cast<char*>(&index), si);
    file.close();
    return;
}

template<class T, int info_len>
void MemoryRiver<T, info_len>::getinfo(int &index, int b) {
        if (b > info_len) return;
        file.open(file_name);
        file.seekg((b - 1) * si);
        file.read(reinterpret_cast<char*>(&index), si);
        file.close();
        return;
}

class Node{
private:
    int value;
    char key[65];
public:
    friend class Block;
    friend class BlockList;
    Node() : key(""){}
    Node(const string& str, const int& val){
        strcpy(key, str.c_str());
        value = val;
    }
    bool operator == (const Node& tmp) const {
        return value == tmp.value && !strcmp(key, tmp.key);
    }
    bool operator != (const Node& tmp) const {
        return !(*this == tmp);
    }
    bool operator < (const Node& tmp) const {
        if(!strcmp(key, tmp.key)) return value < tmp.value;
        return strcmp(key, tmp.key) < 0 ? true : false;
    }
    bool operator > (const Node& tmp) const {
        return !(*this < tmp) && *this != tmp;
    }
    bool operator <= (const Node& tmp) const {
        return *this < tmp || *this == tmp;
    }
    bool operator >= (const Node& tmp) const {
        return *this > tmp || *this == tmp;
    }
};

class Block{//一个块 头部:重要信息 尾部:Node 数组
private:
    //存储该块的前驱,后继,该块中数据的数量, 该块的起始位置
    int pre_index, nxt_index, size, pos;
    //存储该块中最大和最小的pair
    Node array[(1000 << 1) + 5];
public:
    friend class BlockList;
    Block() : pre_index(0), nxt_index(0), size(0), pos(-1){}
    Block(const Block& tmp){
        *this = tmp;
    }
    Block &operator = (const Block& tmp){
        pre_index = tmp.pre_index;
        nxt_index = tmp.nxt_index;
        size = tmp.size;
        pos = tmp.pos;
        memcpy(array, tmp.array, size + 1);
        return *this;
    }
};

class BlockList{
private:
    //将一个块状链表与一个 MemoryRiver 类关联
    MemoryRiver<Block> Blocks;
public:
    BlockList() = default;
    BlockList(const string& name):Blocks(name){}
    void Insert(const Node& tmp){
        int num, fpos;
        Blocks.getinfo(num, 1);//获取块数;
        fpos = 8;
        if(!num){//第一个插入的元素,没有可以被读取的块,于是创建一个新的块
            Block new_block;
            new_block.size = 1;
            new_block.array[0] = tmp;
            new_block.pos = Blocks.write(new_block);//将该块写入文件
            Blocks.update(new_block, new_block.pos);
        }
        else{//读取所在的块,注意边界情况的处理,比如说:最小的pair, 与最大的pair
            Block tmp_block;
            int cnt = 0;
            for(int i = fpos; i; i = tmp_block.nxt_index){
                cnt++;
                Blocks.read(tmp_block, i);
                if(!tmp_block.size){//对于空块特判即可
                    if(cnt == num){//最后一块的情况
                        tmp_block.array[0] = tmp;
                        tmp_block.size++;
                        Blocks.update(tmp_block, i);
                        return;
                    }
                    else{
                        int nxt_pos = tmp_block.nxt_index;
                        Block nxt_block;
                        Blocks.read(nxt_block, nxt_pos);
                        if(nxt_block.size && tmp < nxt_block.array[0]){
                            tmp_block.array[0] = tmp;
                            tmp_block.size++;
                            Blocks.update(tmp_block, i);
                            return;
                        }
                        continue;
                    }
                }
                if( cnt == num || tmp < tmp_block.array[tmp_block.size - 1]){//读到这个Block里面
                    int it = std::lower_bound(tmp_block.array, tmp_block.array + tmp_block.size, tmp) - tmp_block.array;
                    for(int j = tmp_block.size; j > it; --j){
                        tmp_block.array[j] = tmp_block.array[j - 1];
                    }
                    tmp_block.array[it] = tmp;
                    tmp_block.size++;
                    Blocks.update(tmp_block, i);
                    break;
                }
            }
            if(tmp_block.size >= 1500){//触发分裂
                Split(tmp_block.pos);
            }
        }
    }
    void FindAll(vector<int>& ans){
        int num, fpos = 8;
        Blocks.getinfo(num, 1);
        Block tmp_block;
        for(int i = fpos; i; i = tmp_block.nxt_index){
            Blocks.read(tmp_block, i);
            for(int j = 0; j < tmp_block.size; ++j){
                ans.push_back(tmp_block.array[j].value);
            }
        }
    }
    void Find(const string& TmpK, vector<int>& ans){
        int num, fpos = 8;
        Blocks.getinfo(num, 1);
        Block tmp_block;
        for(int i = fpos; i;){
            Blocks.read(tmp_block, i);
            if(!tmp_block.size){
                i = tmp_block.nxt_index;
                continue;
            }
            if(TmpK < tmp_block.array[0].key) break;//太大,则break;
//            cout << "Hello : in Blocks.h 258 " << tmp_block.size << endl;
            if(TmpK > tmp_block.array[tmp_block.size - 1].key){//太小,则continue;
                i = tmp_block.nxt_index;
                continue;
            }
            for(int j = 0; j < tmp_block.size; ++j){
                if(TmpK == tmp_block.array[j].key){
                    ans.push_back(tmp_block.array[j].value);
                    continue;
                }
                if(TmpK < tmp_block.array[j].key) break;
            }
            i = tmp_block.nxt_index;
        }
    }
    void Delete(const Node& tmp){//先找到该条目,找不到就返回
        int num, fpos = 8;
        Blocks.getinfo(num, 1);
        Block tmp_block;
        for(int i = fpos; i; i = tmp_block.nxt_index){
            Blocks.read(tmp_block, i);
            if(!tmp_block.size){
                continue;
            }
            if(tmp < tmp_block.array[0] || tmp > tmp_block.array[tmp_block.size - 1]){//不在该块中
                continue;
            }
            else{//可能在该块中
                for(int j = 0; j < tmp_block.size; ++j){
                    if(tmp_block.array[j] == tmp){
                        tmp_block.size--;
                        for(int k = j; k < tmp_block.size; ++k)//依次修改数组元素
                            tmp_block.array[k] = tmp_block.array[k + 1];
                        Blocks.update(tmp_block, i);
                        if(tmp_block.size <= 500) Merge(tmp_block.pos);
                        break;
                    }
                    else if(tmp_block.array[j] > tmp) break;
                }
                continue;
            }
        }
    }
    void Split(const int& pos){//位于pos处的块分裂(向后分裂,即分裂出的块是被分裂的后继)
        Block tmp_block, new_block;
        int org_size, num;
        Blocks.getinfo(num, 1);
        Blocks.read(tmp_block, pos);
        org_size = tmp_block.size;
        tmp_block.size /= 2;
        new_block.size = org_size - tmp_block.size;
        new_block.pre_index = tmp_block.pos;
        new_block.nxt_index = tmp_block.nxt_index;
        for(int i = 0; i < new_block.size; ++i)
            new_block.array[i] = tmp_block.array[i + tmp_block.size];
        new_block.pos = Blocks.write(new_block);
        tmp_block.nxt_index = new_block.pos;
        Blocks.update(tmp_block, tmp_block.pos);
        Blocks.update(new_block, new_block.pos);
    }
    void Merge(const int& pos){//将位于pos处的块与其pre合并(向前合并,即与其前驱合并)
        if(pos == 8) return; //第一个块不merge
        Block tmp_block, pre_block, nxt_block;
        int num, org_size;
        Blocks.getinfo(num, 1);
        Blocks.read(tmp_block, pos);
        Blocks.read(pre_block, tmp_block.pre_index);
        if(tmp_block.nxt_index){//如果不是尾结点
            Blocks.read(nxt_block, tmp_block.nxt_index);
            nxt_block.pre_index = pre_block.pos;
            Blocks.update(nxt_block, nxt_block.pos);
        }
        pre_block.nxt_index = tmp_block.nxt_index;
        org_size = pre_block.size;
        pre_block.size += tmp_block.size;
        for(int i = org_size; i < pre_block.size; ++i){
            pre_block.array[i] = tmp_block.array[i - org_size];
        }
        Blocks.update(pre_block, pre_block.pos);
        Blocks.Delete(tmp_block.pos);
        if(pre_block.size >= 1500)
            Split(pre_block.pos);
    }
};

#endif //BOOKSTORE_BLOCKS_H
