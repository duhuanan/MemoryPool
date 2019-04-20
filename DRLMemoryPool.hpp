//
//  DRLMemoryPool.hpp
//  CalendarPickView
//
//  Created by duha on 2019/4/20.
//  Copyright © 2019 www.microlink.im. All rights reserved.
//

#ifndef DRLMemoryPool_hpp
#define DRLMemoryPool_hpp

#include <stdio.h>
#include <unordered_map>
#include <mutex>
#include <list>

using namespace std;

namespace DRLMemoryPool {
    static mutex m_mutex;
    static unordered_map<int, list<char *> *> pool;
    
    class MemoryPool {
    public:
        MemoryPool *next;
        
        static void *operator new(size_t size) {
            unique_lock<mutex> lock(m_mutex);
            int newSize = (int)size;
            char *p;
            unordered_map<int, list<char *> *>::iterator it = pool.find(newSize);
            list<char *> *memoryList = NULL;
            
            if(it == pool.end()) {
                memoryList = new list<char *>();
                pool[newSize] = memoryList;
            }else {
                memoryList = it->second;
            }
            
            if(memoryList->empty()) {
                static int chunkSize = 500;
                p = new char[chunkSize * newSize];
                char *lastPtr = p + newSize * (chunkSize - 1);
                
                for(; p != lastPtr; p += newSize) {
                    memoryList->push_back(p);
                }
            }
            
            p = memoryList->front();
            memoryList->pop_front();
            
            return p;
        }
        
        static void operator delete(void *p, size_t size) {
            unique_lock<mutex> lock(m_mutex);
            int newSize = (int)size;
            unordered_map<int, list<char *> *>::iterator it = pool.find(newSize);
            
            if(it != pool.end()) {
                it->second->push_front((char *)p);
            }
        }
    };
}

#endif /* DRLMemoryPool_hpp */
