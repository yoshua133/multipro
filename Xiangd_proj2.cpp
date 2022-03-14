#include <iostream>
#include <fstream>
#include <stdlib.h>  
#include <string>
#include <sstream> 
#include <ctime>
#include <deque>
using namespace std;
void generate_file(){
    srand (0);//设置种子
    int req ; 
    ofstream myfile;
    myfile.open ("Xiangd_proj2_input.txt");
    for(int i=1; i<101;i++){
        req = rand() % 16+ 0;
        myfile << to_string(req) +' ';
        if (i%20==0){
            myfile << '\n';
        }
    }
    myfile.close();
}


int check(deque<int> queue, int val){
    for(int i =0; i < queue.size();i++){
        if (abs(queue[i])==val){
            return i;
        }
    }
    return -1;
}

void queue_print(ofstream &myfile, int frame_size, deque<int> cur, deque<int> removed, int cur_num=-1, int result=0){
    time_t now = time(0);
    char* dt = ctime(&now);
    myfile << dt<< "  ";
    if (cur_num<=-1){
        cout<< string( 13, ' ' );
    }
    else{
        cout<<" current num: "+to_string(cur_num)+ "  ";
        myfile << " current reference: "+to_string(cur_num)+ "  ";
        //cout<<to_string(cur_num);
    }
    cout<<" cur pages: ";
    for(int i =0;i<frame_size;i++){
        if (i>= cur.size()){
            cout<< "   ";
        }
        else{
            cout<<" "+to_string(abs(cur[i])) +" ";
        }
    }
    cout<<" recent removed: ";
    for(int i =0;i<frame_size;i++){
        if (i>= removed.size()){
            cout<< "   ";
        }
        else{
            cout<<" "+to_string(abs(removed[i])) +" ";
        }
    }
    if (result>=0){
        cout<< " no page fault"<<endl;
        myfile << " no page fault"<<endl;
    }
    else{
        cout<< " has page fault"<<endl;
        myfile << " has page fault"<<endl;
    }

}

void FIFO(ofstream &myfile, int frame_size,  deque<int> reference_string){
    int length =  reference_string.size();
    deque<int> cur_pages;
    deque<int> removed;
    int num_page_failed = 0;
    myfile << "FIFO "<<endl;
    //queue_print(frame_size, cur_pages,removed);
    for (int i =0; i<length; i++){
        int cur  = reference_string[i];
        int result = check(cur_pages,cur);
        if ( cur_pages.size()<frame_size){
                cur_pages.push_back(cur);
                queue_print(myfile, frame_size, cur_pages,removed, cur, result);
                num_page_failed++;
                continue;
            }

        if  (result<0){
            num_page_failed++;
            int cur_removed = cur_pages.front();
            cur_pages.pop_front();
            cur_pages.push_back(cur);
            if (removed.size()<frame_size){
                removed.push_back(cur_removed);
            }
            else{
                removed.pop_front();
                removed.push_back(cur_removed);
            }
        }
        queue_print(myfile, frame_size, cur_pages,removed, cur, result);

    }
    cout<<"total pages "<<length<<" ,page fault rate: "<< (float)num_page_failed/(float) length<<endl;
    myfile << "total pages "<<length<<" ,page fault rate: "<< (float)num_page_failed/(float) length<<endl;
    myfile<<endl;
    myfile<<endl;
    myfile<<endl;
}


void LRU(ofstream &myfile, int frame_size,  deque<int> reference_string){
    int length =  reference_string.size();
    deque<int> cur_pages;
    deque<int> removed;
    int num_page_failed = 0;
    myfile << "LRU "<<endl;
    //queue_print(frame_size, cur_pages,removed);
    for (int i =0; i<length; i++){
        int cur  = reference_string[i];
        int result = check(cur_pages,cur);
        if ( cur_pages.size()<frame_size){
                cur_pages.push_back(cur);
                queue_print(myfile, frame_size, cur_pages,removed, cur, result);
                num_page_failed++;
                continue;
            }

        if  (result<0){
            num_page_failed++;
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
            cout<<"i: "<<i<<"  "<<cur<<endl;
            deque<int>::iterator it = cur_pages.begin() ;
            it += result;
            cur_pages.erase(it);
            cur_pages.push_back(cur);

        }
        queue_print(myfile, frame_size, cur_pages,removed, cur, result);

    }
    cout<<"total pages "<<length<<" ,page fault rate: "<< (float)num_page_failed/(float) length<<endl;
    myfile << "total pages "<<length<<" ,page fault rate: "<< (float)num_page_failed/(float) length<<endl;
    myfile<<endl;
    myfile<<endl;
    myfile<<endl;
}



void second(ofstream &myfile, int frame_size,  deque<int> reference_string){
    int length =  reference_string.size();
    deque<int> cur_pages;
    deque<int> signs; // sign for whether referenced
    deque<int> removed;
    myfile << "second chance "<<endl;
    //queue_print(frame_size, cur_pages,removed);
    int num_page_failed = 0;
    int cur_pointer = 0;
    for (int i =0; i<length; i++){
        int cur_num  = reference_string[i];
        int result = check(cur_pages,cur_num);
        if ( cur_pages.size()<frame_size){
                cur_pages.push_back(cur_num);
                signs.push_back(0);
                queue_print(myfile, frame_size, cur_pages,removed, cur_num, result);
                num_page_failed++;
                continue;
            }
        if  (result<0){
            num_page_failed++;
            while(signs[cur_pointer]==1){
                signs[cur_pointer]=0;
                cur_pointer ++;
                if (cur_pointer>= signs.size()){
                    cur_pointer = 0;
                }
            }
            int cur_removed = cur_pages[cur_pointer];


            cur_pages.erase(cur_pages.begin() +cur_pointer);
            //cout<<cur_num;
            cur_pages.insert(cur_pages.begin()+cur_pointer ,cur_num);
            //it1 = cur_pages.insert(it1,20);
            //it1 = cur_pages.insert(it1,200);
            signs.erase(signs.begin() + cur_pointer);
            signs.insert(signs.begin() + cur_pointer,0);
            cur_pointer ++;
            if (cur_pointer>= signs.size()){
                    cur_pointer = 0;
            }
            if (removed.size()<frame_size){
                removed.push_back(cur_removed);
            }
            else{
                removed.pop_front();
                removed.push_back(cur_removed);
            }
        }
        else{
            cout<<"i: "<<i<<"  "<<cur_num<<endl;
            signs[result] = 1;

        }
        queue_print(myfile, frame_size, cur_pages,signs, cur_num, result);

    }
    cout<<"total pages "<<length<<" ,page fault rate: "<< (float)num_page_failed/(float) length<<endl;
    myfile << "total pages "<<length<<" ,page fault rate: "<< (float)num_page_failed/(float) length<<endl;
    myfile<<endl;
    myfile<<endl;
    myfile<<endl;
}


int main () {
    generate_file();
    //read the input file to a deque
    deque<int> q;
    string line,t ;
    ifstream myfile_in ("Xiangd_proj2_input.txt");
    if (myfile_in.is_open())
    {
    while ( getline (myfile_in,line) )
    {
        cout << line << '\n';
        istringstream in(line);
        while(in>> t) {
        q.push_back(stoi(t));
        }
    }
    myfile_in.close();
    }
    else cout << "Unable to open file"; 

    for(int i =0;i<q.size();i++){
        cout<<to_string(q[i])+" ";
    }
    // choose frame size and algorithm
    cout<< "choosing algorithm 0:FIFO, 1:LRU, 2:second chance";
    int algo_id, frame_size;
    cin >> algo_id;
    cout<< "choosing frame size";
    cin >> frame_size;
    ofstream myfile;
    myfile.open ("Xiangd_proj2_output_user.txt");
    if (algo_id==0){
        FIFO(myfile, frame_size,q);
    }
    else if(algo_id==1){
        LRU(myfile, frame_size,q);
    }
    else{
        second(myfile, frame_size,q);
    }
    myfile.close();

    myfile.open ("Xiangd_proj2_output_4frames.txt");
    frame_size = 4;
    FIFO(myfile, frame_size,q);
    LRU(myfile, frame_size,q);
    second(myfile, frame_size,q);
    myfile.close();

    myfile.open ("Xiangd_proj2_output_8frames.txt");
    frame_size = 8;
    FIFO(myfile, frame_size,q);
    LRU(myfile, frame_size,q);
    second(myfile, frame_size,q);
    myfile.close();
    
    return 0;
}