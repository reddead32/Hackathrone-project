#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <ctime>
#include <iomanip>
using namespace std;

#define MIN_BALANCE 500

class InsufficientFunds {};

struct Transaction {
    string date;
    string type;
    float amount;
};

class Account {
private:
    long accountNumber;
    string firstName;
    string lastName;
    float balance;
    static long NextAccountNumber;
    vector<Transaction> passbook;

public:
    Account() {}
    Account(string fname, string lname, float balance);

    long getAccNo() const { return accountNumber; }
    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
    float getBalance() const { return balance; }

    void Deposit(float amount);
    void Withdraw(float amount);
    void AddToPassbook(const string& type, float amount);
    void ShowPassbook() const;

    static void setLastAccountNumber(long accountNumber);
    static long getLastAccountNumber();

    friend ofstream& operator<<(ofstream& ofs, const Account& acc);
    friend ifstream& operator>>(ifstream& ifs, Account& acc);
    friend ostream& operator<<(ostream& os, const Account& acc);
};

long Account::NextAccountNumber = 0;

class Bank {
private:
    map<long, Account> accounts;

public:
    Bank();
    Account OpenAccount(const string& fname, const string& lname, float balance);
    Account BalanceEnquiry(long accountNumber) const;
    Account Deposit(long accountNumber, float amount);
    Account Withdraw(long accountNumber, float amount);
    void CloseAccount(long accountNumber);
    void ShowAllAccounts() const;
    void ConvertCurrency(float amount, const string& fromCurrency, const string& toCurrency);
    ~Bank();
};

int main() {
    Bank b;
    Account acc;
    int choice;
    string fname, lname;
    long accountNumber;
    float balance;
    float amount;

    cout << "Banking System" << endl;
    do {
        cout << "\nSelect one option below:"
             << "\n1. Open an Account"
             << "\n2. Balance Enquiry"
             << "\n3. Deposit"
             << "\n4. Withdrawal"
             << "\n5. Close an Account"
             << "\n6. Show All Accounts"
             << "\n7. Show Passbook"
             << "\n8. Currency Converter"
             << "\n9. Quit"
             << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter First Name: ";
                cin >> fname;
                cout << "Enter Last Name: ";
                cin >> lname;
                cout << "Enter initial Balance: ";
                cin >> balance;
                acc = b.OpenAccount(fname, lname, balance);
                cout << "Congratulations, Account is Created" << endl;
                cout << acc;
                break;

            case 2:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                acc = b.BalanceEnquiry(accountNumber);
                cout << "Your Account Details" << endl;
                cout << acc;
                break;

            case 3:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                cout << "Enter Amount: ";
                cin >> amount;
                acc = b.Deposit(accountNumber, amount);
                cout << "Amount is Deposited" << endl;
                cout << acc;
                break;

            case 4:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                cout << "Enter Amount: ";
                cin >> amount;
                acc = b.Withdraw(accountNumber, amount);
                cout << "Amount Withdrawn" << endl;
                cout << acc;
                break;

            case 5:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                b.CloseAccount(accountNumber);
                cout << "Account is Closed" << endl;
                break;

            case 6:
                b.ShowAllAccounts();
                break;

            case 7:
                cout << "Enter Account Number to Show Passbook: ";
                cin >> accountNumber;
                acc = b.BalanceEnquiry(accountNumber);
                acc.ShowPassbook();
                break;

            case 8: {
                float amount;
                string fromCurrency, toCurrency;
                cout << "Enter amount: ";
                cin >> amount;
                cout << "Enter from currency (USD, EUR, INR): ";
                cin >> fromCurrency;
                cout << "Enter to currency (USD, EUR, INR): ";
                cin >> toCurrency;
                b.ConvertCurrency(amount, fromCurrency, toCurrency);
                break;
            }

            case 9:
                break;

            default:
                cout << "Enter a valid choice" << endl;
                exit(0);
        }
    } while (choice != 9);

    return 0;
}

Account::Account(string fname, string lname, float balance) {
    NextAccountNumber++;
    accountNumber = NextAccountNumber;
    firstName = fname;
    lastName = lname;
    this->balance = balance;
}

void Account::Deposit(float amount) {
    balance += amount;
    AddToPassbook("Deposit", amount);
}

void Account::Withdraw(float amount) {
    if (balance - amount < MIN_BALANCE)
        throw InsufficientFunds();
    balance -= amount;
    AddToPassbook("Withdrawal", amount);
}

void Account::AddToPassbook(const string& type, float amount) {
    time_t now = time(0);
    char* dt = ctime(&now);
    string date(dt);
    Transaction transaction = { date, type, amount };
    passbook.push_back(transaction);
}

void Account::ShowPassbook() const {
    cout << "Passbook for Account Number: " << accountNumber << endl;
    for (const auto& transaction : passbook) {
        cout << "Date: " << transaction.date;
        cout << "Type: " << transaction.type << ", Amount: " << transaction.amount << endl;
    }
}

void Account::setLastAccountNumber(long accountNumber) {
    NextAccountNumber = accountNumber;
}

long Account::getLastAccountNumber() {
    return NextAccountNumber;
}

ofstream& operator<<(ofstream& ofs, const Account& acc) {
    ofs << acc.accountNumber << endl;
    ofs << acc.firstName << endl;
    ofs << acc.lastName << endl;
    ofs << acc.balance << endl;
    ofs << acc.passbook.size() << endl;
    for (const auto& entry : acc.passbook) {
        ofs << entry.date << entry.type << endl << entry.amount << endl;
    }
    return ofs;
}

ifstream& operator>>(ifstream& ifs, Account& acc) {
    ifs >> acc.accountNumber;
    ifs >> acc.firstName;
    ifs >> acc.lastName;
    ifs >> acc.balance;
    size_t passbookSize;
    ifs >> passbookSize;
    acc.passbook.clear();
    for (size_t i = 0; i < passbookSize; ++i) {
        Transaction transaction;
        ifs >> ws;
        getline(ifs, transaction.date);
        getline(ifs, transaction.type);
        ifs >> transaction.amount;
        acc.passbook.push_back(transaction);
    }
    return ifs;
}

ostream& operator<<(ostream& os, const Account& acc) {
    os << "First Name: " << acc.getFirstName() << endl;
    os << "Last Name: " << acc.getLastName() << endl;
    os << "Account Number: " << acc.getAccNo() << endl;
    os << "Balance: " << acc.getBalance() << endl;
    return os;
}

Bank::Bank() {
    Account account;
    ifstream infile("Bank.data");
    if (!infile) {
        return;
    }
    while (infile >> account) {
        accounts.insert(pair<long, Account>(account.getAccNo(), account));
    }
    Account::setLastAccountNumber(account.getAccNo());
}

Account Bank::OpenAccount(const string& fname, const string& lname, float balance) {
    Account account(fname, lname, balance);
    accounts.insert(pair<long, Account>(account.getAccNo(), account));

    ofstream outfile("Bank.data", ios::trunc);
    for (const auto& entry : accounts) {
        outfile << entry.second;
    }

    return account;
}

Account Bank::BalanceEnquiry(long accountNumber) const {
    auto itr = accounts.find(accountNumber);
    return itr->second;
}

Account Bank::Deposit(long accountNumber, float amount) {
    auto itr = accounts.find(accountNumber);
    itr->second.Deposit(amount);
    return itr->second;
}

Account Bank::Withdraw(long accountNumber, float amount) {
    auto itr = accounts.find(accountNumber);
    itr->second.Withdraw(amount);
    return itr->second;
}

void Bank::CloseAccount(long accountNumber) {
    auto itr = accounts.find(accountNumber);
    if (itr != accounts.end()) {
        cout << "Account Deleted: " << itr->second;
        accounts.erase(accountNumber);

        ofstream outfile("Bank.data", ios::trunc);
        for (const auto& entry : accounts) {
            outfile << entry.second;
        }
    } else {
        cout << "Account number " << accountNumber << " not found!" << endl;
    }
}

void Bank::ShowAllAccounts() const {
    for (const auto& entry : accounts) {
        cout << "Account " << entry.first << endl << entry.second << endl;
    }
}

void Bank::ConvertCurrency(float amount, const string& fromCurrency, const string& toCurrency) {
    float convertedAmount;
    float conversionRates[3][3] = {
        {1, 0.85, 73.5},  // USD to USD, EUR, INR
        {1.18, 1, 86.5},  // EUR to USD, EUR, INR
        {0.014, 0.012, 1} // INR to USD, EUR, INR
    };

    map<string, int> currencyIndex = {{"USD", 0}, {"EUR", 1}, {"INR", 2}};
    
    if (currencyIndex.find(fromCurrency) == currencyIndex.end() || currencyIndex.find(toCurrency) == currencyIndex.end()) {
        cout << "Unsupported currency." << endl;
        return;
    }

    int fromIndex = currencyIndex[fromCurrency];
    int toIndex = currencyIndex[toCurrency];
    
    convertedAmount = amount * conversionRates[fromIndex][toIndex];
    cout << fixed << setprecision(2) << amount << " " << fromCurrency << " = " << convertedAmount << " " << toCurrency << endl;
}

Bank::~Bank() {
    ofstream outfile("Bank.data", ios::trunc);
    for (const auto& entry : accounts) {
        outfile << entry.second;
    }
}
