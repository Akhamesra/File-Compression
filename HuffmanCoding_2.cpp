#include <iostream>
#include <queue>
#include <map>
#include <fstream>
#include <string>
using namespace std;

map<char, int> freq;
map<char, string> codes;

struct MinHeapNode {
    int freq;
    char data;
    MinHeapNode* left, * right;

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


void calfreq(string str) {
    cout << "Calculating Freq.... " << endl;
    for (int i = 0; i < str.size(); i++) {
        freq[str[i]]++;
    }
}
void storecodes(MinHeapNode* root, string str) {
    if (root == NULL) return;
    if (root->data != '$')
        codes[root->data] = str;

    storecodes(root->left, str + "0");
    storecodes(root->right, str + "1");
}

priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;

void huffman(int size) {
    cout << "Making Huffman Tree.... " << endl;
    MinHeapNode* left, * right, * top;
    for (map<char, int> ::iterator v = freq.begin(); v != freq.end(); v++)
        minHeap.push(new MinHeapNode(v->first, v->second));
    while (minHeap.size() != 1)
    {
        left = minHeap.top();
        minHeap.pop();
        right = minHeap.top();
        minHeap.pop();
        top = new MinHeapNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
    cout << "String Codes..." << endl;
    storecodes(minHeap.top(), "");
}

void print_code() {
    cout << "Printing Codes.... " << endl;
    for (map<char, string>::iterator v = codes.begin(); v != codes.end(); v++) {
        cout << v->first << " " << v->second << endl;
    }
}

string encode(string s) {
    string enco_s = "";
    for (auto v : s) {
        enco_s += codes[v];
    }
    return enco_s;
}
string decode(MinHeapNode* root, string encoded_string) {
    string ans = "";
    MinHeapNode* curr = root;
    for (int i = 0; i < encoded_string.size(); i++) {
        if (encoded_string[i] == '0') curr = curr->left;
        if (encoded_string[i] == '1') curr = curr->right;
        if (curr->left == NULL && curr->right == NULL) {
            ans += curr->data;
            curr = root;
        }

    }
    return ans + '\0';
}

int main()
{
    string s;
    cout << "Enter String to Encode : ";
    getline(cin, s);
    int size_s = sizeof(s) / sizeof(s[0]);
    calfreq(s);
    huffman(size_s);
    print_code();
    cout << endl << "Original String : " << s << endl << endl;
    string encoded_string = encode(s);
    cout << "Encoded String : " << encoded_string << endl << endl;
    string decoded_string = decode(minHeap.top(), encoded_string);
    cout << "Decoded String : " << decoded_string << endl << endl;


}
