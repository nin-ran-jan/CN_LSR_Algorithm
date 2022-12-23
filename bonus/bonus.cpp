#include "node.h"
#include <unistd.h>

vector<RoutingNode*> distanceVectorNodes;
void routingAlgo(vector<RoutingNode*> distanceVectorNodes);
void trafficAlgo(vector<RoutingNode*> distanceVectorNodes);

int main() {
    int n;
    cin>>n;
    string name;
    distanceVectorNodes.clear();
    for (int i = 0 ; i < n; i++) {
        RoutingNode *newnode = new RoutingNode();
        cin>>name;
        newnode->setName(name);
        distanceVectorNodes.push_back(newnode);
    }
    cin>>name;
    while(name != "EOE") {
        for (int i =0 ; i < distanceVectorNodes.size(); i++) {
            string myeth,oeth, oname;
            int cost;
            if(distanceVectorNodes[i]->getName() == name) {
                cin>>myeth;
                // cin>>cost;
                cin>>oeth;
                cin>>oname;
                for(int j = 0 ; j < distanceVectorNodes.size(); j++) {
                    if(distanceVectorNodes[j]->getName() == oname) {
                        distanceVectorNodes[i]->addInterface(myeth, oeth, distanceVectorNodes[j], 1); 
                        //cost weight across all links = 1
                        distanceVectorNodes[i]->addTblEntry(myeth, 0);
                        break;
	                }
	            }
	        }
        }
        cin>>name;
    }

    // cout << "FIRST INIT ROUTING ALGO" << endl;
    routingAlgo(distanceVectorNodes);

    while(true){
        // cout << "\n\n\n\nTRAFFIC ALGO PLUS ROUTING ALGO" << endl;
        trafficAlgo(distanceVectorNodes);
        routingAlgo(distanceVectorNodes);
        sleep(1); //computing algo every 1 second
    }
    
}