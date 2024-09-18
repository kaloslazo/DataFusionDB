#include <cstring>
#include <vector>
#include "Record.hpp"

using namespace std;


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
    AVLFile(string filename);

    // Find by key
    Record find(TK key) ;

    // Insert
    void insert(Record record);

    // Inorder traversal
    vector<Record> inorder();

    // Remove by key
    bool remove(TK key);

    void debugAVL();
    void printAVL();

   private:

    // read and write in disk
    Record readRecord(pos_t pos);
    void writeRecord(Record& record, pos_t pos);

    // recursive functions
    Record find(TK key, pos_t node_pos);
    void inorder(pos_t node_pos, vector<Record>& records);
    pair<pos_t, Record> findMin(pos_t node_pos);

    int height(pos_t node_pos);

    // AVL functions
    int diff(pos_t node_pos);
    pos_t rr_rotation(pos_t parent_pos);
    pos_t ll_rotation(pos_t parent_pos);
    pos_t lr_rotation(pos_t parent_pos);
    pos_t rl_rotation(pos_t parent_pos);
    pos_t balance(pos_t node_pos);

    // recursive insert
    pos_t insert(Record& record, pos_t node_pos);
    // recursive remove
    pair<bool, pos_t> remove(TK key, pos_t node_pos);
    // recursive print
    void printAVL(pos_t node_pos, int depth);

    /*
    * file functions
    */
    pos_t into_pos(pos_t pos);
    pos_t into_node_ptr(pos_t pos);
    void update_header();

    template <typename file_t>
    void check_file_open(file_t& file);

    template <typename file_t>
    void check_file_valid(file_t& file);

    template <typename file_t>
    bool is_file_empty(file_t& file);
};

