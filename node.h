#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

class RoutingEntry{
    public:
        string dstip, nexthop;
        string ip_interface;
        int cost;
};

class Comparator{
    public:
        bool operator()(const RoutingEntry &R1,const RoutingEntry &R2){
            if (R1.cost == R2.cost) {
                return R1.dstip.compare(R2.dstip)<0;
            }
            else if(R1.cost > R2.cost) {
                return false;
            }
            else {
                return true;
            }
        }
};

struct routingtbl {
    vector<RoutingEntry> tbl;
};

class Node;

class NetInterface {
    private:
        string ip;
        string connectedTo; 	//this node is connected to this ip
        int cost;
  
    public:
        string getip() {
            return this->ip;
        }
        string getConnectedIp() {
            return this->connectedTo;
        }
        int getCost(){
            return this->cost;
        }
        void setip(string ip) {
            this->ip = ip;
        }
        void setConnectedip(string ip) {
            this->connectedTo = ip;
        }
        void setCost(int cost){
            this->cost = cost;
        }
};

class RouteMsg {
    public:
        vector<pair<NetInterface, Node*>> msg_interface;
        Node* from_where;
        int seq_num;
};

class Node {
    private:
        string name;
        int sequence_num = 1;

    protected:
        struct routingtbl mytbl;
        vector<pair<NetInterface, Node*>> interfaces;
        unordered_map<string, vector<NetInterface>> ip_graph;
        unordered_map<Node *, int> seq_nums;

    public:

        virtual void recvMsg(RouteMsg msg){
            cout<<"Base"<<endl;
        }

        void setName(string name){
            this->name = name;
        }
        
        void addInterface(string ip, string connip, Node *nextHop, int cost) {
            NetInterface eth;
            eth.setip(ip);
            eth.setConnectedip(connip);
            eth.setCost(cost);
            interfaces.push_back({eth, nextHop});
        }
        
        void addTblEntry(string myip, int cost) {
            RoutingEntry entry;
            entry.dstip = myip;
            entry.nexthop = myip;
            entry.ip_interface = myip;
            entry.cost = cost;
            mytbl.tbl.push_back(entry);
        }

        void updateTblEntry(string dstip, string nexthop, string ip_interface, int cost) {
            for (int i = 0; i < mytbl.tbl.size(); i++){
                if(dstip == mytbl.tbl[i].dstip){
                    mytbl.tbl.erase(mytbl.tbl.begin()+i);
                }
            }
            RoutingEntry entry;
            entry.dstip = dstip;
            entry.nexthop = nexthop;
            entry.ip_interface = ip_interface;
            entry.cost = cost;
            mytbl.tbl.push_back(entry);
        }

        string getName() {
            return this->name;
        }
        
        void printTable() {
            Comparator myobject;
            sort(mytbl.tbl.begin(),mytbl.tbl.end(),myobject);
            cout<<this->getName()<<":"<<endl;
            for (int i = 0; i < mytbl.tbl.size(); ++i) {
                cout<<mytbl.tbl[i].dstip<<" | "<<mytbl.tbl[i].nexthop<<" | "<<mytbl.tbl[i].ip_interface<<" | "<<mytbl.tbl[i].cost <<endl;
            }
        }
        
        void sendMsg(){
            seq_nums[this] = sequence_num; //own sequence number will be the same

            //building graph
            unordered_set<string> src_ips;
            for (int i = 0; i < interfaces.size(); i++){
                NetInterface temp;
                temp = interfaces[i].first;

                ip_graph[interfaces[i].first.getip()].push_back(temp);
                src_ips.insert(interfaces[i].first.getip());
            }

            for (string i: src_ips){
                for (string j: src_ips){
                    if(i != j){
                        NetInterface tempInt;
                        tempInt.setip(i);
                        tempInt.setConnectedip(j);
                        tempInt.setCost(0);

                        ip_graph[i].push_back(tempInt);
                    }
                }
            }

            for (int i = 0; i<interfaces.size(); i++){
                RouteMsg msg;
                msg.from_where = this;
                msg.msg_interface = interfaces;
                msg.seq_num = sequence_num;
                interfaces[i].second->recvMsg(msg);
            }
        }

        void dijkstra(){
            const int INF = 1e9;
            unordered_map<string, int> dist;
            unordered_map<string, string> ip_interfaces;
            unordered_map<string, string> next_hops;

            for(auto element : ip_graph){
                dist[element.first] = INF;
                ip_interfaces[element.first] = "NULL";
                next_hops[element.first] = "NULL";
            }

            priority_queue<pair<int, string>, vector<pair<int, string>>, less<pair<int, string>>> pq;

            for(auto i : interfaces){
                ip_interfaces[i.first.getip()] = i.first.getip();
                dist[i.first.getip()] = 0;
                pq.push({0, i.first.getip()});
            }

            while(!pq.empty()){
                string cur_ip = pq.top().second;
                pq.pop();
                for(NetInterface child : ip_graph[cur_ip]){
                    // cout<<child.netInt.getCost()<<"cost\n"<<child.netInt.getip()<<"ip\n"<<child.netInt.getConnectedIp()<<"child ip"<<endl;
                    if (dist[cur_ip] + child.getCost() < dist[child.getConnectedIp()]){
                        dist[child.getConnectedIp()] = dist[cur_ip] + child.getCost();
                        ip_interfaces[child.getConnectedIp()] = ip_interfaces[cur_ip];
                        if (next_hops[cur_ip] == "NULL"){
                            next_hops[child.getConnectedIp()] = child.getConnectedIp();
                        }
                        else{
                            next_hops[child.getConnectedIp()] = next_hops[cur_ip];
                        }
                        pq.push({dist[child.getConnectedIp()], child.getConnectedIp()});
                    }
                }
            }

            //Update table
            for (auto i : ip_graph){

                if(next_hops[i.first] == "NULL"){
                    updateTblEntry(i.first, i.first, ip_interfaces[i.first], dist[i.first]);
                }
                else{
                    updateTblEntry(i.first, next_hops[i.first], ip_interfaces[i.first], dist[i.first]);
                }
            }

        }
};

class RoutingNode: public Node {
    public:
        void recvMsg(RouteMsg msg);
};
