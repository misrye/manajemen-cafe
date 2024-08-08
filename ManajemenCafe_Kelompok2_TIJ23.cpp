#include <iostream>
#include <string>
#include <cstdlib> // Untuk fungsi rand()
#include <ctime>   // Untuk inisialisasi seed random

using namespace std;

// Struktur untuk representasi node dalam graf
struct NodeMakanan {
    string nama;
    string type;
    double price; // Tambahkan atribut harga
    NodeMakanan* next;
    
    NodeMakanan(const string& n, const string& t, double p) : nama(n), type(t), price(p), next(nullptr) {}
};

// Struktur untuk representasi node dalam pohon
struct TreeNode {
    string type;
    NodeMakanan* foodList;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const string& t) : type(t), foodList(nullptr), left(nullptr), right(nullptr) {}
};

// Struktur untuk representasi node dalam queue pemesanan
struct OrderNode {
    string customerName;
    string menuName;
    int quantity;
    double pricePerItem;
    double totalPrice;
    OrderNode* next;

    OrderNode(const string& cn, const string& mn, int qty, double ppi)
        : customerName(cn), menuName(mn), quantity(qty), pricePerItem(ppi), totalPrice(qty * ppi), next(nullptr) {}
};

// Struktur untuk representasi node dalam stack riwayat pesanan
struct OrderHistoryNode {
    string customerName;
    string menuName;
    int quantity;
    double pricePerItem;
    double totalPrice;
    OrderHistoryNode* next;

    OrderHistoryNode(const string& cn, const string& mn, int qty, double ppi)
        : customerName(cn), menuName(mn), quantity(qty), pricePerItem(ppi), totalPrice(qty * ppi), next(nullptr) {}
};

// Class untuk merepresentasikan graf makanan dan minuman berdasarkan jenis
class FoodGraph {
private:
    NodeMakanan* head;
    TreeNode* root;
    OrderNode* front;
    OrderNode* rear;
    OrderHistoryNode* historyTop; // Penunjuk ke puncak stack riwayat pesanan

    // Fungsi bantu untuk menambahkan node ke dalam BST
    // Kompleksitas Waktu: O(log n) pada rata-rata, O(n) pada kasus terburuk
    TreeNode* addTreeNode(TreeNode* node, const string& type) {
        if (!node) return new TreeNode(type);
        if (type < node->type) node->left = addTreeNode(node->left, type);
        else if (type > node->type) node->right = addTreeNode(node->right, type);
        return node;
    }

    // Fungsi bantu untuk mencari node dalam BST
    // Kompleksitas Waktu: O(log n) pada rata-rata, O(n) pada kasus terburuk
    TreeNode* findTreeNode(TreeNode* node, const string& type) {
        if (!node || node->type == type) return node;
        if (type < node->type) return findTreeNode(node->left, type);
        return findTreeNode(node->right, type);
    }

    // Fungsi bantu untuk menambahkan makanan atau minuman ke dalam list pada node pohon
    // Kompleksitas Waktu: O(n), di mana n adalah jumlah elemen dalam list
    void addFoodToTree(TreeNode* node, const string& nama, double price) {
        NodeMakanan* newNode = new NodeMakanan(nama, node->type, price);
        if (!node->foodList) {
            node->foodList = newNode;
        } else {
            NodeMakanan* current = node->foodList;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    // Fungsi bantu untuk mengurutkan daftar makanan atau minuman berdasarkan nama
    // Kompleksitas Waktu: O(n^2), menggunakan bubble sort
    NodeMakanan* sortFoodList(NodeMakanan* head) {
        if (!head || !head->next) return head; // Jika list kosong atau hanya ada satu elemen, tidak perlu diurutkan

        bool swapped;
        NodeMakanan* ptr1;
        NodeMakanan* lptr = nullptr;

        do {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr) {
                if (ptr1->nama > ptr1->next->nama) {
                    // Tukar data
                    swap(ptr1->nama, ptr1->next->nama);
                    swap(ptr1->type, ptr1->next->type);
                    swap(ptr1->price, ptr1->next->price);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);

        return head;
    }

    // Fungsi bantu untuk menghapus makanan atau minuman dari list pada node pohon
    // Kompleksitas Waktu: O(n), di mana n adalah jumlah elemen dalam list
    NodeMakanan* removeFoodFromList(NodeMakanan* head, const string& nama, bool& found) {
        if (!head) return nullptr;

        if (head->nama == nama) {
            NodeMakanan* temp = head->next;
            delete head;
            found = true;
            return temp;
        }

        NodeMakanan* current = head;
        while (current->next && current->next->nama != nama) {
            current = current->next;
        }

        if (current->next) {
            NodeMakanan* temp = current->next;
            current->next = current->next->next;
            delete temp;
            found = true;
        } 

        return head;
    }

    // Fungsi bantu untuk mencari harga satuan dari nama menu
    // Kompleksitas Waktu: O(n), di mana n adalah jumlah elemen dalam list
    double getPrice(const string& menuName) {
        NodeMakanan* current = head;
        while (current) {
            if (current->nama == menuName) {
                return current->price;
            }
            current = current->next;
        }
        return -1; // Indikator bahwa menu tidak ditemukan
    }

    // Fungsi bantu untuk menampilkan daftar makanan atau minuman dalam node pohon
    // Kompleksitas Waktu: O(n), di mana n adalah jumlah elemen dalam list
    void displayFoodList(NodeMakanan* head) {
        NodeMakanan* current = head;
        while (current) {
            if (!isDeleted(current->nama)) {  // Memeriksa apakah menu masih ada sebelum menampilkannya
                cout << "- " << current->nama << " (Harga: Rp " << current->price << ")\n";
            }
            current = current->next;
    }
}

    // Fungsi untuk memeriksa apakah menu sudah dihapus
    bool isDeleted(const string& nama) {
    NodeMakanan* current = head;
    while (current) {
        if (current->nama == nama) {
            return false;
        }
        current = current->next;
    }
    return true;
}

    // Fungsi bantu untuk traversal inorder pada pohon
    // Kompleksitas Waktu: O(n), di mana n adalah jumlah node dalam pohon
    void inorderTraversal(TreeNode* node) {
        if (!node) return;

        inorderTraversal(node->left);
        cout << "Jenis: " << node->type << "\n";
        displayFoodList(sortFoodList(node->foodList));
        inorderTraversal(node->right);
    }

public:
    FoodGraph() : head(nullptr), root(nullptr), front(nullptr), rear(nullptr), historyTop(nullptr) {}

    // Method untuk menambahkan edge antara makanan atau minuman dengan jenis
    // Kompleksitas Waktu: O(log n) untuk BST insertion, O(n) untuk linked list insertion
    void addEdge(const string& nama, const string& type, double price) {
        NodeMakanan* newNode = new NodeMakanan(nama, type, price);
        if (!head) {
            head = newNode;
        } else {
            NodeMakanan* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }

        // Tambahkan node ke dalam pohon
        root = addTreeNode(root, type);

        // Tambahkan makanan atau minuman ke dalam list pada node pohon
        TreeNode* treeNode = findTreeNode(root, type);
        if (treeNode) {
            addFoodToTree(treeNode, nama, price); // Panggil fungsi dengan 3 argumen
        }
    }

    // Method untuk menghapus menu makanan atau minuman
    // Kompleksitas Waktu: O(log n) untuk BST search, O(n) untuk linked list deletion
    void removeMenu(const string& nama) {
        bool found = false;

        head = removeFoodFromList(head, nama, found);

        // Perbarui juga di pohon
        root = removeFoodFromTree(root, nama, found);

        if (found) {
            cout << "Menu " << nama << " berhasil dihapus.\n";
        } else {
            cout << "Menu " << nama << " tidak ditemukan.\n";
        }
    }

    // Method bantu untuk menghapus makanan atau minuman dari pohon
    TreeNode* removeFoodFromTree(TreeNode* node, const string& nama, bool& found) {
        if (!node) return nullptr;

        node->foodList = removeFoodFromList(node->foodList, nama, found);

        if (nama < node->type) {
            node->left = removeFoodFromTree(node->left, nama, found);
        } else if (nama > node->type) {
            node->right = removeFoodFromTree(node->right, nama, found);
        }

        return node;
    }

    // Method untuk menampilkan rekomendasi makanan atau minuman berdasarkan jenis
    // Kompleksitas Waktu: O(n) untuk linked list traversal
    void recommendByType(const string& type) {
        cout << "Makanan atau minuman yang direkomendasikan untuk Anda berdasarkan jenis " << type << " adalah: ";
        NodeMakanan* current = head;
        int count = 0;
        while (current) {
            if (current->type == type) {
                count++;
            }
            current = current->next;
        }
        if (count == 0) {
            cout << "Tidak ada makanan atau minuman dengan jenis tersebut.\n";
        } else {
            srand(time(0));
            int randomIndex = rand() % count;
            current = head;
            count = 0;
            while (current) {
                if (current->type == type) {
                    if (count == randomIndex) {
                        cout << current->nama << " (Rp " << current->price << ")";
                        break;
                    }
                    count++;
                }
                current = current->next;
            }
        }
        cout << endl;
    }

    // Method untuk menampilkan daftar semua makanan atau minuman
    // Kompleksitas Waktu: O(n), di mana n adalah jumlah node dalam pohon
    void displayAll() {
        cout << "Daftar semua makanan dan minuman berdasarkan jenis:\n";
        inorderTraversal(root); // Traversal inorder untuk menampilkan data
    }

    // Method untuk menambahkan pesanan ke dalam queue
    // Kompleksitas Waktu: O(1) untuk queue insertion
    void addOrder(const string& customerName, const string& menuName, int quantity, double pricePerItem) {
        OrderNode* newOrder = new OrderNode(customerName, menuName, quantity, pricePerItem);
        if (!rear) {
            front = rear = newOrder;
        } else {
            rear->next = newOrder;
            rear = newOrder;
        }
        cout << "Pesanan berhasil ditambahkan.\n";
    }

    // Method untuk menampilkan semua pesanan
    // Kompleksitas Waktu: O(n), di mana n adalah jumlah elemen dalam queue
    void displayOrders() {
        if (!front) {
            cout << "Tidak ada pesanan.\n";
            return;
        }

        OrderNode* current = front;
        cout << "Daftar semua pesanan:\n";
        while (current) {
            cout << "- Nama Pemesan: " << current->customerName 
                    << ", Menu: " << current->menuName 
                    << ", Jumlah: " << current->quantity 
                    << ", Harga Satuan: Rp " << current->pricePerItem 
                    << ", Total Harga: Rp " << current->totalPrice << "\n";
            current = current->next;
        }
    }

    // Method untuk memesan makanan atau minuman
    // Kompleksitas Waktu: O(n) untuk linked list search dan queue insertion
    void placeOrder() {
        string customerName, menuName;
        int quantity;

        cout << "Masukkan nama pemesan: ";
        getline(cin, customerName);

        cout << "Masukkan nama menu yang dipesan: ";
        getline(cin, menuName);

        double pricePerItem = getPrice(menuName);
        if (pricePerItem == -1) {
            cout << "Menu tidak ditemukan. Silakan coba lagi.\n";
        } else {
            cout << "Masukkan jumlah yang dipesan: ";
            cin >> quantity;
            cin.ignore(); // Untuk menghapus newline character dari buffer

            addOrder(customerName, menuName, quantity, pricePerItem);

            cout << "- Nama Pemesan: " << customerName << ", Menu: " << menuName
                    << ", Jumlah: " << quantity 
                    << ", Harga Satuan: Rp " << pricePerItem 
                    << ", Total Harga: Rp " << quantity * pricePerItem << "\n";
        }

    }

    // Method untuk menandai pesanan sebagai selesai
    void completeOrder() {
        if (!front) {
            cout << "Tidak ada pesanan yang sedang menunggu.\n";
            return;
        }

        string input;
        cout << "Apakah pesanan paling atas telah selesai? (yes/no): ";
        getline(cin, input);

        if (input == "yes") {
            OrderNode* completedOrder = front;
            front = front->next;
            if (!front) {
                rear = nullptr;
            }

            // Tambahkan pesanan ke dalam riwayat (stack)
            OrderHistoryNode* historyNode = new OrderHistoryNode(
                completedOrder->customerName,
                completedOrder->menuName,
                completedOrder->quantity,
                completedOrder->pricePerItem
            );
            historyNode->next = historyTop;
            historyTop = historyNode;

            delete completedOrder;
            cout << "Pesanan telah ditandai sebagai selesai dan dipindahkan ke riwayat pesanan.\n";
        } else {
            cout << "Pesanan tidak ditandai sebagai selesai.\n";
        }
    }

    // Method untuk menampilkan riwayat pesanan
    void displayOrderHistory() {
        if (!historyTop) {
            cout << "Riwayat pesanan kosong.\n";
            return;
        }

        OrderHistoryNode* current = historyTop;
        cout << "Riwayat semua pesanan yang telah selesai:\n";
        while (current) {
            cout << "- Nama Pemesan: " << current->customerName 
                    << ", Menu: " << current->menuName 
                    << ", Jumlah: " << current->quantity 
                    << ", Harga Satuan: Rp " << current->pricePerItem 
                    << ", Total Harga: Rp " << current->totalPrice << "\n";
            current = current->next;
        }
    }
};

// Hash table untuk menyimpan data karyawan dengan metode chaining
struct HashTable {
    static const int SIZE = 10;
    struct Entry {
        string nama;
        Entry* next;
    }* table[SIZE];

    HashTable() {
        for (int i = 0; i < SIZE; ++i) {
            table[i] = nullptr;
        }
    }

    int hashFunction(string nama) {
        unsigned long hash = 5381; // Nilai awal yang umum digunakan dalam DJB2
        for (char c : nama) {
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */ 
    }
    return hash % SIZE;
    }

    // Kompleksitas Waktu: O(n), di mana n adalah jumlah elemen dalam bucket yang diakses
    bool cariKaryawan(string nama) {
        int index = hashFunction(nama);
        Entry* current = table[index];
        while (current != nullptr) {
            if (current->nama == nama) {
                return true;
            }
            current = current->next;
        }
        return false;
    }
};

// Function untuk menambahkan karyawan ke hash table
void tambahKaryawan(HashTable& hashTable, string nama) {
    int index = hashTable.hashFunction(nama);
    HashTable::Entry* newEntry = new HashTable::Entry{nama, hashTable.table[index]};
    hashTable.table[index] = newEntry; // Chaining
    cout << "Karyawan " << nama << " berhasil ditambahkan.\n";
}

// Function untuk menampilkan seluruh karyawan dari hash table
    void tampilkanDaftarKaryawan(const HashTable &hashTable) {
        cout << "Daftar Karyawan:\n";
        for (int i = 0; i < HashTable::SIZE; ++i) {
            HashTable::Entry* current = hashTable.table[i];
            while (current != nullptr) {
                cout << current->nama << endl;
                current = current->next;
        }
    }
}


HashTable hashTable;



int main() {
    // Membuat objek graf makanan dan minuman berdasarkan jenis
    FoodGraph graph;

    // Meminta pilihan menu dari pengguna
    int choice;
    string nama, type;
    double price;
    string input;

    while (true){
        cout << "-------------------------------------------------------\n";
        cout << "|             | ~ WELCOME TO OUR CAFE ~ |             |\n";
        cout << "-------------------------------------------------------\n";
        cout << "\n===== O    U   R | M   A   I   N | M   E   N   U =====\n";
        cout << "1. Masuk sebagai Owner\n";
        cout << "2. Masuk sebagai Customer\n";
        cout << "3. Keluar\n";
        cout << "Pilih opsi: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            while (true) {
                cout << "\n===== O  W   N   E   R'  S =====\n";
                cout << "1. Tambah Makanan atau Minuman\n";
                cout << "2. Hapus Menu\n";
                cout << "3. Tampilkan Daftar Semua Menu\n";
                cout << "4. Tampilkan Daftar Pesanan\n";
                cout << "5. Tampilkan Riwayat Pesanan\n";
                cout << "6. Tambah Karyawan\n";
                cout << "7. Tampilkan Daftar Karyawan\n";
                cout << "8. Kembali ke Menu Utama\n";
                cout << "Pilih opsi: ";
                cin >> choice;
                cin.ignore();

                if (choice == 1){
                    cout << "Masukkan nama makanan atau minuman: ";
                    getline(cin, nama);
                    cout << "Masukkan jenis (makanan/minuman): ";
                    getline(cin, type);
                    cout << "Masukkan harga: ";
                    cin >> price;
                    cin.ignore(); // Untuk menghapus newline character dari buffer
                    graph.addEdge(nama, type, price); 
                } else if (choice == 2){
                    cout << "Masukkan nama makanan atau minuman yang ingin dihapus: ";
                    getline(cin, nama);
                    graph.removeMenu(nama);
                } else if (choice == 3){
                    graph.displayAll(); 
                } else if(choice == 4){
                    graph.displayOrders();
                    graph.completeOrder();
                } else if(choice == 5){
                    graph.displayOrderHistory();
                } else if (choice == 6){
                    cout << "Masukkan nama karyawan: ";
                    getline(cin, nama);
                    tambahKaryawan(hashTable, nama);
                } else if(choice == 7){
                    tampilkanDaftarKaryawan(hashTable);
                } else if (choice == 8) {
                    break;
                } else {
                    cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                }
            }
        } else if (choice == 2) {
            while (true) {
                cout << "\n===== C  U   S   T   O   M   E   R'  S =====\n";
                cout << "1. Tampilkan Semua Menu\n";
                cout << "2. Rekomendasi Menu berdasarkan Jenis\n";
                cout << "3. Buat Pesanan\n";
                cout << "4. Kembali ke Menu Utama\n";
                cout << "Pilih opsi: ";
                cin >> choice;
                cin.ignore();

                if (choice == 1){
                    graph.displayAll();
                } else if (choice == 2){
                    cout << "Masukkan jenis (makanan/minuman) untuk mendapatkan rekomendasi: ";
                    getline(cin, input);
                    graph.recommendByType(input);
                } else if (choice == 3){
                    graph.placeOrder();
                } else if (choice == 4) {
                    break;
                } else {
                    cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                }
            } 
            } else if (choice == 3) {
                cout << "\n---------------------------------------------\n";
                cout << "|        ~ SEE    YOU NEXT    TIME~         |  \n";
                cout << "\n---------------------------------------------\n";
                break;
            } else {
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    }
    return 0;
}
