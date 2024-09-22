#ifndef AVLFILEA_HPP
#define AVLFILEA_HPP

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "AvlRecordA.hpp"

using namespace std;

struct AvlHeaderA {
    int root = -1;
    int nextdel = -1;
};

template <typename TK>
class AVLFileA {
   public:
    long long int n_access = 0;

   private:
    string filename = "./data/avlb.dat";
    AvlHeaderA header;
    bool debug = false;

   public:
    AVLFileA() {
        ofstream file(filename, ios::out | ios::binary);
        file.seekp(0, ios::beg);
        file.write((char*)&header, sizeof(AvlHeaderA));
    }
    AVLFileA(string csvfile) {
        ofstream file(filename, ios::out | ios::binary);
        file.seekp(0, ios::beg);
        file.write((char*)&header, sizeof(AvlHeaderA));
        file.close();
        loadCSV(csvfile);
    }

    // Find by key
    AvlRecordA find(TK key) { return find(key, header.root); }

    // Insert
    void insert(AvlRecordA record, bool do_balance = true) {
        if (debug)
            cout << "Inserting record with key: " << record.key() << "\n";
        int new_root = insert(record, header.root, do_balance);
        if (new_root != header.root) {
            header.root = new_root;
            update_header();
        }
    }

    // Inorder traversal
    vector<AvlRecordA> inorder() {
        vector<AvlRecordA> records;
        inorder(header.root, records);
        return records;
    }

    // Remove by key
    bool remove(TK key) {
        auto ret = remove(key, header.root);
        if (ret.first) {
            if (header.root != ret.second) {
                if (debug)
                    cout << "Root changed to pos: " << ret.second << "\n";
                header.root = ret.second;
                update_header();
            }
        }
        return ret.first;
    }

    // Load CSV
    void loadCSV(const string& filename) {
        ifstream file(filename);
        string line;
        vector<AvlRecordA> records;

        if (!file.is_open()) {
            cerr << "No se pudo abrir el archivo.\n";
            return;
        }

        // skip header line
        getline(file, line);

        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            AvlRecordA record;

            getline(ss, token, ',');
            record.year = stoi(token);

            getline(ss, token, ',');
            strncpy(record.make, token.c_str(), sizeof(record.make));
            record.make[sizeof(record.make) - 1] = '\0';

            getline(ss, token, ',');
            strncpy(record.model, token.c_str(), sizeof(record.model));
            record.model[sizeof(record.model) - 1] = '\0';

            getline(ss, token, ',');
            strncpy(record.vin, token.c_str(), sizeof(record.vin));
            record.vin[sizeof(record.vin) - 1] = '\0';

            records.push_back(record);
        }

        file.close();

        // Sort the records based on the VIN field
        sort(records.begin(), records.end(),
             [](const AvlRecordA& a, const AvlRecordA& b) {
                 return strcmp(a.vin, b.vin) < 0;
             });

        // Bulk insert the sorted records into the AVL tree
        bulkInsert(records, 0, records.size() - 1);

        // Balance the AVL tree
        recursiveBalance(header.root);
    }

    void debugAVL() {
        ifstream file(filename, ios::binary);
        check_file_open(file);
        cout << "Starting debug\n";
        cout << "Root Pos in ram: " << header.root << "\n";
        cout << "Nextdel Pos in ram: " << header.nextdel << "\n";

        AvlHeaderA file_header;
        file.seekg(0, ios::beg);
        file.read((char*)&file_header, sizeof(AvlHeaderA));
        cout << "Root Pos in file: " << file_header.root << "\n";
        cout << "Nextdel Pos in file: " << file_header.nextdel << "\n";

        if (header.root != -1) {
            cout << "Root in file:\n";
            AvlRecordA record = readRecord(header.root);
            record.Print();
            cout << "left: " << record.left << "\n";
            cout << "right: " << record.right << "\n";
            cout << "nextdel: " << record.nextdel << "\n";
        }

        vector<AvlRecordA> records = inorder();
        cout << "\nAvlRecordA count: " << records.size() << "\n";
        // cout << "\nInorder traversal:\n";
        // for (auto& record : records) {
        //     record.showData();
        //     cout << "\n";
        // }
        cout << "Finished debug\n";
        file.close();
    }
    void printAVL() {
        printAVL(header.root, 0);
        cout << "\n\n";
    }

   private:
    // create index helper functions
    void bulkInsert(vector<AvlRecordA>& records, int start, int end) {
        if (start > end)
            return;

        // Insert middle element first to keep the tree balanced
        int mid = (start + end) / 2;
        insert(records[mid], false);

        // Recursively insert the left and right halves
        bulkInsert(records, start, mid - 1);
        bulkInsert(records, mid + 1, end);
    }
    void recursiveBalance(int node_pos) {
        if (node_pos == -1)
            return;

        AvlRecordA node = readRecord(node_pos);
        recursiveBalance(node.left);
        recursiveBalance(node.right);
    }

    // read and write in disk
    AvlRecordA readRecord(int pos) {
        n_access++;
        if (pos == -1) {
            cerr << "Error! Se intento leer un registro con posicion -1.\n";
            exit(1);
        } else if (pos < 0) {
            cerr << "Error! Se intento leer un registro con posicion "
                    "negativa.\n";
            exit(1);
        }
        AvlRecordA record;
        ifstream file(filename, ios::binary);
        /*check_file_open(file);*/
        file.seekg(into_pos(pos), ios::beg);
        if (file.eof()) {
            cerr << "Error! Se intento leer un registro fuera de rango.\n";
            exit(1);
        }
        file.read((char*)&record, sizeof(AvlRecordA));
        return record;
    }
    void writeRecord(AvlRecordA& record, int pos) {
        n_access++;
        if (debug)
            cout << "Writing in pos: " << pos << ", record: " << record.key()
                 << "\n";
        ofstream file(filename, ios::binary | ios::in);
        /*check_file_open(file);*/
        file.seekp(into_pos(pos), ios::beg);
        file.write((char*)&record, sizeof(AvlRecordA));
    }

    // recursive functions
    AvlRecordA find(TK key, int node_pos) {
        if (node_pos == -1)
            return AvlRecordA();

        AvlRecordA node = readRecord(node_pos);
        if (key == node.key())
            return node;
        else if (key < node.key())
            return find(key, node.left);
        else
            return find(key, node.right);
    }
    void inorder(int node_pos, vector<AvlRecordA>& records) {
        if (node_pos == -1)
            return;
        AvlRecordA node = readRecord(node_pos);
        inorder(node.left, records);
        records.push_back(node);
        inorder(node.right, records);
    }
    pair<int, AvlRecordA> findMin(int node_pos) {
        if (node_pos == -1)
            return make_pair(-1, AvlRecordA());

        AvlRecordA node = readRecord(node_pos);
        while (node.left != -1) {
            node_pos = node.left;
            node = readRecord(node_pos);
        }
        return make_pair(node_pos, node);
    }

    int height(int node_pos) {
        int h = 0;
        if (node_pos != -1) {
            // if (debug)
            // cout << "Checking height of node with pos: " << node_pos
            // << "\n";
            AvlRecordA node = readRecord(node_pos);
            h = 1 + max(height(node.left), height(node.right));
        }
        return h;
    }

    // AVL functions
    int diff(int node_pos) {
        if (node_pos == -1)
            return 0;
        // if (debug)
        // cout << "Checking diff of node with pos: " << node_pos << "\n";
        AvlRecordA node = readRecord(node_pos);
        return height(node.left) - height(node.right);
    }
    int rr_rotation(int parent_pos) {
        // move parent to left of right child
        AvlRecordA parent = readRecord(parent_pos);
        int right_child_pos = parent.right;
        AvlRecordA right_child = readRecord(right_child_pos);
        parent.right = right_child.left;
        right_child.left = parent_pos;
        // update pointers
        writeRecord(parent, parent_pos);
        writeRecord(right_child, right_child_pos);
        return right_child_pos;
    }
    int ll_rotation(int parent_pos) {
        // move parent to right of left child
        AvlRecordA parent = readRecord(parent_pos);
        int left_child_pos = parent.left;
        AvlRecordA left_child = readRecord(left_child_pos);
        parent.left = left_child.right;
        left_child.right = parent_pos;
        // update pointers
        writeRecord(parent, parent_pos);
        writeRecord(left_child, left_child_pos);
        return left_child_pos;
    }
    int lr_rotation(int parent_pos) {
        AvlRecordA parent = readRecord(parent_pos);
        int left_child_pos = parent.left;
        parent.left = rr_rotation(left_child_pos);
        writeRecord(parent, parent_pos);

        return ll_rotation(parent_pos);
    }
    int rl_rotation(int parent_pos) {
        AvlRecordA parent = readRecord(parent_pos);
        int right_child_pos = parent.right;
        parent.right = ll_rotation(right_child_pos);
        writeRecord(parent, parent_pos);

        return rr_rotation(parent_pos);
    }
    int balance(int node_pos) {
        int bal_factor = diff(node_pos);
        if (bal_factor > 1) {
            if (debug)
                cout << "Left inbalance in node with pos: " << node_pos << "\n";

            if (diff(readRecord(node_pos).left) > 0) {
                if (debug)
                    cout << "LL rotation\n";
                node_pos = ll_rotation(node_pos);
            } else {
                if (debug)
                    cout << "LR rotation\n";
                node_pos = lr_rotation(node_pos);
            }
        } else if (bal_factor < -1) {
            if (debug) {
                cout << "Right inbalance in node with pos: " << node_pos
                     << "\n";
            }
            if (diff(readRecord(node_pos).right) > 0) {
                if (debug)
                    cout << "RL rotation\n";
                node_pos = rl_rotation(node_pos);
            } else {
                if (debug)
                    cout << "RR rotation\n";
                node_pos = rr_rotation(node_pos);
            }
        }
        return node_pos;
    }

    // recursive insert
    int insert(AvlRecordA& record, int node_pos, bool do_balance = true) {
        if (node_pos == -1) {
            AvlRecordA new_record = record;
            new_record.left = new_record.right = -1;
            ofstream file(filename, ios::binary | ios::in);
            /*check_file_open(file);*/
            // use nextdel if available
            int new_pos;
            if (header.nextdel == -1) {
                file.seekp(0, ios::end);
                new_pos = into_node_ptr(file.tellp());
            } else {
                // LIFO freelist
                new_pos = header.nextdel;
                AvlRecordA free_record = readRecord(new_pos);
                header.nextdel = free_record.nextdel;
                update_header();
                file.seekp(into_pos(new_pos), ios::beg);
            }
            file.write((char*)&new_record, sizeof(AvlRecordA));
            if (debug) {
                cout << "Inserted record with key: " << record.key()
                     << " in pos: " << new_pos << "\n";
            }
            return new_pos;
        }
        AvlRecordA node = readRecord(node_pos);
        if (record.key() < node.key()) {
            int newleft = insert(record, node.left, do_balance);
            if (node.left != newleft) {
                node.left = newleft;
                writeRecord(node, node_pos);
            }
        } else if (record.key() > node.key()) {
            int newright = insert(record, node.right, do_balance);
            if (node.right != newright) {
                node.right = newright;
                writeRecord(node, node_pos);
            }
        } else {
            // cerr << "Warning! Trying to insert duplicate key: " << node.key()
            //      << "\n";
            return node_pos;
        }
        if (do_balance)
            return balance(node_pos);
        else
            return node_pos;
    }
    // recursive remove
    pair<bool, int> remove(TK key, int node_pos) {
        bool found = false;
        if (node_pos == -1)
            return make_pair(false, -1);

        AvlRecordA node = readRecord(node_pos);
        if (key < node.key()) {
            auto leftret = remove(key, node.left);
            found = leftret.first;
            if (found && leftret.second != node.left) {
                node.left = leftret.second;
                writeRecord(node, node_pos);
            }
        } else if (key > node.key()) {
            auto rightret = remove(key, node.right);
            found = rightret.first;
            if (found && rightret.second != node.right) {
                node.right = rightret.second;
                writeRecord(node, node_pos);
            }
        } else {
            if (debug)
                cout << "Removing record with key: " << key
                     << " and pos: " << node_pos << "\n";
            // 2 children: replace with successor data and delete successor
            if (node.left != -1 && node.right != -1) {
                if (debug)
                    cout << "Removing record with 2 children\n";
                auto successor = findMin(node.right);  // get pos also !!
                int successor_pos = successor.first;
                AvlRecordA new_record = successor.second;

                // successor will never have left child
                new_record.left = node.left;

                // delete successor and update node->right
                new_record.right = remove(new_record.key(), node.right).second;
                // update node data with successor data
                writeRecord(new_record, node_pos);
                return make_pair(true, balance(node_pos));
            }
            // 0 or 1 children: replace with non--1 child, if any
            else {
                if (debug)
                    cout << "Removing record with 0 or 1 children\n";
                // if 1 child, select non--1 one
                int temp = node.left == -1 ? node.right : node.left;
                // update LIFO freelist
                node.nextdel = header.nextdel;
                header.nextdel = node_pos;
                update_header();
                // update node
                writeRecord(node, node_pos);
                return make_pair(true, temp);
            }
        }
        if (found)
            return make_pair(true, balance(node_pos));
        else
            return make_pair(false, node_pos);
    }
    // recursive print
    void printAVL(int node_pos, int depth) {
        if (node_pos == -1)
            return;
        AvlRecordA node = readRecord(node_pos);
        printAVL(node.left, depth + 1);
        cout << "Node: " << node.key() << ", depth: " << depth << "\n";
        cout << "Left: ";
        if (node.left != -1)
            cout << readRecord(node.left).key() << "\n";
        else
            cout << "-1\n";
        cout << "Right: ";
        if (node.right != -1)
            cout << readRecord(node.right).key() << "\n";
        else
            cout << "Null\n";
        printAVL(node.right, depth + 1);
    }

    /*
     * file functions
     */
    int into_pos(int pos) { return (pos * sizeof(AvlRecordA)) + sizeof(AvlHeaderA); }
    int into_node_ptr(int pos) {
        return (pos - sizeof(AvlHeaderA)) / sizeof(AvlRecordA);
    }
    void update_header() {
        ofstream file(filename, ios::binary | ios::in);
        /*check_file_open(file);*/
        file.seekp(0, ios::beg);
        file.write((char*)&header, sizeof(AvlHeaderA));
        file.close();
    }

    template <typename file_t>
    void check_file_open(file_t& file) {
        if (!file.is_open()) {
            cerr << "Error! No se pudo abrir el archivo '" << filename
                 << "'.\n";
            exit(1);
        }
    }

    template <typename file_t>
    void check_file_valid(file_t& file) {
        file.seekg(0, ios::end);
        int filelen = file.tellg();
        if (filelen % sizeof(AvlRecordA) != sizeof(AvlHeaderA)) {
            cerr << "Error! El archivo '" << filename
                 << "'no es un AVLFile valido.\n";
            exit(1);
        }
    }

    template <typename file_t>
    bool is_file_empty(file_t& file) {
        return file.peek() == ifstream::traits_type::eof();
    }
};

#endif  // AVLFILEA_HPP
