#include<bits/stdc++.h>
# include<fstream>
using namespace std;

struct Process{
    int pid;
    int ArrivalTime;
    int BurstTime;
    int CompletionTime;
    int TurnaroundTime;
    int WaitingTime;
    int burstTimeRemaining;
};

const int N=1e6+10;
Process proc[N];

void getTAT(Process& P){
    P.TurnaroundTime=P.CompletionTime-P.ArrivalTime;
}

void getWT(Process& P){
    P.WaitingTime=P.TurnaroundTime-P.BurstTime;
}

int TotalWaitTime=0, TotalTurnAroundTime=0;
float AverageTurnAroundTime, AverageWaitTime;

void getData(int n){
    for(int i=1; i<=n; i++){
        TotalWaitTime+=proc[i].WaitingTime;
        TotalTurnAroundTime+=proc[i].TurnaroundTime;
    }
    AverageWaitTime= (float) TotalWaitTime / n;
    AverageTurnAroundTime= (float) TotalTurnAroundTime / n;
}
bool CompareArrivalTime(Process a, Process b)
{
    if (a.ArrivalTime == b.ArrivalTime)
        return a.pid < b.pid;
    return a.ArrivalTime < b.ArrivalTime;
}

struct util1 {
    int pid1;
    int bt1;
};
 
util1 range;
util1 tree[4 * N + 5];
int mp[N + 1];

// Function to update the burst time and process id in the segment tree
void update(int node, int st, int end, int ind, int id1, int b_t)
{
    if (st == end) {
        tree[node].pid1 = id1;
        tree[node].bt1 = b_t;
        return;
    }
    int mid = st+ (end - st) / 2;
    if (ind <= mid)
        update(2 * node, st, mid, ind, id1, b_t);
    else
        update(2 * node + 1, mid + 1, end, ind, id1, b_t);
    if (tree[2 * node].bt1 < tree[2 * node + 1].bt1) {
        tree[node].bt1 = tree[2 * node].bt1;
        tree[node].pid1 = tree[2 * node].pid1;
    }
    else {
        tree[node].bt1 = tree[2 * node + 1].bt1;
        tree[node].pid1 = tree[2 * node + 1].pid1;
    }
}


// Function to return the range minimum of the burst time of all the arrived processes using segment tree
util1 query(int node, int st, int end, int lt, int rt)
{
    if (end < lt || st > rt)
        return range;
    if (st >= lt && end <= rt)
        return tree[node];
    int mid = st+ (end-st) / 2;
    util1 lm = query(2 * node, st, mid, lt, rt);
    util1 rm = query(2 * node + 1, mid + 1, end, lt, rt);
    if (lm.bt1 < rm.bt1)
        return lm;
    return rm;
}

void FCFS(int n){
    int time=0;
    for(int i=1; i<=n; i++){
        proc[i].CompletionTime=max(time, proc[i].ArrivalTime)+proc[i].BurstTime;
        getTAT(proc[i]);
        getWT(proc[i]);
        time=proc[i].CompletionTime;
    }
}

void SJF(int n)
{
    int counter = n;
    int upper_range = 0;

    int time = min(INT_MAX, proc[upper_range + 1].ArrivalTime);

    while (counter) {
        for (; upper_range <= n;) {
            upper_range++;
            if (proc[upper_range].ArrivalTime > time || upper_range > n){
                upper_range--;
                break;
            }

            update(1, 1, n, upper_range, proc[upper_range].pid, proc[upper_range].BurstTime);
        }
        
        util1 res = query(1, 1, n, 1, upper_range);

        if (res.bt1 != INT_MAX) {
            counter--;
            int index = mp[res.pid1];
            time += (res.bt1);
            proc[index].CompletionTime = time;
            getTAT(proc[index]);
            getWT(proc[index]);

            update(1, 1, n, index, INT_MAX, INT_MAX);
        }
        else {
            time = proc[upper_range + 1].ArrivalTime;
        }
    }
}

int quant=0;

void RR(int n)
{
    bool inQ[n+1]={false}, complete[n+1]={false};

    queue<int> readyQueue;
    readyQueue.push(1);
    inQ[1] = true;
   
    int currentTime = 0;
    int programsExecuted = 0;
 
    while (!readyQueue.empty())
    {
        int i = readyQueue.front();
        readyQueue.pop();

    if (proc[i].burstTimeRemaining <= quant)
    {
        complete[i] = true;
        currentTime += proc[i].burstTimeRemaining;
        proc[i].CompletionTime = currentTime;
        getTAT(proc[i]);
        getWT(proc[i]);
 
        if (proc[i].WaitingTime < 0)
            proc[i].WaitingTime = 0;
 
        proc[i].burstTimeRemaining = 0;

        if (programsExecuted != n)
        {
            for (int i = 1; i <= n; i++)
            {   
                if (proc[i].ArrivalTime<= currentTime && !inQ[i] && !complete[i])
                {
                    inQ[i] = true;
                    readyQueue.push(i);
                }
            }
        }
    }
    else
    {
        proc[i].burstTimeRemaining -= quant;
        currentTime += quant;

        if (programsExecuted != n)
        {
            for (int i = 1; i <= n; i++)
            {
                if (proc[i].ArrivalTime<= currentTime && !inQ[i] && !complete[i])
                {
                    inQ[i] = true;
                    readyQueue.push(i);
                }
            }
        }
        readyQueue.push(i);
    }
    }
}

void SRT(int n)
{
    int RemainingTime[n+1];

    for (int i = 1; i <= n; i++)
        RemainingTime[i] = proc[i].BurstTime;

    int complete = 0, time = 0, minm = INT_MAX;
    int shortest = 0, temp=0;
    bool check = false;

    int j=1;
    priority_queue <pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    while (complete != n) {
        if(pq.empty() && j<=n ){
            pq.push({RemainingTime[j], j});
            time=max(time, proc[j].ArrivalTime);
            j++;
        }

        int id=pq.top().second;
        pq.pop();
        temp=time;
        if(j<=n){
            time+=min(RemainingTime[id], proc[j].ArrivalTime-temp);
            RemainingTime[id]-=min(RemainingTime[id], proc[j].ArrivalTime-temp);
            if(proc[j].ArrivalTime<=time){
                pq.push({RemainingTime[j], j});
                j++;
            }
        }else{
            time+=RemainingTime[id];
            RemainingTime[id]=0;
        }
        if(RemainingTime[id]==0){
            complete++;
            proc[id].CompletionTime=time;
        }else{
            pq.push({RemainingTime[id], id});
        }
    }

    for(int i=1; i<=n; i++){
        getTAT(proc[i]);
        getWT(proc[i]);
    }
}

void reset(){
    TotalTurnAroundTime=0;
    TotalWaitTime=0;
}

void print(int n, ofstream& outfile)
{

    outfile << "ProcessId  "<< "Arrival Time  "<< "Burst Time  "<< "Completion Time  "<< "Turn Around Time  "<< "Waiting Time\n";
    for (int i = 1; i <= n; i++) {
        outfile << proc[i].pid << " \t\t "<< proc[i].ArrivalTime << " \t\t "<< proc[i].BurstTime << " \t\t "<< proc[i].CompletionTime << " \t\t "<< proc[i].TurnaroundTime << " \t\t "<< proc[i].WaitingTime << " \n";
    }
    outfile<<"Total Waiting Time: "<<TotalWaitTime<<endl;
    outfile<<"Avergae Waiting Time: "<<AverageWaitTime<<endl;
    outfile<<"Total Turn Around Time: "<<TotalTurnAroundTime<<endl;
    outfile<<"Averge Turn Around Time: "<<AverageTurnAroundTime<<endl;
    outfile<<endl;

}

void execute()
{
    unordered_map<string, int> AWT, ATAT;
    float MinimumWaitTime=INT_MAX;
    float MinimumTurnAroundTime=INT_MAX;

    range.pid1 = INT_MAX;
    range.bt1 = INT_MAX;
 
    for (int i = 1; i <= 4 * N + 1; i++) {
        tree[i].pid1 = INT_MAX;
        tree[i].bt1 = INT_MAX;
    }
    
    ofstream outfile("output.txt");
    
    ifstream infile("input.txt");
    if (!infile) {
        cerr << "Error: Unable to open input.txt" << endl;
        return;
    }
    int n;
    infile >> n;
    for(int i=1; i<=n; i++){
        infile >> proc[i].pid >> proc[i].ArrivalTime >> proc[i].BurstTime;
    }
    infile>> quant;

    sort(proc+1, proc+1+n, CompareArrivalTime);
    for (int i = 1; i <= n; i++)
        mp[proc[i].pid] = i;

    reset();
    outfile<<"First Come First Serve"<<endl;
    FCFS(n);
    getData(n);
    print(n, outfile);
    AWT["First Come First Serve"]=AverageWaitTime;
    ATAT["First Come First Serve"]=AverageTurnAroundTime;
    MinimumTurnAroundTime=min(MinimumTurnAroundTime, AverageTurnAroundTime);
    MinimumWaitTime=min(MinimumWaitTime, AverageWaitTime);

    reset();
    outfile<<"Shortest Job First"<<endl;
    SJF(n);
    getData(n);
    print(n, outfile);
    AWT[
        "Shortest Job First"
    ]=AverageWaitTime;
    ATAT["Shortest Job First"]=AverageTurnAroundTime;
    MinimumTurnAroundTime=min(MinimumTurnAroundTime, AverageTurnAroundTime);
    MinimumWaitTime=min(MinimumWaitTime, AverageWaitTime);

    reset();
    outfile<<"Round Robin"<<endl;
    RR(n);
    getData(n);
    print(n, outfile);
    AWT["Round Robin"]=AverageWaitTime;
    ATAT["Round Robin"]=AverageTurnAroundTime;
    MinimumTurnAroundTime=min(MinimumTurnAroundTime, AverageTurnAroundTime);
    MinimumWaitTime=min(MinimumWaitTime, AverageWaitTime);

    reset();
    outfile<<"Shortest Remaining Time"<<endl;
    SRT(n);
    getData(n);
    print(n, outfile);
    AWT["Shortest Remaining Time"]=AverageWaitTime;
    ATAT["Shortest Remaining Time"]=AverageTurnAroundTime;
    MinimumTurnAroundTime=min(MinimumTurnAroundTime, AverageTurnAroundTime);
    MinimumWaitTime=min(MinimumWaitTime, AverageWaitTime);

    outfile<<"....................................................................................................."<<endl;
    outfile<<"Statictics: "<<endl;

    outfile<<"Algorithm with least waiting time: "<<endl;
    for(auto& x:AWT){
        if(x.second==MinimumWaitTime){
            outfile<<"\t"<<x.first<<endl;
        }
    }
    outfile<<endl;
    outfile<<"Algorithm with least Turn around time: "<<endl;
    for(auto& x:ATAT){
        if(x.second==MinimumTurnAroundTime){
            outfile<<"\t"<<x.first<<endl;
        }
    }


    
    outfile.close();
    
}




int main(){
    

    cout << setprecision(2) << fixed;

    execute();

    return 0;
}
