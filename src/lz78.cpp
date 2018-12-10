#include "lz78.h"

#include <cmath>
#include <cassert>
#include <climits>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

const int ALPHABET_SIZE = 256;

struct node {
    node* children[ALPHABET_SIZE];
    int index;
    int cur_trie;

    node(int cur_trie){
        std::memset(this->children, 0, sizeof(this->children));
        //std::fill(children,children+ALPHABET_SIZE, nullptr);
        this->index = -1;
        this->cur_trie = cur_trie;
        //
    }
};

node* root = new node(0);



inline void insert(const std::vector<char> &key, int idx, int cur_trie) {
    //std::cout<<"iniciando o root";
    //std::cout<<"iniciando o root";
    auto aux = root;
    int index;
    for (auto ch : key) {
        index = static_cast<size_t>(static_cast<unsigned char>(ch));
        if (aux->children[index] == nullptr) {
            aux->children[index] = new node(cur_trie);
        }else if(cur_trie != aux->children[index]->cur_trie){
            aux->children[index]->cur_trie = cur_trie;
        }
        aux = aux->children[index];
    }
    aux->index = idx;
    aux->cur_trie = cur_trie;
}
inline int search(const std::vector<char> &key, int cur_trie) {
    auto aux = root;
    int index;
    int i = 1;
    int last_index = 0;
    for (auto ch : key) {
        index = static_cast<size_t>(static_cast<unsigned char>(ch));
        //std::cout<<ch<<" "<<index<<std::endl;
        if (aux->children[index] == nullptr || aux->children[index]->cur_trie != cur_trie) {
            return -1;
        }
        aux = aux->children[index];
        last_index = aux->index;
    }
    return last_index;
}

//static inline void compress(const ::string_view &txt, std::ostream &out) {

void lz78::compress (const ::string_view &in,std::ostream &out){
    //std::cout<<"start compressing"<<std::endl;

    char char0 = 0;

    int cur_trie = 0;
    int vec_size = 0;
    int dict_index = 1;

    int code = 0;
    std::vector<char> prefix;

    int size = 16;
    int index_max = pow(2, size);
    int last_index = 0;
    int idx = 0;

    std::vector<char> empty;

    insert(empty, 0, cur_trie);


    size_t qwe = 0;
    for (auto &c: in){
//            std::cout << (size_t) c << std::endl;
        qwe++;
        //std::cout<<"start compressing";
        if(dict_index == index_max){
            dict_index = 1;
            cur_trie++;
            insert(empty, 0, cur_trie);
        }

        prefix.push_back(c);
        idx = search(prefix, cur_trie);
        prefix.pop_back();
        //std::cout<<idx<<std::endl;
        if(idx > 0){
            prefix.push_back(c);
            last_index = idx;
        }else{
            if(prefix.empty()){
                code = 0;
            }else{
                code = last_index;
            }


            prefix.push_back(c);
            insert(prefix, dict_index, cur_trie);
            prefix.pop_back();

            if(size == 16){
                if (code <= 255) {
                    unsigned char qwe = code;
                    out.write((char*)&char0,sizeof(char));
                    out.write((char*)&qwe,sizeof(char));
                } else {
                    unsigned char byte1 = code & 255;
                    unsigned char byte2 = code >> 8;
                    out.write((char*)&byte2,sizeof(char));
                    out.write((char*)&byte1,sizeof(char));

                }
                out.write((char*)&c,sizeof(char));
            }

            dict_index++;
            prefix.resize(0);
        }
    }

    if(!prefix.empty()){
        code = search(prefix, cur_trie);


        if (code <= 255) {
            unsigned char qwe = code;
            out.write((char*)&char0,sizeof(char));
            out.write((char*)&qwe,sizeof(char));
        } else {
            unsigned char byte1 = code & 255;
            unsigned char byte2 = code >> 8;
            out.write((char*)&byte2,sizeof(char));
            out.write((char*)&byte1,sizeof(char));

        }
    }
    //out.close();
}

//static inline void decompress(std::istream &in, ::string_view &txt) {
void lz78::decompress (std::istream &file, ::string_view &txt){
    //std::ofstream outfile ("new.txt");
    //std::fstream file ("exemple", std::ios::in | std::ios::out | std::ios::binary);
    int size = 16;
    size_t index_max = pow(2, size);
    std::vector<char> dict [index_max];
    size_t dict_index = 1;
    int aux, aux2;
    size_t num = 0;
    std::vector<char> aux_text;

    unsigned char text_in_num1, text_in_num2; char text_in_char;
    std::vector<char> carc;
    char *_txt = const_cast<char *>(txt.begin());
    std::stringstream cc;
    int i = 0;


    char *s = const_cast<char *>(txt.begin());

    size_t index = 0;
    while (index < txt.length()) {

        file.read((char*)&text_in_num1, sizeof(char));
        file.read((char*)&text_in_num2, sizeof(char));

//        aux = ( 255 & int(text_in_num1));
//        aux <<=8;
//        num |= aux;
//        aux2 = ( 255 &  int(text_in_num2));
//        num = aux | aux2;

num = 0;
num |= text_in_num1;
num <<= 8;
num |= text_in_num2;

        if(num == 0) aux_text.resize(0);
        else aux_text = dict[num];

        if(file.eof()) {
            for (auto &e : aux_text)
                s[index++] = e;
            break;
        }

        file.read((char*)&text_in_char, sizeof(char));
        aux_text.push_back(text_in_char);
        carc =  aux_text;
        aux_text.pop_back();
        dict[dict_index] = carc;

        for (auto &e : carc)
            s[index++] = e;

        if (index > txt.length())
            file.putback(text_in_char);

        dict_index++;
        text_in_char = '\0';
        if(dict_index == index_max) dict_index = 1;
    }

//    std::cout<<cc.str();

//    std::string out = cc.str();
//    std::cout << out.length() << std::endl;
    std::cout << txt.length() << '\t' << index << std::endl;
//    assert(out.length() == txt.length());
//    for (auto &c : out)
//        s[index++] = c;

//    txt = cc.str();
    //outfile.close();
}
