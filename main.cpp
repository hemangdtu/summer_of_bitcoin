#include<bits/stdc++.h>
using namespace std;

string input = "mempool.csv";   // Reference to file where dataset is available
set<string> included_trx_ids;   // Variable to store all Taxation IDs

int totalBlockWeight = 4000000; // Max weight of transactions in a block

class TransactionInfo   // Class for all transactions to be done
{
    public:
        int fee;
        int weight;
        string tx_id;
        vector<string> parents;
};

pair<string,TransactionInfo*> initiateTransaction(vector<string>& row)    // Function to initiate track transactions
{
    vector<string> p;
    auto ans = new TransactionInfo();
    
    ans -> tx_id = row[0];
    ans -> fee = stoi(row[1]);
    ans -> weight = stoi(row[2]);
    for (int i = 3; i < row.size(); i++)
        p.push_back(row[i]);
    ans -> parents = p;

    return {row[0], ans};
}

void readinput(string input, unordered_map<string, TransactionInfo*>& ump)    // Function to read input from CSV
{
    ifstream fin(input);

    vector<string> row;
    string word;
    string line;
    string temp;
    
    getline(fin, line);
    
    while(getline(fin, line))
    {
        row.clear();
        stringstream s(line);
        while(getline(s, word, ','))
            row.push_back(word);

        pair<string, TransactionInfo*> p = initiateTransaction(row);
        ump[p.first] = p.second;
    }

    fin.close();

    cout<<"Transaction Count :" << ump.size() << "\n";
}

bool isValidTx(TransactionInfo* tx,set<string>& included_tx_set)    // Function to track valid transactions
{
    for(auto parent : tx -> parents)
        if(included_tx_set.find(parent) == included_tx_set.end())
            return false;

    return true;
}

void writeOutput(vector<string>& included_tx_vector, string fn)    // Function for developing output file
{
    ofstream myfile(fn);
    for(auto s : included_tx_vector)
        myfile << s << "\n";
    myfile.close();
}

int main()
{
    unordered_map<string, TransactionInfo*> ump;
    readinput(input, ump);
    set<pair<float, TransactionInfo*>, greater<pair<float, TransactionInfo*>>> tx_set;
    set<string> included_tx_set;
    vector<string> included_tx_vector;

    for(auto p : ump)
        tx_set.insert({((float)p.second -> fee) / ((float)p.second -> weight), p.second});
    
    int currBlockWeight = 0;
    int totalFee = 0;

    while(!tx_set.empty() && currBlockWeight < totalBlockWeight)
    {
        bool found = false;
        for(auto itr = tx_set.begin(); itr != tx_set.end(); itr++)
        {
            TransactionInfo* curr_tx = (*itr).second;
            int currFee = curr_tx -> fee;
            int currWeight = curr_tx -> weight;
            if(isValidTx(curr_tx, included_tx_set) && currBlockWeight + currWeight <= totalBlockWeight)
            {
                currBlockWeight += currWeight;
                included_tx_set.insert(curr_tx -> tx_id);
                included_tx_vector.push_back(curr_tx -> tx_id);
                totalFee += currFee;
                tx_set.erase(itr);
                found = true;
                break;
            }
        }

        if(!found)
            break; 
    }

    cout<<"Total Fees in Current Block : "<< totalFee << "\nTotal Weight : " << currBlockWeight << "\n";
    cout<<"Transactions in Final Block : " << included_tx_set.size() << "\n";
    writeOutput(included_tx_vector, "block.txt");

    return 0;
}