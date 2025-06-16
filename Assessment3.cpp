#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <exception>
#include <memory>
#include <algorithm>
#include <cctype>
#include <openssl/sha.h>

using namespace std;

const string TRANS_FILENAME = "transactions.csv";
const string USER_FILENAME = "users.dat";

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
	string username;
	TransactionType type; //Income or Expense
	string date; // format: "DD/MM/YYYY"
	TransactionCategory category; //Food, Clothes, Transportation,  Entertainment, Communication, Other
	string description;
	double amount; //positive or negative float value

public:
	//Default constructor
	Transaction();

	//Constructor.
	Transaction(const string &username, TransactionType type,
			const string &date, TransactionCategory category,
			const string &description, double amount);

	//print transaction
	void print() const;

	// getters
	string getType() const;
	double getAmount() const;
	string getCategory() const;
	const string& getDate() const;
	const string& getUsername() const;
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

template<class DataType>
class LinkedList {
protected:
	struct Node {
		DataType data;
		std::shared_ptr<Node> next;
		std::shared_ptr<Node> prev;
	};
protected:
	std::shared_ptr<Node> head;
	std::shared_ptr<Node> tail;
	int count;
protected:
	std::shared_ptr<Node> createNode(const DataType &data) {
		std::shared_ptr<Node> node = std::make_shared<Node>();
		node->data = data;
		node->next = nullptr;
		node->prev = nullptr;
		return node;
	}
public:

	LinkedList() :
			head(nullptr), tail(nullptr), count(0) {

	}

	void saveFile(ostream &ofs) const {
		std::shared_ptr<Node> node = head;
		while (node) {
			ofs << node->data.getUsername() << ",";
			ofs << node->data.getTypeInt() << ",";
			ofs << node->data.getDate() << ",";
			ofs << node->data.getCategoryInt() << ",";
			ofs << node->data.getDescription() << ",";
			ofs << node->data.getAmount() << endl;

			node = node->next;
		}
	}

	const Transaction& get(int index) const {
		if (!(index >= 0 && index < size())) {
			throw "LinkedList::get: Invalid index";
		}

		std::shared_ptr<Node> node = head;
		while (index > 0) {
			node = node->next;
			index--;
		}

		return node;
	}

	void set(int index, const DataType &data) {
		if (!(index >= 0 && index < size())) {
			throw "LinkedList::get: Invalid index";
		}

		std::shared_ptr<Node> node = head;
		while (index > 0) {
			node = node->next;
			index--;
		}

		node->data = data;
	}

	void remove(int index) {
		if (!(index >= 0 && index < size())) {
			throw "LinkedList::get: Invalid index";
		}

		std::shared_ptr<Node> node = head;
		while (index > 0) {
			node = node->next;
			index--;
		}

		if (node == head) {
			removeHead();
		} else if (node == tail) {
			removeTail();
		} else {
			node->prev->next = node->next;
			node->next->prev = node->prev;
			count--;
		}
	}

	void removeHead() {
		if (count > 0) {
			head = head->next;
			if (!head) {
				tail = nullptr;
			} else {
				head->prev = nullptr;
			}

			count--;
		}
	}

	void removeTail() {
		if (count > 0) {
			tail = tail->prev;
			if (!tail) {
				head = nullptr;
			} else {
				tail->next = nullptr;
			}

			count--;
		}
	}

	void addToHead(const DataType &data) {
		if (count == 0) {
			head = tail = createNode(data);
		} else {
			std::shared_ptr<Node> node = createNode(data);
			node->next = head;
			head->prev = node;
			head = node;
		}

		count++;
	}

	void addToTail(const DataType &data) {
		if (count == 0) {
			head = tail = createNode(data);
		} else {
			std::shared_ptr<Node> node = createNode(data);
			node->prev = tail;
			tail->next = node;
			tail = node;
		}

		count++;
	}

	int size() const {
		return count;
	}

	bool empty() const {
		return size() == 0;
	}
};

class Queue: public LinkedList<Transaction> {

public:
	Queue() {

	}

	void push(const Transaction &trans) {
		addToTail(trans);
	}

	Transaction front() const {
		if (size() > 0) {
			return head->data;
		} else {
			return Transaction();
		}
	}

	void popFront() {
		removeHead();
	}

	void print() {
		while (!empty()) {
			front().print();
			popFront();
		}
	}
};

//manage transactions
class TransactionList: public LinkedList<Transaction> {
private:
	string currentUser;
	LinkedList<Transaction> others;
public:
	//Constructor.
	TransactionList();

	void setCurrentUser(const string &username);

	//load data from file
	void loadFile(const string &filename);

	//save data to file
	void saveFile(const string &filename) const;

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
	void searchTransaction(const string &keyword, Queue &queue);

	//display all transactions
	void displayTransactions() const;

	//sort transactions by date
	void sortTransactions();
};

//represents a user.
class User {
	string username;
	string passwordEncrypted;
	bool admin;
public:
	User();

	User(const string &username, const string &password, bool admin = false);

	void writeToFile(ofstream &ofs) const;
	void readFromFile(ifstream &ifs);
	static string hash(const string &password);

	bool isAdmin() const;
	const string& getPassword() const;
	const string& getUsername() const;
};

class UserList: public LinkedList<User> {

public:
	UserList();

	bool hasUser(const string &username) const;

	bool login(const string &username, const string &password,
			bool &admin) const;

	void loadFile(const string &filename);

	void saveFile(const string &filename);
};

//show menu and handle user commands.
class App {
private:
	TransactionList transList;
	UserList userList;
	string currentUser;
public:
	//constructor.
	App();

	//show system menu.
	void runMenu();

	//sign in
	void signIn();

	//sign up
	void signUp();

	//show normal user menu.
	void runUserMenu();

	//show normal user menu.
	void runAdminMenu();

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
	app.runMenu();
}

Transaction::Transaction() :
		type(Income), date(""), category(Other), description(""), amount(0) {

}

Transaction::Transaction(const string &username, TransactionType type,
		const string &date, TransactionCategory category,
		const string &description, double amount) :
		username(username), type(type), date(date), category(category), description(
				description), amount(amount) {
}

const string& Transaction::getUsername() const {
	return username;
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
}

void TransactionList::setCurrentUser(const string &username) {
	currentUser = username;
}

void TransactionList::loadFile(const string &filename) {
	string username;
	int type;
	string date; // format: "DD/MM/YYYY"
	int category; //Housing, Transportation, Food
	string description;
	double amount; //positive or negative float value

	//open file
	ifstream ifs(filename);

	//check file exists or not.
	if (ifs.is_open()) {
		string line;

		while (getline(ifs, line)) {
			string temp;
			istringstream iss(line);

			getline(iss, username, ',');
			getline(iss, temp, ',');
			type = atoi(temp.c_str());
			getline(iss, date, ',');
			getline(iss, temp, ',');
			category = atoi(temp.c_str());
			getline(iss, description, ',');
			getline(iss, temp);
			amount = atof(temp.c_str());

			if (!iss.fail()) {
				//create object and add to array
				Transaction trans(username, (TransactionType) type, date,
						(TransactionCategory) category, description, amount);

				if (username == currentUser) {
					addToTail(trans);
				} else {
					others.addToTail(trans);
				}
			}
		}
		ifs.close();

		cout << "loaded " << size() << " transactions from " << filename << "."
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
		std::shared_ptr<Node> node = head;
		while (node) {
			ofs << node->data.getUsername() << ",";
			ofs << node->data.getTypeInt() << ",";
			ofs << node->data.getDate() << ",";
			ofs << node->data.getCategoryInt() << ",";
			ofs << node->data.getDescription() << ",";
			ofs << node->data.getAmount() << endl;

			node = node->next;
		}

		others.saveFile(ofs);
		ofs.close();

		cout << "saved " << size() << " transactions to " << filename << "."
				<< endl;
	} else {

		ostringstream oss;
		oss << "Failed to create file " << filename << ".";
		throw FileException(oss.str());
	}
}

// prompt user to select an transaction
int TransactionList::selectTransaction() {
	string temp;
	int index;

	displayTransactions();
	cout << "Your selection: ";

	getline(cin, temp);
	index = atoi(temp.c_str());
	while (!(index >= 1 && index <= size())) {
		cout << "Your selection: ";
		getline(cin, temp);
		index = atoi(temp.c_str());
	}

	return index - 1;
}

void TransactionList::addTransaction(const Transaction &trans) {
	addToTail(trans);
}

void TransactionList::modifyTransaction(int index, const Transaction &trans) {
	if (!(index >= 0 && index < size())) {
		throw "Invalid transaction index.";
	}

	set(index, trans);
}

void TransactionList::deleteTransaction(int index) {
	if (!(index >= 0 && index < size())) {
		throw "Invalid transaction index.";
	}

	//remove transaction at index
	remove(index);
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

void TransactionList::searchTransaction(const string &keyword, Queue &queue) {
	string lowercase = toLower(keyword);

	int id = 1;
	std::shared_ptr<Node> node = head;
	while (node) {
		if (node->data.getDate().find(lowercase) != string::npos
				|| toLower(node->data.getCategory()).find(lowercase)
						!= string::npos) {
			cout.setf(ios::right);
			cout << setw(2) << (id++) << ". ";
			cout.unsetf(ios::right);
			node->data.print();
		}
		node = node->next;
	}
}

void TransactionList::displayTransactions() const {
	std::shared_ptr<Node> node = head;
	int i = 0;
	while (node) {
		cout.setf(ios::right);
		cout << setw(2) << (i + 1) << ". ";
		cout.unsetf(ios::right);
		node->data.print();

		node = node->next;
		i++;
	}
}

void TransactionList::sortTransactions() {
	//selection sort
	std::shared_ptr<Node> node1 = head;
	while (node1) {
		//find the max element after i
		std::shared_ptr<Node> maxIndex = node1;
		std::shared_ptr<Node> node2 = node1;
		while (node2) {
			if (node2->data.getDateForCompare()
					> maxIndex->data.getDateForCompare()) {
				maxIndex = node2;
			}
			node2 = node2->next;
		}

		//swap maxIndex and node1
		Transaction temp = node1->data;
		node1->data = maxIndex->data;
		maxIndex->data = temp;

		node1 = node1->next;
	}
}

UserList::UserList() {

}

bool UserList::hasUser(const string &username) const {
	std::shared_ptr<Node> node = head;
	while (node) {
		if (node->data.getUsername() == username) {
			return true;
		}
		node = node->next;
	}
	return false;
}

bool UserList::login(const string &username, const string &password,
		bool &admin) const {
	admin = false;
	string passwordEncrypted = User::hash(password);

	std::shared_ptr<Node> node = head;
	while (node) {
		if (node->data.getUsername() == username
				&& node->data.getPassword() == passwordEncrypted) {
			admin = node->data.isAdmin();
			return true;
		}
		node = node->next;
	}
	return false;
}

void UserList::loadFile(const string &filename) {
	ifstream ifs(filename, ios::binary);
	if (!ifs) {
		throw FileException("No users found.");
	}

	while (ifs.peek() != EOF) {
		User u;
		u.readFromFile(ifs);
		addToTail(u);
	}
	ifs.close();
}

void UserList::saveFile(const string &filename) {
	ofstream ofs(filename, ios::binary);
	if (!ofs) {
		throw FileException("Failed to open file for writing.");
		return;
	}

	std::shared_ptr<Node> node = head;
	while (head) {
		head->data.writeToFile(ofs);
		head = head->next;
	}
	ofs.close();
}

App::App() {

}

//sign in
void App::signIn() {
	string temp;
	string username, password;

	cout << "Enter username: ";
	cin >> username;
	cout << "Enter password: ";
	cin >> password;
	getline(cin, temp); //skip '\n'

	bool admin;
	if (userList.login(username, password, admin)) {
		currentUser = username;
		transList.setCurrentUser(currentUser);
		if (admin) {
			runAdminMenu();
		} else {
			runUserMenu();
		}
	} else {
		cout << "Sign in failed." << endl;
	}
}

//sign up
void App::signUp() {
	string temp;
	string username, password;
	bool admin;

	cout << "Enter username: ";
	cin >> username;
	cout << "Enter password: ";
	cin >> password;
	cout << "Admin?(y/n): ";
	cin >> temp;
	std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower); //convert temp to lowercase
	admin = temp == "y";
	getline(cin, temp); //skip '\n'

	if (!userList.hasUser(username)) {
		User u(username, password, admin);
		userList.addToTail(u);
	} else {
		cout << "The username already exists." << endl;
	}
}

void App::runMenu() {
	try {
		userList.loadFile(USER_FILENAME);
	} catch (const exception &e) {
		cout << "Exception: " << e.what() << endl;
	}

	//Repeatedly show the menu until the user exits
	bool quit = false;
	while (!quit) {
		//show the menu
		cout << endl;
		cout << "1. Sign in" << endl;
		cout << "2. Sing up" << endl;
		cout << "0. exit" << endl;

		//Accept user input
		string option;
		getline(cin, option);

		//Implement command handling via functions
		if (option == "1") {
			signIn();
		} else if (option == "2") {
			signUp();
		} else if (option == "0") {
			//user exits
			quit = true;
		}
	}

	try {
		userList.saveFile(USER_FILENAME);
	} catch (const exception &e) {
		cout << "Exception: " << e.what() << endl;
	}
}

//show normal user menu.
void App::runUserMenu() {
	try {
		transList.loadFile(TRANS_FILENAME);
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
		cout << "4. sort transactions" << endl;
		cout << "5. display transactions" << endl;
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
			sortTransactions();
		} else if (option == "5") {
			displayTransactions();
		} else if (option == "0") {
			//user exits
			quit = true;
		}
	}

	try {
		transList.saveFile(TRANS_FILENAME);
	} catch (const exception &e) {
		cout << "Exception: " << e.what() << endl;
	}
}

//show normal user menu.
void App::runAdminMenu() {
	try {
		transList.loadFile(TRANS_FILENAME);
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
		transList.saveFile(TRANS_FILENAME);
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
	Transaction trans(currentUser, (TransactionType) type, date,
			(TransactionCategory) category, description, amount);
	return trans;
}

void App::addTransaction() {
	//create object and add to array
	Transaction trans = createTransaction();
	transList.addTransaction(trans);
}

void App::modifyTransaction() {
	if (transList.size() == 0) {
		cout << "no transactions now." << endl;
		return;
	}

	int index = transList.selectTransaction();

	//create object and update transaction at index in array
	Transaction trans = createTransaction();
	transList.modifyTransaction(index, trans);
}

void App::deleteTransaction() {
	if (transList.size() == 0) {
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
	Queue queue;
	transList.searchTransaction(keyword, queue);
	queue.print();
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

User::User() :
		admin(false) {

}

void User::writeToFile(ofstream &ofs) const {
	size_t ulen = username.size();
	size_t plen = passwordEncrypted.size();
	ofs.write(reinterpret_cast<const char*>(&ulen), sizeof(ulen));
	ofs.write(username.c_str(), ulen);
	ofs.write(reinterpret_cast<const char*>(&plen), sizeof(plen));
	ofs.write(passwordEncrypted.c_str(), plen);
}

void User::readFromFile(ifstream &ifs) {
	size_t ulen, plen;
	ifs.read(reinterpret_cast<char*>(&ulen), sizeof(ulen));
	username.resize(ulen);
	ifs.read(&username[0], ulen);
	ifs.read(reinterpret_cast<char*>(&plen), sizeof(plen));
	passwordEncrypted.resize(plen);
	ifs.read(&passwordEncrypted[0], plen);
}

string User::hash(const string &password) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256((unsigned char*) password.c_str(), password.size(), hash);
	std::stringstream ss;
	for (unsigned char i : hash) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int) i;
	}
	return ss.str();
}

User::User(const string &username, const string &password, bool admin) :
		username(username), passwordEncrypted(hash(password)), admin(admin) {
}

bool User::isAdmin() const {
	return admin;
}

const string& User::getPassword() const {
	return passwordEncrypted;
}

const string& User::getUsername() const {
	return username;
}
