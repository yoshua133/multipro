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


// class message{
//     public:
//         message(int data=0, int sender_id=0, int receiver_id=0):data(data),sender_id(sender_id), receiver_id(receiver_id){};
//         int data;
//         int sender_id;
//         int receiver_id;
// };

struct message{
    int data;
    int sender_id;
    int receiver_id;
};

class Semaphore
{
public:
    Semaphore(long count=0):count(count) {}
    Semaphore( const Semaphore &obj){
        count = obj.count;
    }
    //V signal
    void V()
    {
        unique_lock<mutex> unique(mt);
        ++count;
        if (count <= 0)
            //cout<<"wake";
            cond.notify_one();
    }
    //P wait
    void P()
    {
        unique_lock<mutex> unique(mt);
        --count;
        if (count < 0){
            //cout<<"sleep";
            cond.wait(unique);
        }
    }
    long count;
private:
    mutex mt;
    condition_variable cond;
    
};
Semaphore print(1);
class mailbox{
    public:
        mailbox(int maxsize=0):space(Semaphore(maxsize)),record(Semaphore(0)){
            
        }
        void add(message to_be_add){
            message_queue.push_back(to_be_add);

        }
        message pop(){
            message output = message_queue.front();
            message_queue.pop_front();
            return output;
        };
        Semaphore space;
        Semaphore record;
        deque<message> message_queue;
        
};

int maxsize = 1000;
const int num = 4;
const int producer_num = num;
//mailbox producer_mailboxes[producer_num]{mailbox(maxsize),mailbox(maxsize)};//,mailbox(maxsize),mailbox(maxsize)};
//mailbox consumer_mailboxes[producer_num]{mailbox(maxsize),mailbox(maxsize)};//,mailbox(maxsize),mailbox(maxsize)};
mailbox producer_mailboxes[producer_num]{mailbox(maxsize) , mailbox(maxsize),mailbox(maxsize),mailbox(maxsize)};
mailbox consumer_mailboxes[producer_num]{mailbox(maxsize) ,mailbox(maxsize),mailbox(maxsize),mailbox(maxsize)};
int counter = 0;
int counter2 = 0;

class producer{
    public:
        producer(int idx=0): idx(idx),self_box(producer_mailboxes[idx]){}
        void receive(){
            counter2++;
            self_box.record.P();
            message received = self_box.pop();

            self_box.space.V();
            
            int consumer_id = received.sender_id;
            int data = received.data;
            int to_send = data *producer_num +idx;

            print.P();
            cout<<"producer "<<idx<<" received "<< "DATA "<<data<<" from "<<consumer_id<<endl;
            print.V();
            send(consumer_id, 0); //send ack
            
            send(consumer_id, to_send);
        }

        void send(int receiver_id, int data){
            message to_send = {data, idx, receiver_id};
            mailbox &target = consumer_mailboxes[receiver_id];
            target.space.P();
            target.add(to_send);
            print.P();
            cout<<"producer "<<idx<<" send "<< "DATA "<<data<<" to "<<idx<<endl;
            print.V();
            target.record.V();
        }
        void entry(){
            while(true){
                receive();
                };
            
        }
        int idx;
        mailbox &self_box;
};

class consumer{
    public:
        consumer(int idx=0): idx(idx),self_box(consumer_mailboxes[idx]){}
        void receive(){
            self_box.record.P();
            message received = self_box.pop();
            self_box.space.V();
            int data = received.data;

            print.P();
            if (data==0) cout<< "consumer "<<idx<<" get ack from" <<received.sender_id<<endl;
            else cout<< "consumer "<<idx<<" get processed data "<< data<<" from " <<received.sender_id<<endl;
            print.V();
        }

        void send(int receiver_id, int request){
            message to_send = {request, idx, receiver_id};
            mailbox &target = producer_mailboxes[receiver_id];
            target.space.P();

            print.P();
            cout<<"consumer "<<idx<<" send to "<<receiver_id<<" req "<<request<<endl;// target.message_queue.size()<<endl;
            print.V();

            target.add(to_send);


            target.record.V();
        }
        void entry(){
            while(true){
                if (counter>40) break;
                counter++;
                int need_producer_id = counter % producer_num;
                send(need_producer_id, counter/producer_num);
                receive();
            }
    
        }
        int idx;
        mailbox &self_box;
};

void pro_entry(int idx){
    
    producer new_pro(idx);
    new_pro.entry();
}
void con_entry(int idx){
    consumer new_con(idx);
    new_con.entry();
}
void test(int idx){
    cout<<1;
}

int main(){
    thread producers_thread[num], consumers_thread[num];
    for (int i = 0; i < num; ++i)
    {
        consumers_thread[i] = thread(con_entry,i);
        //thread con_thread(test,1);
        producers_thread[i] = thread(pro_entry, i);
    }
    //cout<<"now";
    for (int i = 0; i < num; ++i)
    {
        consumers_thread[i].join();
        producers_thread[i].join();
    }
    return 0;
}