#include <iostream>
#include <stdlib.h>
#include <time.h> 
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <deque>
using namespace std;

int reference_string[24] = {0, 2, 1, 3, 5, 4, 6, 3, 7, 4, 7, 3, 3, 5, 5, 3, 1, 1, 1, 7, 2, 3, 4, 1};
// FIFO: use a queue to store reference, pop out the head when space is full
// LRU: use a queue to store reference, check whether a new ref in queue, if in the queue, get the index of the ref, erase it and add to the head, update the 


int check(deque<int> queue, int val){
    for(int i =0; i < queue.size();i++){
        if (abs(queue[i])==val){
            return i;
        }
    }
    return -1;
}

void queue_print(deque<int> cur, deque<int> removed, int cur_num=-1, int result=0){
    if (cur_num<=-1){
        cout<< string( 13, ' ' );
    }
    else{
        cout<<" current num: "+to_string(cur_num)+ "  ";
        //cout<<to_string(cur_num);
    }
    cout<<" cur pages: ";
    for(int i =0;i<4;i++){
        if (i>= cur.size()){
            cout<< "   ";
        }
        else{
            cout<<" "+to_string(abs(cur[i])) +" ";
        }
    }
    cout<<" recent removed: ";
    for(int i =0;i<4;i++){
        if (i>= removed.size()){
            cout<< "   ";
        }
        else{
            cout<<" "+to_string(abs(removed[i])) +" ";
        }
    }
    if (result>=0){
        cout<< " no page fault"<<endl;
    }
    else{
        cout<< " has page fault"<<endl;
    }

}

void FIFO(){
    deque<int> cur_pages;
    deque<int> removed;
    queue_print(cur_pages,removed);
    for (int i =0; i<24; i++){
        int cur  = reference_string[i];
        int result = check(cur_pages,cur);
        if ( cur_pages.size()<4){
                cur_pages.push_back(cur);
                queue_print(cur_pages,removed, cur, result);
                continue;
            }

        if  (result<0){
            int cur_removed = cur_pages.front();
            cur_pages.pop_front();
            cur_pages.push_back(cur);
            if (removed.size()<4){
                removed.push_back(cur_removed);
            }
            else{
                removed.pop_front();
                removed.push_back(cur_removed);
            }
        }
        queue_print(cur_pages,removed, cur, result);

    }
}


void LRU(){
    deque<int> cur_pages;
    deque<int> removed;
    queue_print(cur_pages,removed);
    for (int i =0; i<24; i++){
        int cur  = reference_string[i];
        int result = check(cur_pages,cur);
        if ( cur_pages.size()<4){
                cur_pages.push_back(cur);
                queue_print(cur_pages,removed, cur, result);
                continue;
            }

        if  (result<0){
            
            int cur_removed = cur_pages.front();
            cur_pages.pop_front();
            
            cur_pages.push_back(cur);
            if ( removed.size()<4){
                removed.push_back(cur_removed);
            }
            else{
                removed.pop_front();
                removed.push_back(cur_removed);
            }
        }
        else{
            deque<int>::iterator it = cur_pages.begin() ;
            it += result;
            cur_pages.erase(it);
            cur_pages.push_back(cur);

        }
        queue_print(cur_pages,removed, cur, result);

    }
}



void second(){
    deque<int> cur_pages;
    deque<int> removed;
    queue_print(cur_pages,removed);
    for (int i =0; i<24; i++){
        int cur  = reference_string[i];
        int result = check(cur_pages,cur);
        if ( cur_pages.size()<4){
                cur_pages.push_back(cur);
                queue_print(cur_pages,removed, cur, result);
                continue;
            }

        if  (result<0){
            
            int cur_removed = cur_pages.front();
            cur_pages.pop_front();
            
            cur_pages.push_back(cur);
            if ( removed.size()<4){
                removed.push_back(cur_removed);
            }
            else{
                removed.pop_front();
                removed.push_back(cur_removed);
            }
        }
        else{
            deque<int>::iterator it = cur_pages.begin() ;
            it += result;
            cur_pages.erase(it);
            cur_pages.push_back(cur);

        }
        queue_print(cur_pages,removed, cur, result);

    }
}
//     deque<int> cur_pages;
//     deque<int> signs; // sign for whether referenced
//     deque<int> removed;
//     queue_print(cur_pages,removed);
//     int cur_pointer = 0;
//     for (int i =0; i<24; i++){
//         int cur_num  = reference_string[i];
//         int result = check(cur_pages,cur_num);
//         if ( cur_pages.size()<4){
//                 cur_pages.push_back(cur_num);
//                 signs.push_back(0);
//                 queue_print(cur_pages,removed, cur_num, result);
//                 continue;
//             }
//         if  (result<0){
//             while(signs[cur_pointer]==1){
//                 signs[cur_pointer]=0;
//                 cur_pointer ++;
//                 if (cur_pointer>= signs.size()){
//                     cur_pointer = 0;
//                 }
//             }
//             int cur_removed = cur_pages[cur_pointer];


//             deque<int>::iterator it1 = cur_pages.begin() ;
//             deque<int>::iterator it2 = signs.begin() ;
//             it1 += cur_pointer;
//             it2 += cur_pointer;
//             cur_pages.erase(it1);
//             //cout<<cur_num;
//             it1 = cur_pages.insert(it1,2);
//             cur_pages.insert(it1,20);
//             cur_pages.insert(it1,200);
//             signs.erase(it2);
//             signs.insert(it2,0);
            
//             if (removed.size()<4){
//                 removed.push_back(cur_removed);
//             }
//             else{
//                 removed.pop_front();
//                 removed.push_back(cur_removed);
//             }
//         }
//         else{
//             signs[result] = 1;

//         }
//         queue_print(cur_pages,removed, cur_num, result);

//     }
// }


int main(){
    cout<< "choosing algorithm 0:FIFO, 1:LRU, 2:second chance";
    int algo_id;
    cin >> algo_id;
    if (algo_id==0){
        FIFO();
    }
    else if(algo_id==1){
        LRU();
    }
    else{
        second();
    }
    return 0;
}