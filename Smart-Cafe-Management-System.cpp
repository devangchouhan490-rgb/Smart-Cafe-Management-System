#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

#define GST_RATE 0.10
#define MAX_TABLES 15

// CLEAR SCREEN 
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// DATE TIME 
string getDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buf[50];
    strftime(buf, sizeof(buf), "%A, %d %B %Y | %I:%M %p", ltm);
    return string(buf);
}

// UPI VALIDATION
bool validUPI(string upi) {
    int at = upi.find('@');
    return (at > 0 && at < upi.length() - 1);
}

// ORDER
class Order {
public:
    string name;
    float price;
    int qty;

    Order(string n, float p, int q) : name(n), price(p), qty(q) {}
    float total() { return price * qty; }
};

// MENU DATA
struct MenuCategory {
    string name;
    string items[6];
    float prices[6];
};

MenuCategory menuList[6] = {
    {"Burgers", {"Veg Burger","Cheese Burger","Paneer Burger","Aloo Tikki","Double Patty","Special Burger"}, {80,100,120,70,150,180}},
    {"Pizza", {"Margherita","Farmhouse","Paneer Pizza","Cheese Burst","Mexican","Special Pizza"}, {199,249,299,349,279,399}},
    {"Snacks", {"Fries","Momos","Sandwich","Garlic Bread","Nuggets","Spring Roll"}, {90,100,80,120,110,130}},
    {"Main Course", {"Veg Thali","Paneer Butter","Dal Tadka","Chole Bhature","Biryani","Special Thali"}, {199,249,180,160,220,299}},
    {"Beverages", {"Cold Coffee","Hot Coffee","Tea","Soft Drink","Milk Shake","Fresh Juice"}, {90,70,30,40,110,80}},
    {"Desserts", {"Ice Cream","Brownie","Gulab Jamun","Rasmalai","Pastry","Special Dessert"}, {60,90,50,70,80,120}}
};

// ADMIN
class Admin {
public:
    float minBillAmount = 0;
    int billDiscount = 0;

    vector<string> comboItems;
    float comboPrice = 0;

    bool login() {
        string id, pass;
        cout << "Admin ID: ";
        cin >> id;
        cout << "Password: ";
        cin >> pass;
        clearScreen();
        return id == "admin" && pass == "1234";
    }

    void panel() {
        while (true) {
            cout << "\n--- ADMIN PANEL ---\n";
            cout << "1. Set Amount Offer\n2. Create Combo\n3. Back\n";
            int ch; cin >> ch;
            clearScreen();

            if (ch == 1) {
                cout << "Min Bill Amount: ";
                cin >> minBillAmount;
                cout << "Discount %: ";
                cin >> billDiscount;
                clearScreen();
            }
            else if (ch == 2) createCombo();
            else break;
        }
    }

    void createCombo() {
        comboItems.clear();
        comboPrice = 0;
        float total = 0;

        while (true) {
            cout << "Select Category (0 Done)\n";
            for (int i = 0; i < 6; i++)
                cout << i+1 << ". " << menuList[i].name << endl;

            int c; cin >> c;
            clearScreen();
            if (c == 0) break;

            for (int i = 0; i < 6; i++)
                cout << i+1 << ". " << menuList[c-1].items[i]
                     << " - " << menuList[c-1].prices[i] << endl;

            int it; cin >> it;
            clearScreen();

            comboItems.push_back(menuList[c-1].items[it-1]);
            total += menuList[c-1].prices[it-1];
        }

        cout << "Total Combo Value: Rs " << total << endl;
        cout << "Final Combo Price: ";
        cin >> comboPrice;
        clearScreen();
    }
};

// USER
class User {
public:
    string name, mobile;
    int tableNo;
    vector<Order> orders;
    float total = 0;

    void takeDetails(int t) {
        tableNo = t;
        cin.ignore();
        cout << "Customer Name: ";
        getline(cin, name);
        do {
            cout << "Mobile (10 digit): ";
            cin >> mobile;
        } while (mobile.length() != 10);
        clearScreen();
    }

    void addItem(int cat) {
        for (int i = 0; i < 6; i++)
            cout << i+1 << ". " << menuList[cat].items[i]
                 << " - " << menuList[cat].prices[i] << endl;

        int ch; cin >> ch;
        int q; cout << "Qty: "; cin >> q;

        orders.emplace_back(menuList[cat].items[ch-1],
                            menuList[cat].prices[ch-1], q);
        total += menuList[cat].prices[ch-1] * q;
        clearScreen();
    }

    void orderCombo(Admin &admin) {
        if (admin.comboItems.empty()) {
            cout << " No combo available\n";
            return;
        }

        cout << "--- COMBO OFFER ---\n";
        for (auto &i : admin.comboItems) cout << "- " << i << endl;
        cout << "Price: Rs " << admin.comboPrice << endl;

        int q; cout << "Quantity: "; cin >> q;
        orders.emplace_back("Combo Offer", admin.comboPrice, q);
        total += admin.comboPrice * q;
        clearScreen();
    }

    void printBill(Admin &admin) {
        int pay; string upi;
        while (true) {
            cout << "Payment Mode\n1. Cash\n2. Online\n";
            cin >> pay;
            if (pay == 1) break;
            if (pay == 2) {
                cout << "Enter UPI (name@bank): ";
                cin >> upi;
                if (validUPI(upi)) break;
                cout << " Wrong UPI\n";
            }
        }

        clearScreen();
        float discount = (total >= admin.minBillAmount)
                         ? total * admin.billDiscount / 100 : 0;
        float gst = (total - discount) * GST_RATE;

        cout << "=========== BILL ===========\n";
        cout << getDateTime() << endl;
        cout << "Table No: " << tableNo << "\nCustomer: " << name << endl;

        for (auto &o : orders)
            cout << o.name << " x" << o.qty << " = Rs " << o.total() << endl;

        cout << "----------------------------\n";
        cout << "Total: Rs " << total << endl;
        cout << "Discount: Rs " << discount << endl;
        cout << "GST: Rs " << gst << endl;
        cout << "Payable: Rs " << (total - discount + gst) << endl;
        cout << "Visit Again Sir \n";
    }
};

// TABLE MANAGER
class TableManager {
    vector<User> users;

public:
    bool canAdd() { return users.size() < MAX_TABLES; }

    User& addUser() {
        User u;
        u.takeDetails(users.size() + 1);
        users.push_back(u);
        return users.back();
    }

    User* getUser(int tableNo) {
        if (tableNo < 1 || tableNo > users.size()) return nullptr;
        return &users[tableNo - 1];
    }

    void removeTable(int tableNo) {
        if (tableNo < 1 || tableNo > users.size()) return;
        users.erase(users.begin() + tableNo - 1);
    }
};

// MAIN
int main() {
    Admin admin;
    TableManager manager;

    while (true) {
        cout << "\n1. Admin Panel\n2. Add New User\n3. Print Bill\n4. Exit\n";
        int ch; cin >> ch;
        clearScreen();

        if (ch == 1) {
            if (admin.login()) admin.panel();
        }
        else if (ch == 2) {
            if (!manager.canAdd()) {
                cout << " All tables occupied\n";
                continue;
            }

            User &u = manager.addUser();

            while (true) {
                cout << "Menu:\n";
                for (int i = 0; i < 6; i++)
                    cout << i+1 << ". " << menuList[i].name << endl;
                cout << "7. Combo\n8. Cancel Order\n9. Back\n";

                int c; cin >> c;
                clearScreen();

                if (c >= 1 && c <= 6) u.addItem(c-1);
                else if (c == 7) u.orderCombo(admin);
                else if (c == 8) { manager.removeTable(u.tableNo); break; }
                else break;
            }
        }
        else if (ch == 3) {
            int t;
            cout << "Enter Table No: ";
            cin >> t;
            clearScreen();

            User* u = manager.getUser(t);
            if (!u) cout << " Invalid Table No\n";
            else {
                u->printBill(admin);
                manager.removeTable(t);
            }
        }
        else break;
    }
    return 0;
}
