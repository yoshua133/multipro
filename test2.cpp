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
            //space = Semaphore(maxsize);
            //record = Semaphore(0);
            //message no_use{-1,-1,-1};
            //this->add(no_use);
            
        }
        void add(message to_be_add){
            message_queue.push_back(to_be_add);
            //print.P();
            //cout<<"after push "<<this->message_queue.size()<<endl;
            //print.V();

        }
        message pop(){
            message output = message_queue.front();
            //print.P();
            //cout<<"pop"<<output.data<<endl;
            //print.V();
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
            //print.P();
            //cout<<counter2;
            //print.V();
            //self_box.record.V();
            self_box.record.P();
            message received = self_box.pop();

            //print.P();
            //cout<<"size"<<self_box.message_queue.size()<<endl;
            //print.V();

            //cout<<"producer PV";
            self_box.space.V();
            
            int consumer_id = received.sender_id;
            int data = received.data;
            int to_send = data *producer_num +idx;

            print.P();
            cout<<"producer "<<idx<<" received"<< consumer_id << " DATA "<<data<<endl;
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
            cout<<"producer "<<idx<<" send to "<< idx << " DATA "<<data<<endl;
            print.V();
            target.record.V();
        }
        void entry(){
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
            while(true){
                receive();
                // if (counter2>80) break;
                };
            //cout<< "producer entry";
            
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

            // print.P();
            // cout<<"after consumer send"<< target.message_queue.size()<<endl;
            // print.V();

            target.record.V();
        }
        void entry(){
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            while(true){
                //cout<<"counter"<<counter;
                if (counter>40) break;
                counter++;
                int need_producer_id = counter % producer_num;
                send(need_producer_id, counter/producer_num);
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                //print.P();
                //cout<<"send to "<<need_producer_id<<" "<<counter/producer_num<<endl;
                //print.V();
                receive();
            }
            //cout<<"in con entry";
        }
        int idx;
        mailbox &self_box;
};

void pro_entry(int idx){
    
    producer new_pro(idx);
    //cout<<"pro entry";
    new_pro.entry();
}
void con_entry(int idx){
    //cout<<"true id" <<idx;
    consumer new_con(idx);
    //cout<<"con entry"<<endl<<"id"<<new_con.idx;
    new_con.entry();
}
void test(int idx){
    cout<<1;
}

int main(){
    // for(int i =0;i<producer_num;i++){
    //     producer_mailboxes[i] = mailbox(maxsize);
    // };
    // for(int i =0;i<producer_num;i++){
    //     consumer_mailboxes[i] = mailbox(maxsize);
    // };
    // producer producers[num];
    // consumer consumers[num];
    //cout<<"1111";
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