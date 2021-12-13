#include <iostream>
#include <queue>
#include <map>
#include <fstream>
#include <string>
using namespace std;

struct MinHeapNode {
    int freq;
    char data;
    MinHeapNode* left, * right;
    MinHeapNode() {
        left = right = NULL;
    }
    MinHeapNode(char _data, int _freq) {
        left = right = NULL;
        this->freq = _freq;
        this->data = _data;
    }
};
struct compare {
public:
    bool operator()(MinHeapNode* l, MinHeapNode* r) {
        return (l->freq > r->freq);
    }
};

map<char, int> freq;
map<char, string> codes;
priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;
priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> create_temp;
priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> save_temp;
fstream inFile, outFile;
string inFileName, outFileName;

//Calculate freq of every character
void calfreq() {
    cout << "Calculating Freq.... " << endl;
    char id;
    inFile.open(inFileName, ios::in);
    inFile.get(id);
    while (!inFile.eof()) {
        freq[id]++;
        inFile.get(id);
    }
}

//Store encoded code for each character
void storecodes(MinHeapNode* root, string str) {
    if (root == NULL) return;
    if (root->data != '$')
        codes[root->data] = str;

    storecodes(root->left, str + "0");
    storecodes(root->right, str + "1");
}


//Creating Heap and Making huffman tree
void huffman() {
    cout << "Making Huffman Tree.... " << endl;
    MinHeapNode* left, * right, * top;
    for (map<char, int> ::iterator v = freq.begin(); v != freq.end(); v++) {
        minHeap.push(new MinHeapNode(v->first, v->second));
        create_temp.push(new MinHeapNode(v->first, v->second));
        save_temp.push(new MinHeapNode(v->first, v->second));
    }

    while (create_temp.size() != 1)
    {
        left = create_temp.top();
        create_temp.pop();
        right = create_temp.top();
        create_temp.pop();
        top = new MinHeapNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        create_temp.push(top);
    }
    cout << "Storing Codes..." << endl;
    storecodes(create_temp.top(), "");
}

void print_code() {
    cout << "Printing Codes.... " << endl;
    for (map<char, string>::iterator v = codes.begin(); v != codes.end(); v++) {
        cout << v->first << " " << v->second << endl;
    }
}

//string encode(string s) {
//    string enco_s = "";
//    for (auto v : s) {
//        enco_s += codes[v];
//    }
//    return enco_s;
//}
int binary2decimal(string str) {
    int res = 0;
    for (auto c : str) {
        res = res * 2 + c - '0';
    }
    return res;
}
string decimal2binary(int inNum) {
    string temp = "", res = "";
    while (inNum > 0) {
        temp += (inNum % 2 + '0');
        inNum /= 2;
    }
    res.append(8 - temp.length(), '0');
    for (int i = temp.length() - 1; i >= 0; i--) {
        res += temp[i];
    }
    return res;
}

void save_encoded() {
    inFile.open(inFileName, ios::in);
    outFile.open(outFileName, ios::out | ios::binary);
    string enc = "";
    string temp = "";
    char a;
    //save heap size
    enc += (char)minHeap.size();

    //save heap data
    while (!save_temp.empty()) {
        MinHeapNode* curr = save_temp.top();
        enc += curr->data;
        temp.assign(127 - codes[curr->data].length(), '0');
        temp += "1";
        temp += codes[curr->data];
        enc += (char)binary2decimal(temp.substr(0, 8));
        for (int i = 0; i < 15; i++) {
            temp = temp.substr(8);
            enc += (char)binary2decimal(temp.substr(0, 8));
        }
        save_temp.pop();
    }
    temp.clear();

    //save codes of every character in the given text file
    inFile.clear();                 // clear fail and eof bits
    inFile.seekg(0, std::ios::beg);
    inFile.get(a);
    while (!inFile.eof()) {
        temp += codes[a];
        while (temp.length() > 8) {
            enc += (char)binary2decimal(temp.substr(0, 8));
            temp = temp.substr(8);
        }
        inFile.get(a);
    }

    //padding 0
    int count = 8 - temp.length();
    if (temp.length() < 8) {
        temp.append(count, '0');
    }
    enc += (char)binary2decimal(temp);

    //save count of 0 appended
    enc += (char)count;

    // write the enc string to output file
    outFile.write(enc.c_str(), enc.size());
    inFile.close();
    outFile.close();
}

MinHeapNode* decode_root;
void buildTree(char data, string& path) {
    MinHeapNode* curr = decode_root;
    for (int i = 0; i < path.length(); i++) {
        if (path[i] == '0') {
            if (curr->left == NULL) {
                curr->left = new MinHeapNode();
            }
            curr = curr->left;
        }
        else if (path[i] == '1') {
            if (curr->right == NULL) {
                curr->right = new MinHeapNode();
            }
            curr = curr->right;
        }
        curr->data = data;
    }

}
void getTree() {
    inFile.open(inFileName, ios::in | ios::binary);
    unsigned char size;
    inFile.read(reinterpret_cast<char*>(&size), 1);
    decode_root = new MinHeapNode();
    for (int i = 0; i < size; i++) {
        char data;
        unsigned char code[16];
        inFile.get(data);
        inFile.read(reinterpret_cast<char*>(code), 16);
        string codebin = "";
        for (int i = 0; i < 16; i++) {
            codebin += decimal2binary(code[i]);
        }
        int count = 0;
        while (codebin[count] == '0') count++;
        codebin = codebin.substr(count + 1);
        buildTree(data, codebin);
    }
    inFile.close();

}



void saveDecodedFile() {
    inFile.open(inFileName, ios::in | ios::binary);
    outFile.open(outFileName, ios::out);

    /*unsigned char size;
    inFile.read(reinterpret_cast<char*>(&size), 1);
    cout << 1*size << "end"<<endl;*/
    char size;
    inFile.get(size);

    //Reading count at the end of the file which is number of bits appended to make final value 8-bit
    inFile.seekg(-1, ios::end);
    char count0;
    inFile.read(&count0, 1);
    //Ignoring the meta data (huffman tree) (1 + 17 * size) and reading remaining file
    inFile.seekg(1 + (17 * size), ios::beg);

    vector<unsigned char> text;
    unsigned char textseg;
    inFile.read(reinterpret_cast<char*>(&textseg), 1);
    while (!inFile.eof()) {
        text.push_back(textseg);
        inFile.read(reinterpret_cast<char*>(&textseg), 1);
    }

    MinHeapNode* curr = decode_root;
    string path;
    for (int i = 0; i < text.size() - 1; i++) {
        //Converting decimal number to its equivalent 8-bit binary code
        path = decimal2binary(text[i]);
        if (i == text.size() - 2) {
            path = path.substr(0, 8 - count0);
        }
        //Traversing huffman tree and appending resultant data to the file
        for (int j = 0; j < path.size(); j++) {
            if (path[j] == '0') {
                curr = curr->left;
            }
            else {
                curr = curr->right;
            }

            if (curr->left == NULL && curr->right == NULL) {
                outFile.put(curr->data);
                curr = decode_root;
            }
        }
    }
    inFile.close();
    outFile.close();
}
void compress() {
    calfreq();
    huffman();
    save_encoded();
    cout << "FILE COMPRESSED " << endl << endl;
}

void decompress() {
    getTree();
    saveDecodedFile();
    cout << "FILE DECOMPRESSED " << endl << endl;
}

int main()
{
    int n;
wrong:
    cout << "Enter 1 to COMPRESS " << endl << "Enter 2 to DECOMPRESS " << endl << "Enter 0 to EXIT " << endl;
    cin >> n;
    if (n == 1) {
        cout << "Enter file name to be compressed (with extension): ";
        cin >> inFileName;
        cout << endl;
        outFileName = "output.huf";
        compress();
    }
    else if (n == 2) {
        inFileName = "output.huf";
        outFileName = "outputfile.txt";
        decompress();
    }
    else if (n == 0) return 0;
    else cout << "INVALID INPUT, try again " << endl;
    goto wrong;

}
