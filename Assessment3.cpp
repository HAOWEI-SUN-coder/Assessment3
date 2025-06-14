#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <exception>
using namespace std;

const string FILENAME = "transactions.csv";

enum TransactionType {
	Income, Expense
};

enum TransactionCategory {
	Salary,
	Cash,
	Gift,
	Food,
	Clothes,
	Transportation,
	Entertainment,
	Communication,
	Other
};

// file error exception.
class FileException: public exception {
	string message;

public:
	FileException(const string &message) :
			message(message) {
	}

	const char* what() const noexcept {
		return message.c_str();
	}
};

//represents a transaction.
class Transaction {
private:
	TransactionType type; //Income or Expense
	string date; // format: "DD/MM/YYYY"
	TransactionCategory category; //Food, Clothes, Transportation,  Entertainment, Communication, Other
	string description;
	double amount; //positive or negative float value

public:
	//Default constructor
	Transaction();

	//Constructor.
	Transaction(TransactionType type, const string &date,
			TransactionCategory category, const string &description,
			double amount);

	//print transaction
	void print() const;

	// getters
	string getType() const;
	double getAmount() const;
	string getCategory() const;
	const string& getDate() const;
	string getDateForCompare() const; //get date in YYYYMMDD format
	const string& getDescription() const;
	TransactionType getTypeInt() const;
	TransactionCategory getCategoryInt() const;

	// setters
	void setType(TransactionType type);
	void setAmount(double amount);
	void setCategory(TransactionCategory category);
	void setDate(const string &date);
	void setDescription(const string &description);
};

//The maximum number of transactions
const int MAX_SIZE = 100;

//manage transactions
class TransactionList {
private:
	Transaction transactions[MAX_SIZE];
	int size; //number of transactions in array.
public:
	//Constructor.
	TransactionList();

	//load data from file
	void loadFile(const string &filename);

	//save data to file
	void saveFile(const string &filename) const;

	// get number of transactions
	int count() const;

	// prompt user to select an transaction
	// return a number in range [0, size)
	int selectTransaction();

	//add Transaction
	void addTransaction(const Transaction &trans);

	//modify transaction by the index of transaction in array.
	void modifyTransaction(int index, const Transaction &trans);

	//delete transaction by the index of transaction in array.
	void deleteTransaction(int index);

	//search transaction (linear search) by category or date.
	void searchTransaction(const string &keyword);

	//display all transactions
	void displayTransactions() const;

	//sort transactions by date
	void sortTransactions();
};

//show menu and handle user commands.
class App {
private:
	TransactionList transList;

public:
	//constructor.
	App();

	//show system menu.
	void showMenu();

	//prompt user to create a transaction.
	Transaction createTransaction();

	//add a transaction
	void addTransaction();

	//modify a transaction
	void modifyTransaction();

	//delete a transaction
	void deleteTransaction();

	//search transactions
	void searchTransaction();

	//display all transactions
	void displayTransactions() const;

	//sort transactions
	void sortTransactions();

	//print table header
	void printTableHeader() const;

	//validate date (DD/MM/YYYY)
	bool validateDate(const string &input) const;

	//validate amount (a number)
	bool validateAmount(const string &input) const;
};

int main() {
	App app;
	app.showMenu();
}

Transaction::Transaction() :
		type(Income), date(""), category(Other), description(""), amount(0) {

}

Transaction::Transaction(TransactionType type, const string &date,
		TransactionCategory category, const string &description, double amount) :
		type(type), date(date), category(category), description(description), amount(
				amount) {
}

//print transaction
void Transaction::print() const {
	cout.setf(ios::left);
	cout.setf(ios::fixed);

	cout << setw(10) << getType();
	cout << setw(15) << date;
	cout << setw(20) << getCategory();
	cout << setw(15) << setprecision(2) << amount;
	cout << description;
	cout << endl;

	cout.unsetf(ios::left);
}

double Transaction::getAmount() const {
	return amount;
}

string Transaction::getType() const {
	switch (type) {
	case Income:
		return "Income";
	case Expense:
		return "Expense";
	default:
		return "";
	}
}

string Transaction::getCategory() const {
	switch (category) {
	case Salary:
		return "Salary";
	case Cash:
		return "Cash";
	case Gift:
		return "Gift";
	case Food:
		return "Food";
	case Clothes:
		return "Clothes";
	case Transportation:
		return "Transportation";
	case Entertainment:
		return "Entertainment";
	case Communication:
		return "Communication";
	default:
		return "Other";
	}
}

const string& Transaction::getDate() const {
	return date;
}

//get date in YYYYMMDD format
string Transaction::getDateForCompare() const {
	//get sub strings for dd, mm, yyyy
	string dd = date.substr(0, 2);
	string mm = date.substr(3, 2);
	string yyyy = date.substr(6, 4);

	//convert string to integer
	int d = atoi(dd.c_str());
	int m = atoi(mm.c_str());
	int y = atoi(yyyy.c_str());

	//convert integers to yyyymmdd format
	ostringstream oss;
	oss << y;
	oss << setw(2) << setfill('0') << m;
	oss << setw(2) << setfill('0') << d;

	return oss.str();
}

void Transaction::setAmount(double amount) {
	this->amount = amount;
}

void Transaction::setType(TransactionType type) {
	this->type = type;
}

void Transaction::setCategory(TransactionCategory category) {
	this->category = category;
}

void Transaction::setDate(const string &date) {
	this->date = date;
}

void Transaction::setDescription(const string &description) {
	this->description = description;
}

const string& Transaction::getDescription() const {
	return description;
}

TransactionType Transaction::getTypeInt() const {
	return type;
}
TransactionCategory Transaction::getCategoryInt() const {
	return category;
}
TransactionList::TransactionList() {
	size = 0;
}

void TransactionList::loadFile(const string &filename) {
	int type;
	string date; // format: "DD/MM/YYYY"
	int category; //Housing, Transportation, Food
	string description;
	double amount; //positive or negative float value

	//open file
	ifstream ifs(filename);

	//check file exists or not.
	if (ifs.is_open()) {
		string temp;
		//read all transactions
		while (getline(ifs, temp, ',')) {
			//if we get the date correctly, continue to read other data
			type = atoi(temp.c_str());
			getline(ifs, date, ',');
			getline(ifs, temp, ',');
			category = atoi(temp.c_str());
			getline(ifs, description, ',');
			getline(ifs, temp);
			amount = atof(temp.c_str());

			//create object and add to array
			Transaction trans((TransactionType) type, date,
					(TransactionCategory) category, description, amount);
			transactions[size++] = trans;
		}
		ifs.close();

		cout << "loaded " << size << " transactions from " << filename << "."
				<< endl;
	} else {
		ostringstream oss;
		oss << "Failed to open file " << filename << ".";

		throw FileException(oss.str());
	}
}

void TransactionList::saveFile(const string &filename) const {
	//create file
	ofstream ofs(filename);

	if (ofs.is_open()) {
		//output all transactions to file
		for (int i = 0; i < size; i++) {
			ofs << transactions[i].getTypeInt() << ",";
			ofs << transactions[i].getDate() << ",";
			ofs << transactions[i].getCategoryInt() << ",";
			ofs << transactions[i].getDescription() << ",";
			ofs << transactions[i].getAmount() << endl;
		}
		ofs.close();

		cout << "saved " << size << " transactions to " << filename << "."
				<< endl;
	} else {

		ostringstream oss;
		oss << "Failed to create file " << filename << ".";
		throw FileException(oss.str());
	}
}

// get number of transactions
int TransactionList::count() const {
	return size;
}

// prompt user to select an transaction
int TransactionList::selectTransaction() {
	string temp;
	int index;

	displayTransactions();
	cout << "Your selection: ";

	getline(cin, temp);
	index = atoi(temp.c_str());
	while (!(index >= 1 && index <= size)) {
		cout << "Your selection: ";
		getline(cin, temp);
		index = atoi(temp.c_str());
	}

	return index - 1;
}

void TransactionList::addTransaction(const Transaction &trans) {
	transactions[size++] = trans;
}

void TransactionList::modifyTransaction(int index, const Transaction &trans) {
	if (!(index >= 0 && index < size)) {
		throw "Invalid transaction index.";
	}

	transactions[index] = trans;
}

void TransactionList::deleteTransaction(int index) {
	if (!(index >= 0 && index < size)) {
		throw "Invalid transaction index.";
	}

	//remove transaction at index
	for (int i = index; i < size - 1; i++) {
		transactions[i] = transactions[i + 1];
	}
	size--;
}

static string toLower(const string &str) {
	string temp;

	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			temp += (char) (str[i] + 'a' - 'A');
		} else {
			temp += str[i];
		}
	}

	return temp;
}
void TransactionList::searchTransaction(const string &keyword) {
	string lowercase = toLower(keyword);

	int id = 1;
	for (int i = 0; i < size; i++) {
		if (transactions[i].getDate().find(lowercase) != string::npos
				|| toLower(transactions[i].getCategory()).find(lowercase)
						!= string::npos) {
			cout.setf(ios::right);
			cout << setw(2) << (id++) << ". ";
			cout.unsetf(ios::right);
			transactions[i].print();
		}
	}
}

void TransactionList::displayTransactions() const {
	for (int i = 0; i < size; i++) {
		cout.setf(ios::right);
		cout << setw(2) << (i + 1) << ". ";
		cout.unsetf(ios::right);
		transactions[i].print();
	}
}

void TransactionList::sortTransactions() {
	//selection sort
	for (int i = 0; i < size; i++) {
		//find the max element after i
		int maxIndex = i;
		for (int j = i; j < size; j++) {
			if (transactions[j].getDateForCompare()
					> transactions[maxIndex].getDateForCompare()) {
				maxIndex = j;
			}
		}

		//swap maxIndex and i
		Transaction temp = transactions[i];
		transactions[i] = transactions[maxIndex];
		transactions[maxIndex] = temp;
	}
}

App::App() {

}

void App::showMenu() {
	try {
		transList.loadFile(FILENAME);
	} catch (const exception &e) {
		cout << "Exception: " << e.what() << endl;
	}

	//Repeatedly show the menu until the user exits
	bool quit = false;
	while (!quit) {
		//show the menu
		cout << endl;
		cout << "1. add transaction" << endl;
		cout << "2. modify transaction" << endl;
		cout << "3. delete transaction" << endl;
		cout << "4. search transactions" << endl;
		cout << "5. sort transactions" << endl;
		cout << "6. display transactions" << endl;
		cout << "0. exit" << endl;

		//Accept user input
		string option;
		getline(cin, option);

		//Implement command handling via functions
		if (option == "1") {
			addTransaction();
		} else if (option == "2") {
			modifyTransaction();
		} else if (option == "3") {
			deleteTransaction();
		} else if (option == "4") {
			searchTransaction();
		} else if (option == "5") {
			sortTransactions();
		} else if (option == "6") {
			displayTransactions();
		} else if (option == "0") {
			//user exits
			quit = true;
		}
	}

	try {
		transList.saveFile(FILENAME);
	} catch (const exception &e) {
		cout << "Exception: " << e.what() << endl;
	}
}

//prompt user to create a transaction.
Transaction App::createTransaction() {
	string temp;

	int type;
	string date; // format: "DD/MM/YYYY"
	int category; //Housing, Transportation, Food
	string description;
	double amount; //positive or negative float value

	//enter type
	cout << "Enter type(1-Income, 2-Expense): ";
	getline(cin, temp);
	type = atoi(temp.c_str());
	while (!(type >= 1 && type <= 2)) {
		cout << "Enter type(1-Income, 2-Expense): ";
		getline(cin, temp);
		type = atoi(temp.c_str());
	}
	type--; //type is 0 or 1

	//enter date
	cout << "Enter date(DD/MM/YYYY): ";
	getline(cin, date);
	while (!validateDate(date)) {
		cout << "Enter date(DD/MM/YYYY): ";
		getline(cin, date);
	}

	//enter category
	if (type == 0) {
		//Income
		cout << "Enter category(1-Salary, 2-Cash, 3-Gift): ";
		getline(cin, temp);
		category = atoi(temp.c_str());
		while (!(category >= 1 && category <= 3)) {
			cout << "Enter category(1-Salary, 2-Cash, 3-Gift): ";
			getline(cin, temp);
			category = atoi(temp.c_str());
		}
		category--; //category is 0-2
	} else {
		//Exponse
		cout
				<< "Enter category(1-Food, 2-Clothes, 3-Transportation, 4-Entertainment, 5-Communication, 6-Other): ";
		getline(cin, temp);
		category = atoi(temp.c_str());
		while (!(category >= 1 && category <= 6)) {
			cout
					<< "Enter category(1-Food, 2-Clothes, 3-Transportation, 4-Entertainment, 5-Communication, 6-Other): ";
			getline(cin, temp);
			category = atoi(temp.c_str());
		}
		category += 2; //category is 3-8
	}

	//enter description
	cout << "Enter description: ";
	getline(cin, description);

	//enter amount
	cout << "Enter amount: ";
	getline(cin, temp);
	while (!validateAmount(temp)) {
		cout << "Enter amount: ";
		getline(cin, temp);
	}
	amount = atof(temp.c_str());

	//create object
	Transaction trans((TransactionType) type, date,
			(TransactionCategory) category, description, amount);
	return trans;
}

void App::addTransaction() {
	//create object and add to array
	Transaction trans = createTransaction();
	transList.addTransaction(trans);
}

void App::modifyTransaction() {
	if (transList.count() == 0) {
		cout << "no transactions now." << endl;
		return;
	}

	int index = transList.selectTransaction();

	//create object and update transaction at index in array
	Transaction trans = createTransaction();
	transList.modifyTransaction(index, trans);
}

void App::deleteTransaction() {
	if (transList.count() == 0) {
		cout << "no transactions now." << endl;
		return;
	}

	int index = transList.selectTransaction();
	transList.deleteTransaction(index);
}

void App::searchTransaction() {
	string keyword;

	cout << "Enter keyword: ";
	getline(cin, keyword);

	printTableHeader();
	transList.searchTransaction(keyword);
}

void App::displayTransactions() const {
	printTableHeader();
	transList.displayTransactions();
}

void App::sortTransactions() {
	transList.sortTransactions();
	displayTransactions();
}

void App::printTableHeader() const {
	cout.setf(ios::left);
	cout.setf(ios::fixed);
	cout << "    ";
	cout << setw(10) << "Type";
	cout << setw(15) << "Date";
	cout << setw(20) << "Category";
	cout << setw(15) << "Amount";
	cout << "Description";
	cout << endl;

	cout.unsetf(ios::left);
}

//validate date (DD/MM/YYYY)
bool App::validateDate(const string &input) const {
	if (input.size() != 10) {
		return false;
	}

	if (input[2] != '/' || input[5] != '/') {
		return false;
	}

	string dd = input.substr(0, 2);
	string mm = input.substr(3, 2);
	string yyyy = input.substr(6, 4);

	int d = atoi(dd.c_str());
	int m = atoi(mm.c_str());
	int y = atoi(yyyy.c_str());

	return d >= 1 && d <= 31 && m >= 1 && m <= 12 && y >= 2000;
}

//validate amount (a number)
bool App::validateAmount(const string &input) const {
	try {
		size_t pos;
		std::stod(input, &pos); // Try to convert to double
		return pos == input.length(); // Ensure whole string was used
	} catch (const std::invalid_argument &e) {
		return false; // Not a number
	} catch (const std::out_of_range &e) {
		return false; // Out of range for double
	}
}
