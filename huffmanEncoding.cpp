#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

unsigned int frequencies[256] = {0};
string codebook[256];

class Node{
    unsigned char data;
    unsigned int frequency;
    unsigned char min;
    Node * leftC;
    Node * rightC;
public:
    Node(){}
    Node(const Node &n){data = n.data; frequency = n.frequency; leftC = n.leftC; rightC = n.rightC;}
    Node(unsigned char d, unsigned int f){data = d; frequency = f; leftC = NULL; rightC= NULL;} 
    Node(Node *, Node *);
    void fillCodebook(string *, string &);
    bool operator> (const Node &);

};

// Recursive definition for filling the codebook
// Base condition: If it is a leaf node, set the value in the 
//      node to be the code
// Recursive definition:  
// (1) If there exist a node on the left, add 0 to the code
//   and recurse on the left node. 
// (2) If there exist a node on the right, add 1 to the code
//   and recurse on the right node. 
void Node:: fillCodebook(string * codebook, string &code) {
    if(!leftC && !rightC){
        codebook[data] = code;
        return;
    }
    if(leftC){
        code += '0';
        leftC->fillCodebook(codebook, code);
        // Removing the 0 from the code
        code.erase(code.end()-1);
    }
    if(rightC){
        code += '1';
        rightC->fillCodebook(codebook, code);
        // Removing the 1 from the code
        code.erase(code.end()-1);
    }
}

/*  Function that is used to merge nodes
    It is called on a new Node and so it sets the right and
    left of the new node to rc and lc.

    The frequency of the merged node is set to the sum of the 
    two individual nodes

    The min is set as the the character with the minimum ASCII 
    value that is present in the subtree of the node; shall be
    used to break ties. See:
    bool Node::operator> (const Node &rhs)
*/
Node:: Node(Node * rc, Node * lc){
    frequency = rc->frequency + lc->frequency;
    rightC = rc;
    leftC = lc;
    // Initialising min which refers to the character with the 
    // minimum ASCII value that is present in the subtree of the
    // node; shall be used to break ties
    min = (rc->min < lc->min) ? rc->min : lc->min;
}

// Definining the action of the '>' operator on two node elements
bool Node::operator> (const Node &rhs){
    // Initial comparison is on the basis of the frequency of the node
    if(frequency > rhs.frequency)
        return true;
    if(frequency < rhs.frequency)
        return false;
    // If the frequency is the same, compare on the basis of the 
    // character min which refers to the character with the 
    // minimum ASCII value that is present in the subtree of the
    // node
    if(frequency == rhs.frequency)
        if(min > rhs.min)
            return true;
    return false;
}


class Heap{
    Node **minHeap;
    int heapSize;
public:
    Heap(){heapSize = 0; minHeap = new Node*[257];} // max of 255 characters
    void push(Node *);
    int size(){return heapSize;}
    void pop();
    Node * top(){return minHeap[1];}
};

void Heap:: push(Node *newNode) {
    int currentHeapNode = ++heapSize;
    // While the currentHeapNode is not at the root position
    // and its parent is larger than it
    while (currentHeapNode != 1 && *minHeap[currentHeapNode / 2] > *newNode) {
        // At the position of the currentNode, put its parent
        minHeap[currentHeapNode] = minHeap[currentHeapNode / 2];
        // Update the position of the currentNode
        currentHeapNode = currentHeapNode / 2;
    }
    // Place the newNode at its correct position
    minHeap[currentHeapNode] = newNode;
}

void Heap:: pop(){
    // Literally the last node
    Node *lastNode = minHeap[heapSize];
    // Taking the first element to the end of the array 
    // and simultaneously reducing the heap size
    minHeap [heapSize--] = minHeap[1];
    // The heap node that needs to be replaced
    int currentHeapNode = 1;
    // The first child of the heap node (Left one)
    int child = 2;
    
    while (child <= heapSize) {
        // If right child exists and is lesser than left child
        if (child < heapSize && *minHeap[child] > *minHeap[child + 1])
            // Make child, the right child
            child++;

        if (*minHeap[child] > *lastNode)
            break;

        minHeap[currentHeapNode] = minHeap[child];
        currentHeapNode = child;
        child *= 2;
    } // while not at end of heap
    
    minHeap[currentHeapNode] = lastNode;
}

void compress();
Node * constructHeap();

void compress(){ 
    unsigned char nextChar;
    string input="";
    cout << "Enter text, enter $ to indicate the end of the string." << endl;    
    // first, calculate the frequencies of each character
    cin >> noskipws; // Keyword tells cin not to skip whitespaces while reading the characters.
    while( (cin >> nextChar) && (nextChar != '$') )
    {
        frequencies[nextChar]++;
        input+=nextChar;
    }
    cout << "Inputted: " << input << endl;
    
    Node * root = constructHeap();
    string code="";
    root->fillCodebook(codebook,code);
    
    cout << "Char\tASCII\tCode" << endl;
    for(int i=0; i<256; i++)
    {
        if(frequencies[i] != 0)
        {
            cout << (char)i << ":\t" << i << ":\t" << codebook[i] << endl;
        }
    }   
    cout << "\n\n";
    
    int org_length=input.length()*8;
    int counter=0;
    cout << "Encoded text: " << endl;
    for(int i=0; input[i]!='\0'; i++) 
    {
        cout << codebook[(int)input[i]];
        counter+=codebook[(int) input[i]].length();
    }
    cout << "\n\nNumber of bits used by unencoded text: " << org_length << endl;
    cout << "Number of bits used by encoded text: " << counter << endl;
    double eff=( 1 - ((double)counter/(double)org_length) )*100;
    cout << "Storage space requirement reduced by " << eff << "%" << endl;
}


// First, it creates a min heap using all frequencies.
// In the next part, it pops the two smallest elements and 
// merges them to form the 
Node * constructHeap(){
    Heap minHeap;
    Node *nextNode;
    for(int i = 0; i < 256; i++){
        if (frequencies[i]){
            nextNode = new Node(i, frequencies[i]);
            minHeap.push(nextNode);
        }
    }
    
    Node * node1;
    Node * node2;
    Node * merged;
    while(minHeap.size() > 1){
        node1 = minHeap.top();
        minHeap.pop();
        node2 = minHeap.top();
        minHeap.pop();
        merged = new Node(node1, node2);
        minHeap.push(merged);
    }
    
    return minHeap.top();
}

int main()
{
    cout << "Welcome to the Huffman Encoder!" << endl;
    cout << "This program accepts a string from the user and outputs: " << endl;
    cout << "1. A table of the characters, their ASCII codes and their Huffman codes." << endl;
    cout << "2. The encoded text." << endl;
    compress();
    return 0;
}