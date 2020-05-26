#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>
#include <vector>
#include <cmath>
#include <chrono> 
using namespace std;

//files paths
string filePath = "enwik8";			//input text file
string fileEncodedPath = "Encoded.bin";		//encoded binary file
string fileDecodedPath = "Decoded";     //decoded text file


string nullString = "" ;   //holds empty node in tree
struct node {

	string word = nullString;
	unsigned int Freq = 0;
	node* left = NULL;
	node* right = NULL;
	
	node(string word, unsigned int Freq) :word(word), Freq(Freq), left(NULL), right(NULL) {}
	
	//default constructor with initializers list
	node() :word(nullString), Freq(0), left(NULL), right(NULL) {}
};

//handles operation override when comparing two nodes
struct OrderByValue {
	bool operator() (node const& a, node const& b) { return a.Freq > b.Freq; }
};


//Dictionary for Codes of each word and the inverse dictionary
map < string, string > Dictionary;
map< string, string > InvDictionary;


//this function aims to build codes dictionary 
void HuffmanRec(node n, string code = "") {
	if (n.word == nullString) {
		HuffmanRec(*n.left, code + "1");
		HuffmanRec(*n.right, code + "0");
	}
	else {
		Dictionary[n.word] = code;
		InvDictionary[code] = n.word;
	}
}

//Dictionary for calculating frequencies of each word
map<string, unsigned int> Frequencies;

//priority queue for tree
priority_queue<node, vector<node>, OrderByValue> nodesTree;

vector<string> words;		//holds all input words to be encoded
vector<bool> bits;			//holds concatenation of all codes of words

vector<bool> encodedBits;   //holds concatenation of encoded binary file

//function that converts ineger to binary as string
string getBinary(int x) {
	string binaryS = "";
	string binaryS2 = "00000000";
	for (int i = 0; i < 8; i++) {
		if (x % 2)
			binaryS += "1";
		else
			binaryS += "0";
		x /= 2;
	}
	for (int i = 0; i < 8; i++)
		binaryS2[i] = binaryS[7 - i];
	return binaryS2;
}

//function that gets characeter of 8bits binary 
char getCharOf(int arr[]) {

	int sumBits = 0;
	for (int i = 7; i >=0; i--) 
		sumBits += arr[7-i] * (int)(pow(2, i));

	return (char)sumBits;
}

//convert each char in binary encoded file to bits
void addToBits(char x) {
	int integerChar = (int)x;
	if (integerChar < 0)
		integerChar += 256;     //avoid MSB which is considered a sign

	string binOfChar = getBinary(integerChar);
	for (int i = 0; i < 8; i++)
		if (binOfChar[i] == '0')
			encodedBits.push_back(0);
		else
			encodedBits.push_back(1);
}

//hanldes searcing in dictionary for a code
string searchInDictionary(string x) {
	map<string, string>::iterator itInv;
	itInv = InvDictionary.find(x);
	if (itInv == InvDictionary.end())
		return nullString;
	else
		return InvDictionary[x];
}

//function that calculates all statistics of input text file
void calculateFrequenciesAndWords() {
	ifstream IpFileObj;
	IpFileObj.open(filePath);
	char currentChar;
	string currenWord = "";
	while (IpFileObj.get(currentChar)) {
	
		if ((currentChar > 47 && currentChar < 58) ||
			(currentChar > 64 && currentChar < 91) || (currentChar > 96 && currentChar < 123)) {
			currenWord += currentChar;
		}
		else {
			if (currenWord != "") {
				Frequencies[currenWord]++;
				words.push_back(currenWord);
				currenWord.clear();
			}
			currenWord += currentChar;
			Frequencies[currenWord]++;
			words.push_back(currenWord);
			currenWord.clear();
		}
	}
	if (currenWord != "") {
		Frequencies[currenWord]++;
		words.push_back(currenWord);
	
	}
	
}


int main() {

	auto start = std::chrono::steady_clock::now();     //starting time of staticstics operation

	calculateFrequenciesAndWords();
	
	//calculates time difference between after and before operation
	auto end = std::chrono::steady_clock::now();
	double elapsed_time = double(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	cout << endl << "Statistics time:" << elapsed_time << " ms" << endl;



	/*
	 _______  _        _______  _______  ______  _________ _        _______
	(  ____ \( (    /|(  ____ \(  ___  )(  __  \ \__   __/( (    /|(  ____ \
	| (    \/|  \  ( || (    \/| (   ) || (  \  )   ) (   |  \  ( || (    \/
	| (__    |   \ | || |      | |   | || |   ) |   | |   |   \ | || |
	|  __)   | (\ \) || |      | |   | || |   | |   | |   | (\ \) || | ____
	| (      | | \   || |      | |   | || |   ) |   | |   | | \   || | \_  )
	| (____/\| )  \  || (____/\| (___) || (__/  )___) (___| )  \  || (___) |
	(_______/|/    )_)(_______/(_______)(______/ \_______/|/    )_)(_______)


	*/
	
	start = std::chrono::steady_clock::now();   //starting time of encoding operation
	
	//push each word with it's frequency inside the tree
	node* n;    //template
	map<string, unsigned int>::iterator it;
	for (it = Frequencies.begin(); it != Frequencies.end(); it++) {
		n = new node(it->first, it->second);
		nodesTree.push(*n);
	}

	//sorting the tree
	node* n1, * n2;  //templates for left and right
	while (nodesTree.size() > 1) {
		n1 = new node(nodesTree.top());
		nodesTree.pop();
		n2 = new node(nodesTree.top());
		nodesTree.pop();
		n = new node();
		n->Freq = n1->Freq + n2->Freq;
		n->left = n1;
		n->right = n2;
		nodesTree.push(*n);
	}

	//huffman recursion
	node head = nodesTree.top();  
	nodesTree.pop();
	if (Frequencies.size() > 1)
		HuffmanRec(head);
	else 
		HuffmanRec(head, "0");

	//concatenates all codes of each word 
	for (unsigned long int i = 0; i < words.size(); i++) {
		string currentCode = Dictionary[words[i]];
		for (int j = 0; j < currentCode.length(); j++) {
			if (currentCode[j] == '0')
				bits.push_back(0);
			else
				bits.push_back(1);
		}
	}
	
	//add stop flag at the end of bits string(encoded file)
	
	//if number of bits is not factor of 8, so apply zeros-padding
	if (bits.size() % 8) {
		int differece = 8 - (bits.size() % 8);
		for (int i = 0; i < differece; i++)
			bits.push_back(0);	
		string stopFlag = getBinary((8 - differece) + 48);
		for (int i = 0; i < 8; i++)
			if (stopFlag[i] == '0')
				bits.push_back(0);
			else
				bits.push_back(1);
	}
	else {
		string stopFlag = getBinary(8 + 48);
		for (int i = 0; i < 8; i++)
			if (stopFlag[i] == '0')
				bits.push_back(0);
			else
				bits.push_back(1);
	}
	

	ofstream binEncodedFile("Encoded.bin", ios::out | ios::binary);   //to out encoded chars as binary
	binEncodedFile.clear();    //clear file firstly to avoid garbage

	unsigned long int encodedLength = bits.size() / 8;  //chars of encoded file (all bits / 8)
	unsigned long int currentBit = 0;					//counter of current bit
	
	//convert each 8bits into char and write it to binary file
	for (unsigned long int i = 0; i < encodedLength; i++) {
		int currentOutput[8] = {};
		for (int j = 0; j < 8; j++) {
			currentOutput[j] = bits[currentBit];
			currentBit++;
		}
		binEncodedFile << getCharOf(currentOutput);
	}
	binEncodedFile.close();
	
	//calculating time difference to show encoding time 
	end = std::chrono::steady_clock::now();
	elapsed_time = double(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	cout << endl << "Encoding time:" << elapsed_time << " ms" << endl;


///////////////////////////////////////////////////////////////////////////////////////////////
/*
		 ______   _______  _______  _______  ______  _________ _        _______ 
		(  __  \ (  ____ \(  ____ \(  ___  )(  __  \ \__   __/( (    /|(  ____ \
		| (  \  )| (    \/| (    \/| (   ) || (  \  )   ) (   |  \  ( || (    \/
		| |   ) || (__    | |      | |   | || |   ) |   | |   |   \ | || |      
		| |   | ||  __)   | |      | |   | || |   | |   | |   | (\ \) || | ____ 
		| |   ) || (      | |      | |   | || |   ) |   | |   | | \   || | \_  )
		| (__/  )| (____/\| (____/\| (___) || (__/  )___) (___| )  \  || (___) |
		(______/ (_______/(_______/(_______)(______/ \_______/|/    )_)(_______)
		
*/

	
	start = std::chrono::steady_clock::now();

	ifstream EncodedFile(fileEncodedPath, ios::in | ios::binary);    //input encoded binary file
	ofstream Decoded_File(fileDecodedPath, ios::out | ios::trunc);   //create a new file to store decoded characters
	Decoded_File.clear();


	//convert all chars in binary file into equivilant bits
	char currentChar;
	while (EncodedFile.get(currentChar))
		addToBits(currentChar);

	//real length of bits = fileLength - (8 bits of flag number at end) - (padding of zeros)
	unsigned long int endOfEncoded = encodedBits.size() - 8 - (8 - ((int)currentChar - 48));

	string codeStr = "";   //holds concatenation of bits to check codes in inverse dictionary
	for (unsigned long int i = 0; i < endOfEncoded; i++) {
		codeStr += (encodedBits[i]? "1":"0");
		string decodedStr = searchInDictionary(codeStr);   //search on this code in dictionary 
		//if this code is not found or the node is just parent(not leaf), continue concatenation
		if (decodedStr != nullString) {
			//if the right code is in dictionary, so write it's equivilant decoded word
			Decoded_File << decodedStr;
			codeStr = "";
		}
	}

	//calculating time difference to show decoding time
	end = std::chrono::steady_clock::now();
	elapsed_time = double(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	cout << endl << "Decoding time:" << elapsed_time << " ms" << endl;

	EncodedFile.close();
	Decoded_File.close();

	return 0;
}