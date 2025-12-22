#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <memory>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <conio.h>
#include <functional>

using namespace std;

// Base class for Users
class User {
protected:
    string id;
    string name;
    string email;

    string type;
public:
    User(string i, string n, string e, string t) : id(i), name(n), email(e), type(t) {}
    virtual ~User() {}
    string getId() const { return id; }
    string getName() const { return name; }
    string getEmail() const { return email; }
    string getType() const { return type; }
    virtual string getDetails() const = 0;
    virtual void save(ofstream& file) const = 0;
    static unique_ptr<User> load(const string& line);
};

class Student : public User {
private:
    string studentId;
    string department;
public:

    Student(string i, string n, string e, string sid, string dept)
        : User(i, n, e, "Student"), studentId(sid), department(dept) {}
    string getDetails() const override {
        return "Student ID: " + studentId + ", Department: " + department;
    }
    void save(ofstream& file) const override {
        file << "Student|" << id << "|" << name << "|" << email << "|" << studentId << "|" << department << endl;
    }
    string getStudentId() const { return studentId; }
    string getDepartment() const { return department; }
};

class Faculty : public User {
private:

    string facultyId;
    string department;
    string position;
public:
    Faculty(string i, string n, string e, string fid, string dept, string pos)
        : User(i, n, e, "Faculty"), facultyId(fid), department(dept), position(pos) {}
    string getDetails() const override {
        return "Faculty ID: " + facultyId + ", Department: " + department + ", Position: " + position;
    }
    void save(ofstream& file) const override {
        file << "Faculty|" << id << "|" << name << "|" << email << "|" << facultyId << "|" << department << "|" << position << endl;
    }
    string getFacultyId() const { return facultyId; }
    string getDepartment() const { return department; }
    string getPosition() const { return position; }
};



unique_ptr<User> User::load(const string& line) {
    stringstream ss(line);
    string type;
    getline(ss, type, '|');
    if (type == "Student") {
        string id, name, email, studentId, department;
        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, email, '|');
        getline(ss, studentId, '|');
        getline(ss, department, '|');
        return make_unique<Student>(id, 

name, email, studentId, department);
    } else if (type == "Faculty") {
        string id, name, email, facultyId, department, position;
        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, email, '|');
        getline(ss, facultyId, '|');
        getline(ss, department, '|');
        getline(ss, position, '|');
        return make_unique<Faculty>(id, name, email, facultyId, department, position);
    }
    return nullptr;
}

// Book class
class Book {
private:
    string isbn;

    string title;
    string author;
    int totalCopies;
    int availableCopies;
public:
    Book(string i, string t, string a, int copies)
        : isbn(i), title(t), author(a), totalCopies(copies), availableCopies(copies) {}
    string getISBN() const { return isbn; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    int getAvailableCopies() const { return availableCopies; }
    int getTotalCopies() const { return totalCopies; }
    bool borrow(int qty = 1) {
        if (availableCopies >= qty) {
            availableCopies -= qty;
            return true;
        }

        return false;
    }
    void returnBook(int qty = 1) {
        availableCopies += qty;
        if (availableCopies > totalCopies) availableCopies = totalCopies;
    }
    void save(ofstream& file) const {
        file << isbn << "|" << title << "|" << author << "|" << totalCopies << "|" << availableCopies << endl;
    }
    static Book load(const string& line) {
        stringstream ss(line);
        string isbn, title, author, totalStr, availStr;
        getline(ss, isbn, '|');
        getline(ss, title, '|');
        getline(ss, author, '|');
        getline(ss, totalStr, '|');
        getline(ss, availStr, '|');

        Book b(isbn, title, author, stoi(totalStr));
        b.availableCopies = stoi(availStr);
        return b;
    }
};

// Transaction class
class Transaction {
private:
    string userId;
    string isbn;
    time_t borrowDate;
    time_t dueDate;
    bool returned;
    time_t returnDate;
    double fine;
    int quantity;
public:
    Transaction(string uid, string i, time_t borrow, time_t due, int qty = 1)
        : userId(uid), isbn(i), 

borrowDate(borrow), dueDate(due), returned(false), returnDate(0), fine(0.0), quantity(qty) {}
    string getUserId() const { return userId; }
    string getISBN() const { return isbn; }
    time_t getBorrowDate() const { return borrowDate; }
    time_t getDueDate() const { return dueDate; }
    bool isReturned() const { return returned; }
    double getFine() const { return fine; }
    int getQuantity() const { return quantity; }
    void returnBook(time_t retDate) {
        returned = true;
        returnDate = retDate;
        if (retDate > dueDate) {
            double minutesLate = difftime(retDate, dueDate) / 60;
            fine = minutesLate * 1.0; // Rs. 1 per minute

        }
    }
    void save(ofstream& file) const {
        file << userId << "|" << isbn << "|" << borrowDate << "|" << dueDate << "|" << (returned ? "1" : "0") << "|" << returnDate << "|" << fixed << setprecision(2) << fine << "|" << quantity << endl;
    }
    static Transaction load(const string& line) {
        stringstream ss(line);
        string userId, isbn, borrowStr, dueStr, returnedStr, returnStr, fineStr, qtyStr;
        getline(ss, userId, '|');
        getline(ss, isbn, '|');
        getline(ss, borrowStr, '|');
        getline(ss, dueStr, '|');
        getline(ss, returnedStr, '|');
        getline(ss, returnStr, '|');
        getline(ss, fineStr, '|');

        getline(ss, qtyStr, '|');
        Transaction t(userId, isbn, stol(borrowStr), stol(dueStr), stoi(qtyStr));
        if (returnedStr == "1") {
            t.returnBook(stol(returnStr));
        }
        return t;
    }
};

// Notification system using polymorphism
class NotificationSender {
public:
    virtual ~NotificationSender() {}
    virtual void sendNotification(const User& user, const string& message) = 0;
};

class EmailNotification : public NotificationSender {
public:

    void sendNotification(const User& user, const string& message) override {
        cout << "Email sent to " << user.getEmail() << ": " << message << endl;
    }
};

class SMSNotification : public NotificationSender {
public:
    void sendNotification(const User& user, const string& message) override {
        cout << "SMS sent to " << user.getName() << ": " << message << endl;
    }
};

// Librarian class - manages everything
class Librarian {
private:
    vector<unique_ptr<User>> users;

    vector<Book> books;
    vector<Transaction> transactions;
    unique_ptr<NotificationSender> notifier;
    const string usersFile = "users.txt";
    const string booksFile = "books.txt";
    const string transactionsFile = "transactions.txt";
    const string librarianFile = "librarian.txt";
    int nextUserId;
    string librarianUsername;
    string librarianPassword;

    void loadUsers() {
        ifstream file(usersFile);
        if (!file) return;
        users.clear();
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                auto user = User::load(line);
                if (user) 

users.push_back(move(user));
            }
        }
        file.close();
    }

    void saveUsers() {
        ofstream file(usersFile);
        for (const auto& user : users) {
            user->save(file);
        }
        file.close();
    }

    void loadBooks() {
        ifstream file(booksFile);
        if (!file) return;
        books.clear();
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {

                books.push_back(Book::load(line));
            }
        }
        file.close();
    }

    bool saveBooks() {
        ofstream file(booksFile);
        if (!file) {
            return false;
        }
        for (const auto& book : books) {
            book.save(file);
        }
        file.close();
        return true;
    }

    void loadTransactions() {
        ifstream file(transactionsFile);

        if (!file) return;
        transactions.clear();
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                transactions.push_back(Transaction::load(line));
            }
        }
        file.close();
    }

    bool saveTransactions() {
        ofstream file(transactionsFile);
        if (!file) {
            return false;
        }
        for (const auto& trans : transactions) {
            trans.save(file);
        }

        file.close();
        return true;
    }

    void loadLibrarian() {
        ifstream file(librarianFile);
        if (!file) return;
        string line;
        if (getline(file, line)) {
            librarianUsername = line;
        }
        if (getline(file, line)) {
            librarianPassword = line;
        }
        file.close();
    }

    void saveLibrarian() {
        ofstream file(librarianFile);
        if (!file) return;
        file << librarianUsername << endl;

        file << librarianPassword << endl;
        file.close();
    }

public:
    Librarian(unique_ptr<NotificationSender> sender = make_unique<EmailNotification>())
        : notifier(move(sender)), nextUserId(1), librarianUsername("admin"), librarianPassword("password") {
        loadUsers();
        loadBooks();
        loadTransactions();
        loadLibrarian();
        // Set nextUserId to max existing ID + 1
        int maxId = 0;
        for (const auto& user : users) {
            try {
                int id = stoi(user->getId());

                if (id > maxId) maxId = id;
            } catch (...) {
                // Ignore invalid IDs
            }
        }
        nextUserId = maxId + 1;
    }

    string getLibrarianUsername() const { return librarianUsername; }
    string getLibrarianPassword() const { return librarianPassword; }
    void setCredentials(const string& username, const string& password) {
        librarianUsername = username;
        librarianPassword = password;
        saveLibrarian();
    }

    // CRUD operations for books
    void addBook(const Book& book) {

        if (book.getISBN().empty() || book.getTitle().empty() || book.getAuthor().empty() || book.getTotalCopies() <= 0) {
            cout << "Invalid book details!" << endl;
            return;
        }
        auto it = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == book.getISBN(); });
        if (it != books.end()) {
            cout << "Book with this ISBN already exists!" << endl;
        } else {
            books.push_back(book);
            if (!saveBooks()) {
                books.pop_back();
                cout << "Book added but not saved to file!" << endl;
            } else {

                cout << "Book added successfully!" << endl;
            }
        }
    }

    void updateBook(const string& isbn, const Book& newBook) {
        auto it = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == isbn; });
        if (it != books.end()) {
            if (it->getAvailableCopies() > 0) {
                *it = newBook;
                saveBooks();
                cout << "Book updated successfully!" << endl;
            } else {
                cout << "Book is not available for update (no copies available)!" << endl;
            }

        } else {
            cout << "Book not found!" << endl;
        }
    }

    void deleteBook(const string& isbn) {
        auto it = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == isbn; });
        if (it != books.end()) {
            books.erase(it);
            saveBooks();
            cout << "Book deleted successfully!" << endl;
        } else {
            cout << "Book not found!" << endl;
        }
    }

    void listBooks() {
        if (books.empty()) {

            cout << "No books in the library." << endl;
            return;
        }
        cout << "Books in the library:" << endl;
        for (const auto& b : books) {
            cout << "ISBN: " << b.getISBN() << ", Title: " << b.getTitle() << ", Author: " << b.getAuthor()
                 << ", Available: " << b.getAvailableCopies() << "/" << b.getTotalCopies() << endl;
        }
    }

    void searchByTitle(const string& title) {
        if (books.empty()) {
            cout << "No books in the library." << endl;
            return;
        }

        string lowerTitle = title;
        transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
        vector<Book> results;
        for (const auto& b : books) {
            string lowerBookTitle = b.getTitle();
            transform(lowerBookTitle.begin(), lowerBookTitle.end(), lowerBookTitle.begin(), ::tolower);
            if (lowerBookTitle.find(lowerTitle) != string::npos) {
                results.push_back(b);
            }
        }
        if (results.empty()) {
            cout << "No books found with title containing: " << title << endl;
        } else {
            cout << "Search results for title '" << title << "':" << endl;

            for (const auto& b : results) {
                cout << "ISBN: " << b.getISBN() << ", Title: " << b.getTitle() << ", Author: " << b.getAuthor()
                     << ", Available: " << b.getAvailableCopies() << "/" << b.getTotalCopies() << endl;
            }
        }
    }

    void searchByISBN(const string& isbn) {
        if (books.empty()) {
            cout << "No books in the library." << endl;
            return;
        }
        string lowerISBN = isbn;
        transform(lowerISBN.begin(), lowerISBN.end(), lowerISBN.begin(), ::tolower);

        vector<Book> results;
        for (const auto& b : books) {
            string lowerBookISBN = b.getISBN();
            transform(lowerBookISBN.begin(), lowerBookISBN.end(), lowerBookISBN.begin(), ::tolower);
            if (lowerBookISBN.find(lowerISBN) != string::npos) {
                results.push_back(b);
            }
        }
        if (results.empty()) {
            cout << "No books found with ISBN containing: " << isbn << endl;
        } else {
            cout << "Search results for ISBN '" << isbn << "':" << endl;
            for (const auto& b : results) {
                cout << "ISBN: " << b.getISBN() << ", Title: " << b.getTitle() << ", Author: " << b.getAuthor()

                     << ", Available: " << b.getAvailableCopies() << "/" << b.getTotalCopies() << endl;
            }
        }
    }

    // Manage users
    string addStudent(const string& name, const string& email, const string& studentId, const string& department) {
        string userId = to_string(nextUserId++);
        users.push_back(make_unique<Student>(userId, name, email, studentId, department));
        saveUsers();
        return userId;
    }

    string addFaculty(const string& name, const string& email, const string& 

facultyId, const string& department, const string& position) {
        string userId = to_string(nextUserId++);
        users.push_back(make_unique<Faculty>(userId, name, email, facultyId, department, position));
        saveUsers();
        return userId;
    }

    void listUsers() {
        if (users.empty()) {
            cout << "No users registered." << endl;
            return;
        }
        cout << "Registered users:" << endl;
        for (const auto& user : users) {
            cout << "ID: " << user->getId() << ", Name: " << user->getName() << ", Type: " << 

user->getType()
                 << ", " << user->getDetails() << endl;
        }
    }

    // Borrow and return books
    void borrowBook(const string& userId, const string& isbn, int qty = 1) {
        if (!userExists(userId)) {
            cout << "User not found!" << endl;
            return;
        }
        auto bookIt = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == isbn; });
        if (bookIt == books.end()) {
            cout << "Book not found!" << endl;
            return;
        }

        // Check for existing active transaction for the same user and book
        auto existingTrans = find_if(transactions.begin(), transactions.end(), [&](const Transaction& t) {
            return t.getUserId() == userId && t.getISBN() == isbn && !t.isReturned();
        });
        if (existingTrans != transactions.end()) {
            cout << "You already have this book borrowed and not returned. Please return it first." << endl;
            return;
        }

        if (!bookIt->borrow(qty)) {
            cout << "No copies available!" << endl;
            return;

        }

        time_t now = time(nullptr);
        time_t dueDate = now + 6 * 60; // 6 minutes for testing

        transactions.emplace_back(userId, isbn, now, dueDate, qty);
        saveBooks();
        saveTransactions();

        auto userIt = find_if(users.begin(), users.end(), [&](const unique_ptr<User>& u) { return u->getId() == userId; });
        if (userIt != users.end()) {
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&dueDate));
            string message = "You have borrowed '" + bookIt->getTitle() + "'. Due date: " + buffer + ". Please return on time to avoid fines.";
            notifier->sendNotification(**userIt, message);
        }
        cout << "Book borrowed successfully!" << endl;
    }

    void returnBook(const string& userId, const string& isbn, int qty = 1) {
        auto transIt = find_if(transactions.begin(), transactions.end(),
            [&](const Transaction& t) { return t.getUserId() == userId && t.getISBN() == isbn && !t.isReturned(); });
        if (transIt == transactions.end()) {
            cout << "No active transaction found!" << endl;
            return;
        }


        if (transIt->getQuantity() != qty) {
            cout << "Quantity mismatch! Expected: " << transIt->getQuantity() << ", Provided: " << qty << endl;
            return;
        }

        time_t now = time(nullptr);
        transIt->returnBook(now);

        auto bookIt = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == isbn; });
        if (bookIt != books.end()) {
            bookIt->returnBook(qty);
        }

        saveBooks();
        saveTransactions();

        auto userIt = find_if(users.begin(), users.end(), [&](const unique_ptr<User>& u) { return u->getId() == userId; });
        if (transIt->getFine() > 0) {
            if (userIt != users.end()) {
                string message = "Book returned. Fine due: Rs. " + to_string(transIt->getFine()) + " for late return.";
                notifier->sendNotification(**userIt, message);
            }
            cout << "Book returned. Fine: Rs. " << fixed << setprecision(2) << transIt->getFine() << endl;
        } else {
            if (userIt != users.end()) {
                string message = "Book returned successfully. Thank you!";
                notifier->sendNotification(**userIt, message);
            }

            cout << "Book returned successfully!" << endl;
        }
    }

    // Send reminders
    void sendReminders() {
        time_t now = time(nullptr);
        int reminderCount = 0;
        for (const auto& trans : transactions) {
            if (!trans.isReturned()) {
                auto userIt = find_if(users.begin(), users.end(), [&](const unique_ptr<User>& u) { return u->getId() == trans.getUserId(); });
                if (userIt != users.end()) {
                    auto bookIt = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == trans.getISBN(); });
                    if (bookIt != books.end()) {

                        time_t dueDate = trans.getDueDate();
                        char buffer[80];
                        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&dueDate));
                        string message = "Reminder: '" + bookIt->getTitle() + "' is due on " + buffer + ". Please return it to avoid fines.";
                        notifier->sendNotification(**userIt, message);
                        reminderCount++;
                    }
                }
            }
        }
        cout << reminderCount << " reminders sent to users with outstanding books." << endl;
    }

    void sendOverdueReminders() {
        time_t now = time(nullptr);
        int overdueCount = 0;
        for (const auto& trans : transactions) {
            if (!trans.isReturned() && trans.getDueDate() < now) {
                auto userIt = find_if(users.begin(), users.end(), [&](const unique_ptr<User>& u) { return u->getId() == trans.getUserId(); });
                if (userIt != users.end()) {
                    auto bookIt = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == trans.getISBN(); });
                    if (bookIt != books.end()) {
                        double minutesLate = difftime(now, trans.getDueDate()) / 60;
                        double fine = minutesLate * 1.0;
                        time_t dueDate = 

trans.getDueDate();
                        char buffer[80];
                        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&dueDate));
                        string message = "OVERDUE: '" + bookIt->getTitle() + "' was due on " + buffer + ". You are " +
                                       to_string((int)minutesLate) + " minutes late. Fine: Rs. " + to_string(fine) + ". Please return immediately!";
                        notifier->sendNotification(**userIt, message);
                        overdueCount++;
                    }
                }
            }
        }
        cout << overdueCount << " overdue reminders sent." << endl;

    }

    void listTransactions() {
        if (transactions.empty()) {
            cout << "No transactions found." << endl;
            return;
        }
        cout << "All Transactions:" << endl;
        cout << left << setw(10) << "User ID" << setw(20) << "User Name" << setw(15) << "ISBN" << setw(30) << "Title"
             << setw(10) << "Quantity" << setw(15) << "Borrow Date" << setw(15) << "Due Date" << setw(10) << "Status" << setw(10) << "Fine" << endl;
        cout << string(135, '-') << endl;
        for (const auto& trans : transactions) {
            auto userIt = find_if(users.begin(), users.end(), [&](const unique_ptr<User>& u) { return u->getId() == trans.getUserId(); 

});
            auto bookIt = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == trans.getISBN(); });
            if (userIt != users.end() && bookIt != books.end()) {
                char borrowBuf[80], dueBuf[80];
                time_t borrowTime = trans.getBorrowDate();
                time_t dueTime = trans.getDueDate();
                strftime(borrowBuf, sizeof(borrowBuf), "%Y-%m-%d %H:%M:%S", localtime(&borrowTime));
                strftime(dueBuf, sizeof(dueBuf), "%Y-%m-%d %H:%M:%S", localtime(&dueTime));
                string status = trans.isReturned() ? "Returned" : "Borrowed";
                cout << left << setw(10) << trans.getUserId() << setw(20) << (*userIt)->getName() << setw(15) << trans.getISBN()
                     << setw(30) << bookIt->getTitle() << setw(10) << trans.getQuantity() << setw(15) << borrowBuf
                     << setw(15) << dueBuf << setw(10) << status << setw(10) << fixed << setprecision(2) << trans.getFine() << endl;
            }
        }
    }

    bool userExists(const string& userId) {
        return find_if(users.begin(), users.end(), [&](const unique_ptr<User>& u) { return u->getId() == userId; }) != users.end();
    }

    const vector<unique_ptr<User>>& getUsers() const { return users; }

    const vector<Book>& getBooks() const { return books; }

    // Additional functions
    void viewUserBorrowedBooks(const string& userId) {
        cout << "Books borrowed by User ID: " << userId << endl;
        bool found = false;
        for (const auto& trans : transactions) {
            if (trans.getUserId() == userId && !trans.isReturned()) {
                auto bookIt = find_if(books.begin(), books.end(), [&](const Book& b) { return b.getISBN() == trans.getISBN(); });
                if (bookIt != books.end()) {
                    cout << "ISBN: " << trans.getISBN() << ", Title: " << bookIt->getTitle() << ", Quantity: " << trans.getQuantity() << endl;

                    found = true;
                }
            }
        }
        if (!found) {
            cout << "No books borrowed by this user." << endl;
        }
    }

    void listBorrowedBooks() {
        if (transactions.empty()) {
            cout << "No borrowed books." << endl;
            return;
        }
        cout << "All Borrowed Books:" << endl;
        for (const auto& trans : transactions) {
            if (!trans.isReturned()) {
                auto bookIt = find_if(books.begin(), books.end(), [&]

(const Book& b) { return b.getISBN() == trans.getISBN(); });
                if (bookIt != books.end()) {
                    cout << "ISBN: " << trans.getISBN() << ", Title: " << bookIt->getTitle() << ", Borrower ID: " << trans.getUserId() << ", Quantity: " << trans.getQuantity() << endl;
                }
            }
        }
    }

    void listUnavailableBooks() {
        cout << "Unavailable Books (no copies available):" << endl;
        bool found = false;
        for (const auto& book : books) {
            if (book.getAvailableCopies() == 0) {
                cout << "ISBN: " << book.getISBN() << ", Title: " << 

book.getTitle() << ", Author: " << book.getAuthor() << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "All books are available." << endl;
        }
    }
};

string getPassword() {
    string password;
    char ch;
    cout << "Enter Password: ";
    while ((ch = _getch()) != '\r') { // '\r' is Enter key
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back();

                cout << "\b \b"; // Erase the asterisk
            }
        } else {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

void displayBooksWithNumbers(const vector<Book>& books) {
    if (books.empty()) {
        cout << "No books available." << endl;
        return;
    }
    cout << "Available Books:" << endl;
    for (size_t i = 0; i < books.size(); ++i) {
        cout << (i + 1) << ". ISBN: " << 

books[i].getISBN() << ", Title: " << books[i].getTitle()
             << ", Author: " << books[i].getAuthor() << ", Available: " << books[i].getAvailableCopies()
             << "/" << books[i].getTotalCopies() << endl;
    }
}

int main() {
    unique_ptr<NotificationSender> notifier = make_unique<EmailNotification>();
    Librarian librarian(move(notifier));
    int mainChoice;

    cout << "****************************************" << endl;
    cout << "*                                      *" << endl;
    cout << "* welcome to library management system *" << endl;
    cout << "*                                      *" << endl;
    cout << "*                                      *" << endl;
    cout << "****************************************" << endl;

    do {
        //system("cls");
        cout << "\nMain Menu:" << endl;
        cout << "1. Register Librarian" << endl;
        cout << "2. Librarian" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> mainChoice;
        cin.ignore();

        if (mainChoice == 1) {
            // Register Librarian
            string username, password;
            cout << "Enter Username: ";

            getline(cin, username);
            password = getPassword();
            librarian.setCredentials(username, password);
            cout << "Librarian registered successfully!" << endl;
        } else if (mainChoice == 2) {
            // Librarian Login
            string username, password;
            cout << "Enter Username: ";
            getline(cin, username);
            password = getPassword();
            if (username == librarian.getLibrarianUsername() && password == librarian.getLibrarianPassword()) {
                cout << "Login successful!" << endl;
                int libChoice;
                do {
                    cout << "\nLibrarian Menu:" << 

endl;
                    cout << "1. List Books" << endl;
                    cout << "2. Add Book" << endl;
                    cout << "3. Delete Book" << endl;
                    cout << "4. Update Book" << endl;
                    cout << "5. Issue Book" << endl;
                    cout << "6. Display Borrowed Books" << endl;
                    cout << "7. Search Books" << endl;
                    cout << "8. Return Book" << endl;
                    cout << "9. Send Overdue Reminders" << endl;
                    cout << "0. Back to Main Menu" << endl;
                    cout << "Enter your choice: ";
                    cin >> libChoice;
                    cin.ignore();

                    switch (libChoice) {

                        case 1:
                            librarian.listBooks();
                            break;
                        case 2: {
                            string isbn, title, author;
                            int copies;
                            cout << "Enter ISBN: ";
                            getline(cin, isbn);
                            cout << "Enter Title: ";
                            getline(cin, title);
                            cout << "Enter Author: ";
                            getline(cin, author);
                            cout << "Enter Total Copies: ";
                            cin >> copies;
                            cin.ignore();
                            librarian.addBook(Book(isbn, title, author, copies));
                            break;
                        }

                        case 3: {
                            string isbn;
                            cout << "Enter ISBN to delete: ";
                            getline(cin, isbn);
                            librarian.deleteBook(isbn);
                            break;
                        }
                        case 4: {
                            string isbn, title, author;
                            int copies;
                            cout << "Enter ISBN to update: ";
                            getline(cin, isbn);
                            cout << "Enter New Title: ";
                            getline(cin, title);
                            cout << "Enter New Author: ";
                            getline(cin, author);
                            cout << "Enter New Total Copies: ";

                            cin >> copies;
                            cin.ignore();
                            librarian.updateBook(isbn, Book(isbn, title, author, copies));
                            break;
                        }
                        case 5: {
                            string userId, isbn;
                            int qty;
                            cout << "Enter User ID: ";
                            getline(cin, userId);
                            if (!librarian.userExists(userId)) {
                                cout << "User not found. Would you like to add a new patron? (y/n): ";
                                char addPatron;
                                cin >> addPatron;
                                cin.ignore();
                                if (addPatron == 'y' || addPatron == 'Y') {

                                    int patronType;
                                    cout << "Select Patron Type:" << endl;
                                    cout << "1. Student" << endl;
                                    cout << "2. Faculty" << endl;
                                    cout << "Enter your choice: ";
                                    cin >> patronType;
                                    cin.ignore();
                                    cout << " Would you like to see books inventory? (y/n): ";
                                    char selection;
                                    cin >> selection;
                                    cin.ignore();
                                    librarian.listBooks();
                                    string name, email;
                                    cout << "Enter Name: ";
                                    getline(cin, name);
                                    cout << "Enter Email: ";

                                    getline(cin, email);
                                    if (patronType == 1) {
                                        string studentId, department;
                                        cout << "Enter Student ID: ";
                                        getline(cin, studentId);
                                        cout << "Enter Department: ";
                                        getline(cin, department);
                                        userId = librarian.addStudent(name, email, studentId, department);
                                        cout << "Student added with ID: " << userId << endl;
                                    } else if (patronType == 2) {
                                        string facultyId, department, position;

                                        cout << "Enter Faculty ID: ";
                                        getline(cin, facultyId);
                                        cout << "Enter Department: ";
                                        getline(cin, department);
                                        cout << "Enter Position: ";
                                        getline(cin, position);
                                        userId = librarian.addFaculty(name, email, facultyId, department, position);
                                        cout << "Faculty added with ID: " << userId << endl;
                                    } else {
                                        cout << "Invalid patron type!" << endl;
                                        break;
                                    }

                                } else {
                                    cout << "Cannot issue book without a valid user." << endl;
                                    break;
                                }
                            }
                            cout << "Enter ISBN: ";
                            getline(cin, isbn);
                            cout << "Enter Quantity: ";
                            cin >> qty;
                            cin.ignore();
                            librarian.borrowBook(userId, isbn, qty);
                            break;
                        }
                        case 6:
                            librarian.listTransactions();
                            break;
                        case 7: {
                            int searchChoice;
                            cout << "Search Options:" 

<< endl;
                            cout << "1. Search by Title" << endl;
                            cout << "2. Search by ISBN" << endl;
                            cout << "Enter your choice: ";
                            cin >> searchChoice;
                            cin.ignore();
                            string query;
                            if (searchChoice == 1) {
                                cout << "Enter title to search: ";
                                getline(cin, query);
                                librarian.searchByTitle(query);
                            } else if (searchChoice == 2) {
                                cout << "Enter ISBN to search: ";
                                getline(cin, query);

                                librarian.searchByISBN(query);
                            } else {
                                cout << "Invalid choice!" << endl;
                            }
                            break;
                        }
                        case 8: {
                            string userId, isbn;
                            int qty;
                            cout << "Enter User ID: ";
                            getline(cin, userId);
                            cout << "Enter ISBN: ";
                            getline(cin, isbn);
                            cout << "Enter Quantity: ";
                            cin >> qty;
                            cin.ignore();
                            librarian.returnBook(userId, isbn, qty);
                            break;

                        }
                        case 9:
                            librarian.sendOverdueReminders();
                            break;
                        case 0:
                            cout << "Returning to main menu..." << endl;
                            break;
                        default:
                            cout << "Invalid choice! Please try again." << endl;
                    }
                } while (libChoice != 0);
            } else {
                cout << "Invalid credentials!" << endl;
            }
        } else if (mainChoice == 0) {
            cout << "Exiting..." << endl;
        } else {

            cout << "Invalid choice! Please try again." << endl;
        }
    } while (mainChoice != 0);

    return 0;
}
