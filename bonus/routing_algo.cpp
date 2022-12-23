#include "node.h"
#include <iostream>

using namespace std;

void printRT(vector<RoutingNode*> nd){
	for (int i = 0; i < nd.size(); i++) {
	    nd[i]->printTable();
	}
}

void routingAlgo(vector<RoutingNode*> nd){
    //Flooding
    for (RoutingNode* node: nd){
        node->sendMsg(); //all nodes send a message that recursively propagates through the nodes
    }

    for (RoutingNode* node: nd){
        node->dijkstra();
    }

    /*Print routing table entries after routing algo converges */
    printf("Printing the routing tables after the convergence \n");
    printRT(nd);
}

void trafficAlgo(vector<RoutingNode*> nd){
    for (int i = 0; i < nd.size(); i++){
        nd[i]->clearTraffic(); //all nodes sending traffic to node indexed as 0.
    }

    for (int i = 1; i < nd.size(); i++){
        nd[i]->sendTraffic(nd[0], 1); //all nodes sending traffic to node indexed as 0 (A).
    }

    for (int i = 0; i < nd.size(); i++){
        nd[i]->modifyInterfaces(); //all nodes sending traffic to node indexed as 0.
    }
}

void RoutingNode::recvMsg(RouteMsg msg){
    vector<pair<NetInterface, Node*>> msg_interface = msg.msg_interface;
    Node* from_where = msg.from_where;
    int seq_num = msg.seq_num;

    if(seq_nums[from_where] < seq_num){
        seq_nums[from_where] = seq_num;

        unordered_set<string> src_ips;
        for(int i = 0; i < msg_interface.size(); i++){
            src_ips.insert(msg_interface[i].first.getip());
        }

        for(auto it : ip_graph){
            for(auto src_ip : src_ips){
                ip_graph[src_ip].clear();
            }
        }

        for (int i = 0; i < msg_interface.size(); i++){
            NetInterface temp;
            temp = msg_interface[i].first;
            ip_graph[msg_interface[i].first.getip()].push_back(temp);
            // src_ips.insert(msg_interface[i].first.getip());
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

        for (int i = 0; i < interfaces.size(); i++){
            interfaces[i].second->recvMsg(msg);
        }
    }
    //table updated inside dijkstra implementation
    //calling dijkstra now is pointless since the flooding is still in progress.
    //for more optimized results, we can comment the dijkstra function out.
    // dijkstra(); 
}