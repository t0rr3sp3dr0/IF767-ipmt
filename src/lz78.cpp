#include "lz78.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

const int ALPHABET_SIZE = 256;

struct node {
    node* children[ALPHABET_SIZE]; 
    int index;
    int cur_trie; 

    
    node(int cur_trie){
        //memset()
        std::memset(this->children, 0, sizeof(this->children));
        this->index = -1;
        this->cur_trie = cur_trie;
        //fill(children,children+ALPHABET_SIZE, nullptr);
    }
}; 

node* root; 



inline void insert(const std::string &key, int idx, int cur_trie) {
    auto aux = root;
    int index; 
    for (auto ch : key) {
        index = ch%ALPHABET_SIZE;
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
inline int search(const std::string &key, int cur_trie) {
    auto aux = root;
    int index; 
    int i = 1; 
    int last_index = 0; 
    for (auto ch : key) {
        index = ch%ALPHABET_SIZE;
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

    char char0 = 0;
    int cur_trie = 0; 
    int vec_size = 0; 
    int dict_index = 1;
    int code = 0; 
    std::string prefix = ""; 
    int size = 16; 
    int index_max = pow(2, size); 
    int last_index = 0; 
    int idx = 0; 
    

    insert("", 0, cur_trie); 
    

    for (auto &c: in){

        if(dict_index == index_max){
            dict_index = 1;                 
            cur_trie++; 
            insert("", 0, cur_trie); 
        }

        idx = search(prefix+c, cur_trie); 

        if(idx > 0){
            prefix += c;   
            last_index = idx; 
        }else{
            if(prefix.empty()){
                code = 0; 
            }else{
                code = last_index;
            }

            insert(prefix+c, dict_index, cur_trie);
            
            if(size == 16){
                if (code <= 255) {
                    out.write((char*)&char0,sizeof(char));
                    out.write((char*)&code,sizeof(char));
                } else {
                    char byte1 = code & 255;
                    char byte2 = code >> 8;
                    out.write((char*)&byte2,sizeof(char));
                    out.write((char*)&byte1,sizeof(char));

                }
                out.write((char*)&c,sizeof(char));
            }  

            dict_index++; 
            prefix="";
        }
    }
    if(!prefix.empty()){
        code = search(prefix, cur_trie); 
        
        if (code <= 255) {
            out.write((char*)&char0,sizeof(char));
            out.write((char*)&code,sizeof(char));
        } else {
            char byte1 = code & 255;
            char byte2 = code >> 8;
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
    std::string dict [65536]; 
    int dict_index = 1;
    int aux, aux2; 
    int num = 0; 
    std::string aux_text = ""; 
    int index_max = pow(2, size); 
    char text_in_num1, text_in_num2, text_in_char;
    std::string carc=""; 
    char *_txt = const_cast<char *>(txt.begin());
    std::stringstream cc; 

    while(!file.eof()){

        file.read((char*)&text_in_num1, sizeof(char)); 
        file.read((char*)&text_in_num2, sizeof(char));
        
        aux = ( 255 & int(text_in_num1));  
        aux <<=8;
	    num |= aux;
	    aux2 = ( 255 &  int(text_in_num2));
	    num = aux | aux2;
        
        if(num == 0) aux_text = ""; 
        else aux_text = dict[num]; 

        if(file.eof())
            break;

        file.read((char*)&text_in_char, sizeof(char));
        carc =  aux_text+(char)text_in_char;
        dict[dict_index] = carc;
        aux_text.append(carc); 

        cc<<carc;  

        dict_index++;
        text_in_char = '\0';
        if(dict_index == index_max) dict_index = 1;
    }
    txt = cc.str();
    
    //outfile.close();
}

