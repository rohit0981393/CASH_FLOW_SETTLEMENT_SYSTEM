#include <bits/stdc++.h>
using namespace std;

class Institution
{
public:
    string bankName;
    int balanceSheet;
    set<string> paymentModes;
};

int findMinimumIndex(Institution bankLedger[], int totalBanks)
{
    int minimum = INT_MAX, idx = -1;
    for (int i = 0; i < totalBanks; i++)
    {
        if (bankLedger[i].balanceSheet == 0)
            continue;
        if (bankLedger[i].balanceSheet < minimum)
        {
            minimum = bankLedger[i].balanceSheet;
            idx = i;
        }
    }
    return idx;
}

int findHighestBalanceIndex(Institution bankLedger[], int totalBanks)
{
    int maximum = INT_MIN, idx = -1;
    for (int i = 0; i < totalBanks; i++)
    {
        if (bankLedger[i].balanceSheet == 0)
            continue;
        if (bankLedger[i].balanceSheet > maximum)
        {
            maximum = bankLedger[i].balanceSheet;
            idx = i;
        }
    }
    return idx;
}

pair<int, string> findCompatibleCreditor(Institution bankLedger[], int totalBanks, int debtorIdx, Institution originalData[], int maxTypes)
{
    int maxCredit = INT_MIN;
    int creditorIdx = -1;
    string modeMatch;

    for (int i = 0; i < totalBanks; i++)
    {
        if (bankLedger[i].balanceSheet <= 0)
            continue;

        vector<string> intersectedModes(maxTypes);
        auto itr = set_intersection(bankLedger[debtorIdx].paymentModes.begin(), bankLedger[debtorIdx].paymentModes.end(),
                                    bankLedger[i].paymentModes.begin(), bankLedger[i].paymentModes.end(),
                                    intersectedModes.begin());

        if ((itr - intersectedModes.begin()) != 0 && bankLedger[i].balanceSheet > maxCredit)
        {
            maxCredit = bankLedger[i].balanceSheet;
            creditorIdx = i;
            modeMatch = *(intersectedModes.begin());
        }
    }

    return {creditorIdx, modeMatch};
}

void displayTransactionResult(vector<vector<pair<int, string>>> txnMatrix, int totalBanks, Institution originalData[])
{
    cout << "\nMinimal required transactions for clearing dues:\n\n";
    for (int i = 0; i < totalBanks; i++)
    {
        for (int j = 0; j < totalBanks; j++)
        {
            if (i == j)
                continue;

            if (txnMatrix[i][j].first != 0 && txnMatrix[j][i].first != 0)
            {
                if (txnMatrix[i][j].first == txnMatrix[j][i].first)
                {
                    txnMatrix[i][j].first = 0;
                    txnMatrix[j][i].first = 0;
                }
                else if (txnMatrix[i][j].first > txnMatrix[j][i].first)
                {
                    txnMatrix[i][j].first -= txnMatrix[j][i].first;
                    txnMatrix[j][i].first = 0;
                    cout << originalData[i].bankName << " pays Rs " << txnMatrix[i][j].first << " to " << originalData[j].bankName << " via " << txnMatrix[i][j].second << endl;
                }
                else
                {
                    txnMatrix[j][i].first -= txnMatrix[i][j].first;
                    txnMatrix[i][j].first = 0;
                    cout << originalData[j].bankName << " pays Rs " << txnMatrix[j][i].first << " to " << originalData[i].bankName << " via " << txnMatrix[j][i].second << endl;
                }
            }
            else if (txnMatrix[i][j].first != 0)
            {
                cout << originalData[i].bankName << " pays Rs " << txnMatrix[i][j].first << " to " << originalData[j].bankName << " via " << txnMatrix[i][j].second << endl;
            }
            else if (txnMatrix[j][i].first != 0)
            {
                cout << originalData[j].bankName << " pays Rs " << txnMatrix[j][i].first << " to " << originalData[i].bankName << " via " << txnMatrix[j][i].second << endl;
            }

            txnMatrix[i][j].first = 0;
            txnMatrix[j][i].first = 0;
        }
    }
    cout << "\n";
}

void optimizeCashTransfers(int totalBanks, Institution originalData[], unordered_map<string, int> &bankMap, int txnCount, vector<vector<int>> &txnGraph, int maxTypes)
{
    Institution bankLedger[totalBanks];

    for (int i = 0; i < totalBanks; i++)
    {
        bankLedger[i].bankName = originalData[i].bankName;
        bankLedger[i].paymentModes = originalData[i].paymentModes;
        int netBalance = 0;

        for (int row = 0; row < totalBanks; row++)
        {
            netBalance += txnGraph[row][i];
        }
        for (int col = 0; col < totalBanks; col++)
        {
            netBalance -= txnGraph[i][col];
        }
        bankLedger[i].balanceSheet = netBalance;
    }

    vector<vector<pair<int, string>>> txnMatrix(totalBanks, vector<pair<int, string>>(totalBanks, {0, ""}));
    int clearedAccounts = 0;

    for (int i = 0; i < totalBanks; i++)
    {
        if (bankLedger[i].balanceSheet == 0)
            clearedAccounts++;
    }

    while (clearedAccounts != totalBanks)
    {
        int debtor = findMinimumIndex(bankLedger, totalBanks);
        auto result = findCompatibleCreditor(bankLedger, totalBanks, debtor, originalData, maxTypes);
        int creditor = result.first;

        if (creditor == -1)
        {
            txnMatrix[debtor][0].first += abs(bankLedger[debtor].balanceSheet);
            txnMatrix[debtor][0].second = *(originalData[debtor].paymentModes.begin());

            int backupCreditor = findHighestBalanceIndex(bankLedger, totalBanks);
            txnMatrix[0][backupCreditor].first += abs(bankLedger[debtor].balanceSheet);
            txnMatrix[0][backupCreditor].second = *(originalData[backupCreditor].paymentModes.begin());

            bankLedger[backupCreditor].balanceSheet += bankLedger[debtor].balanceSheet;
            bankLedger[debtor].balanceSheet = 0;

            if (bankLedger[debtor].balanceSheet == 0)
                clearedAccounts++;
            if (bankLedger[backupCreditor].balanceSheet == 0)
                clearedAccounts++;
        }
        else
        {
            int flowAmount = min(abs(bankLedger[debtor].balanceSheet), bankLedger[creditor].balanceSheet);
            txnMatrix[debtor][creditor].first += flowAmount;
            txnMatrix[debtor][creditor].second = result.second;

            bankLedger[debtor].balanceSheet += flowAmount;
            bankLedger[creditor].balanceSheet -= flowAmount;

            if (bankLedger[debtor].balanceSheet == 0)
                clearedAccounts++;
            if (bankLedger[creditor].balanceSheet == 0)
                clearedAccounts++;
        }
    }

    displayTransactionResult(txnMatrix, totalBanks, originalData);
}

int main()
{
    cout << "\n\t\t\t\t*********** CASH FLOW SETTLEMENT SYSTEM ***********\n\n";
    cout << "This tool computes minimum transactions between international banks using available payment options.\n\n";

    int totalBanks;
    cout << "Enter total number of banks:\n";
    cin >> totalBanks;

    Institution bankRecords[totalBanks];
    unordered_map<string, int> bankIndex;

    cout << "Enter each bank's name, number of payment modes and the modes themselves:\n(No spaces in names or modes)\n";

    int maxModeCount;
    for (int i = 0; i < totalBanks; i++)
    {
        if (i == 0)
            cout << "World Financial Hub : ";
        else
            cout << "Bank " << i << " : ";

        cin >> bankRecords[i].bankName;
        bankIndex[bankRecords[i].bankName] = i;

        int modeCount;
        cin >> modeCount;

        if (i == 0)
            maxModeCount = modeCount;

        while (modeCount--)
        {
            string mode;
            cin >> mode;
            bankRecords[i].paymentModes.insert(mode);
        }
    }

    int txnCount;
    cout << "Enter number of transactions:\n";
    cin >> txnCount;

    vector<vector<int>> cashMatrix(totalBanks, vector<int>(totalBanks, 0));
    cout << "Enter debtor bank, creditor bank and transaction amount:\n";

    for (int i = 0; i < txnCount; i++)
    {
        cout << i << "th transaction: ";
        string from, to;
        int amount;
        cin >> from >> to >> amount;
        cashMatrix[bankIndex[from]][bankIndex[to]] = amount;
    }

    optimizeCashTransfers(totalBanks, bankRecords, bankIndex, txnCount, cashMatrix, maxModeCount);
    return 0;
}
