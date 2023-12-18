#pragma once
#ifndef __HEADER_LINKED_HASHMAP__
#define __HEADER_LINKED_HASHMAP__
    struct BinaryNode{

            struct BinaryNode* nexts[2];
            void* data;
            
            struct BinaryNode* parent;
        };
    struct LHashMap{

        struct BinaryNode* root_node;
    };
    unsigned char* getHash(int value){
        const int total_n = sizeof(int)*8;
        unsigned int value2 = (unsigned int) value;
        unsigned char key[16] = {0,};

        for(int i=0;i<total_n;i+=2){
            key[i] = (value2>>i)&0x3;
        }

        return key;
    }
    void innerSearch(struct BinaryNode* node,unsigned char* key_seq,int hop){
        unsigned char key = key_seq[16-hop];
        
    }
    void* get(int block_number){
        void* value = NULL;
        unsigned char* key = getHash(block_number);

        return value;
    }


#endif