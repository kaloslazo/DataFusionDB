// AVLFile Data Structure
// Date: 2024-08-27

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include "Record.hpp"

using namespace std;

bool debug = false;


struct Header {
    pos_t root = VOID;
    pos_t nextdel = VOID;
};

template <typename TK>
class AVLFile {
   private:
    string filename;
    Header header;

   public:
    AVLFile(string filename) : filename(filename) {
        fstream file(filename, ios::binary | ios::in | ios::out);
        check_file_open(file);
        if (is_file_empty(file)) {
            file.close();
            cout << "Inicializando archivo vacio...\n";
            header = Header();
            update_header();
        } else {
            cout << "Leyendo archivo...\n";
            check_file_valid(file);
            file.seekg(0, ios::beg);
            file.read((char*)&header, sizeof(Header));
            file.close();
        }
    }

    // Find by key
    Record find(TK key) { return find(key, header.root); }

    // Insert
    void insert(Record record) {
        if (debug)
            cout << "Inserting record with key: " << record.key() << "\n";
        pos_t new_root = insert(record, header.root);
        if (new_root != header.root) {
            header.root = new_root;
            update_header();
        }
    }

    // Inorder traversal
    vector<Record> inorder() {
        vector<Record> records;
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

    void debugAVL() {
        ifstream file(filename, ios::binary);
        check_file_open(file);
        cout << "Starting debug\n";
        cout << "Root Pos in ram: " << header.root << "\n";
        cout << "Nextdel Pos in ram: " << header.nextdel << "\n";

        Header file_header;
        file.seekg(0, ios::beg);
        file.read((char*)&file_header, sizeof(Header));
        cout << "Root Pos in file: " << file_header.root << "\n";
        cout << "Nextdel Pos in file: " << file_header.nextdel << "\n";

        if (header.root != VOID) {
            cout << "Root in file:\n";
            Record record = readRecord(header.root);
            record.showData();
            cout << "left: " << record.left << "\n";
            cout << "right: " << record.right << "\n";
            cout << "nextdel: " << record.nextdel << "\n";
        }

        vector<Record> records = inorder();
        cout << "\nRecord count: " << records.size() << "\n";
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

    // read and write in disk
    Record readRecord(pos_t pos) {
        if (pos == VOID) {
            cerr << "Error! Se intento leer un registro con posicion VOID.\n";
            exit(1);
        } else if (pos < 0) {
            cerr << "Error! Se intento leer un registro con posicion "
                    "negativa.\n";
            exit(1);
        }
        Record record;
        ifstream file(filename, ios::binary);
        check_file_open(file);
        file.seekg(into_pos(pos), ios::beg);
        if (file.eof()) {
            cerr << "Error! Se intento leer un registro fuera de rango.\n";
            exit(1);
        }
        file.read((char*)&record, sizeof(Record));
        return record;
    }
    void writeRecord(Record& record, pos_t pos) {
        if (debug)
            cout << "Writing in pos: " << pos << ", record: " << record.key()
                 << "\n";
        ofstream file(filename, ios::binary | ios::in);
        check_file_open(file);
        file.seekp(into_pos(pos), ios::beg);
        file.write((char*)&record, sizeof(Record));
    }

    // recursive functions
    Record find(TK key, pos_t node_pos) {
        if (node_pos == VOID)
            return Record();

        Record node = readRecord(node_pos);
        if (key == node.key())
            return node;
        else if (key < node.key())
            return find(key, node.left);
        else
            return find(key, node.right);
    }
    void inorder(pos_t node_pos, vector<Record>& records) {
        if (node_pos == VOID)
            return;
        Record node = readRecord(node_pos);
        inorder(node.left, records);
        records.push_back(node);
        inorder(node.right, records);
    }
    pair<pos_t, Record> findMin(pos_t node_pos) {
        if (node_pos == VOID)
            return make_pair(VOID, Record());

        Record node = readRecord(node_pos);
        while (node.left != VOID) {
            node_pos = node.left;
            node = readRecord(node_pos);
        }
        return make_pair(node_pos, node);
    }

    int height(pos_t node_pos) {
        int h = 0;
        if (node_pos != VOID) {
            // if (debug)
            // cout << "Checking height of node with pos: " << node_pos
            // << "\n";
            Record node = readRecord(node_pos);
            h = 1 + max(height(node.left), height(node.right));
        }
        return h;
    }

    // AVL functions
    int diff(pos_t node_pos) {
        if (node_pos == VOID)
            return 0;
        // if (debug)
        // cout << "Checking diff of node with pos: " << node_pos << "\n";
        Record node = readRecord(node_pos);
        return height(node.left) - height(node.right);
    }
    pos_t rr_rotation(pos_t parent_pos) {
        // move parent to left of right child
        Record parent = readRecord(parent_pos);
        pos_t right_child_pos = parent.right;
        Record right_child = readRecord(right_child_pos);
        parent.right = right_child.left;
        right_child.left = parent_pos;
        // update pointers
        writeRecord(parent, parent_pos);
        writeRecord(right_child, right_child_pos);
        return right_child_pos;
    }
    pos_t ll_rotation(pos_t parent_pos) {
        // move parent to right of left child
        Record parent = readRecord(parent_pos);
        pos_t left_child_pos = parent.left;
        Record left_child = readRecord(left_child_pos);
        parent.left = left_child.right;
        left_child.right = parent_pos;
        // update pointers
        writeRecord(parent, parent_pos);
        writeRecord(left_child, left_child_pos);
        return left_child_pos;
    }
    pos_t lr_rotation(pos_t parent_pos) {
        Record parent = readRecord(parent_pos);
        pos_t left_child_pos = parent.left;
        parent.left = rr_rotation(left_child_pos);
        writeRecord(parent, parent_pos);

        return ll_rotation(parent_pos);
    }
    pos_t rl_rotation(pos_t parent_pos) {
        Record parent = readRecord(parent_pos);
        pos_t right_child_pos = parent.right;
        parent.right = ll_rotation(right_child_pos);
        writeRecord(parent, parent_pos);

        return rr_rotation(parent_pos);
    }
    pos_t balance(pos_t node_pos) {
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
    pos_t insert(Record& record, pos_t node_pos) {
        if (node_pos == VOID) {
            Record new_record = record;
            new_record.left = new_record.right = VOID;
            ofstream file(filename, ios::binary | ios::in);
            check_file_open(file);
            // use nextdel if available
            pos_t new_pos;
            if (header.nextdel == VOID) {
                file.seekp(0, ios::end);
                new_pos = into_node_ptr(file.tellp());
            } else {
                // LIFO freelist
                new_pos = header.nextdel;
                Record free_record = readRecord(new_pos);
                header.nextdel = free_record.nextdel;
                update_header();
                file.seekp(into_pos(new_pos), ios::beg);
            }
            file.write((char*)&new_record, sizeof(Record));
            if (debug) {
                cout << "Inserted record with key: " << record.key()
                     << " in pos: " << new_pos << "\n";
            }
            return new_pos;
        }
        Record node = readRecord(node_pos);
        if (record.key() < node.key()) {
            pos_t newleft = insert(record, node.left);
            if (node.left != newleft) {
                node.left = newleft;
                writeRecord(node, node_pos);
            }
        } else if (record.key() > node.key()) {
            pos_t newright = insert(record, node.right);
            if (node.right != newright) {
                node.right = newright;
                writeRecord(node, node_pos);
            }
        } else {
            cerr << "Warning! Trying to insert duplicate key: " << node.key()
                 << "\n";
            return node_pos;
        }
        return balance(node_pos);
    }
    // recursive remove
    pair<bool, pos_t> remove(TK key, pos_t node_pos) {
        bool found = false;
        if (node_pos == VOID)
            return make_pair(false, VOID);

        Record node = readRecord(node_pos);
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
            if (node.left != VOID && node.right != VOID) {
                if (debug)
                    cout << "Removing record with 2 children\n";
                auto successor = findMin(node.right);  // get pos also !!
                pos_t successor_pos = successor.first;
                Record new_record = successor.second;

                // successor will never have left child
                new_record.left = node.left;

                // delete successor and update node->right
                new_record.right = remove(new_record.key(), node.right).second;
                // update node data with successor data
                writeRecord(new_record, node_pos);
                return make_pair(true, balance(node_pos));
            }
            // 0 or 1 children: replace with non-VOID child, if any
            else {
                if (debug)
                    cout << "Removing record with 0 or 1 children\n";
                // if 1 child, select non-VOID one
                pos_t temp = node.left == VOID ? node.right : node.left;
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
    void printAVL(pos_t node_pos, int depth) {
        if (node_pos == VOID)
            return;
        Record node = readRecord(node_pos);
        printAVL(node.left, depth + 1);
        cout << "Node: " << node.key() << ", depth: " << depth << "\n";
        cout << "Left: ";
        if (node.left != VOID)
            cout << readRecord(node.left).key() << "\n";
        else
            cout << "VOID\n";
        cout << "Right: ";
        if (node.right != VOID)
            cout << readRecord(node.right).key() << "\n";
        else
            cout << "VOID\n";
        printAVL(node.right, depth + 1);
    }

    /*
    * file functions
    */
    pos_t into_pos(pos_t pos) {
        return (pos * sizeof(Record)) + sizeof(Header);
    }
    pos_t into_node_ptr(pos_t pos) {
        return (pos - sizeof(Header)) / sizeof(Record);
    }
    void update_header() {
        ofstream file(filename, ios::binary | ios::in);
        check_file_open(file);
        file.seekp(0, ios::beg);
        file.write((char*)&header, sizeof(Header));
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
        if (filelen % sizeof(Record) != sizeof(Header)) {
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


