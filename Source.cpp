#include <iostream> //for std::cout
#include <string> //for std::string
#include <string_view>
#include <fstream>
#include "nmd5.h"
#include <climits>
#include <chrono>
using namespace std::chrono;
using namespace std;

string process(long long int& nonce);
void fasterHashing(int thread_no, string& out, const string& guide, long long int& nonce_o);
void initBlock();

const char lf = 0x0A;
const int threadcount = 16;
vector<jthread> hashing_threads;
std::atomic<bool> exit_thread_flag{ false };
string guide;
const int difficulty = 7;

int main(int argc, char* argv[])
{
    bool found = false;
    string hash = "";
    cout << "Mining..." << endl;
    initBlock();
    auto start = high_resolution_clock::now();
    long long int nonce;
    string out;
    
    fasterHashing(int thread_no, string & out, const string & guide, long long int& nonce_o);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Block found in:" << duration.count() / 1000.0 << " seconds." << endl;
    cout << "Average hashing power was: " << nonce / duration.count() << " KH/S" << endl;
    cout << "Out: " << out << endl;
    return 0;

    return 0;
}



void updateNonce(const long long int& nonce)
{
    ifstream infile("bloque2.txt");
    string texto = "";
    string texto2 = "";
    string text = "";
    getline(infile, texto);
    getline(infile, texto2);
    ofstream outfile("bloque1.txt", ios_base::binary);
    char lf = 0x0A;
    text += texto + lf + texto2 + lf + "Nonce: " + to_string(nonce) + lf;
    outfile << text;
    infile.close();
    outfile.close();
}

string process(long long int& nonce)
{
    updateNonce(nonce);
    //Start opening your file
    std::ifstream inBigArrayfile;
    inBigArrayfile.open("bloque1.txt", std::ios::binary | std::ios::in);

    //Find length of file
    inBigArrayfile.seekg(0, std::ios::end);
    long Length = inBigArrayfile.tellg();
    inBigArrayfile.seekg(0, std::ios::beg);

    //read in the data from your file
    char* InFileData = new char[Length];
    inBigArrayfile.read(InFileData, Length);

    //Calculate MD5 hash
    string Temp = md5(InFileData, Length);

    //Clean up
    delete[] InFileData;
    return Temp + " " + to_string(nonce);
}

string guide;
char lf = 0x0A;

void initBlock()
{
    ifstream infile("guia.txt");
    string texto = "";
    string texto2 = "";
    getline(infile, texto);
    getline(infile, texto2);
    guide = texto + lf + texto2 + lf + "Nonce: ";
    infile.close();
}

void fasterHashing(int thread_no, string& out, const string& guide, long long int& nonce_o)
{
    char char_array[350];
    int guide_length = guide.length();
    strcpy_s(char_array, guide.length() + 1, guide.c_str());
    MD5 md5;

    long long int relNonce = 0;
    for (size_t nonce = thread_no; nonce < LLONG_MAX && !exit_thread_flag; nonce += threadcount)
    {
        string nonce_str = to_string(nonce) + lf;
        int nonce_length = nonce_str.length();
        for (size_t i = guide_length; i < guide_length + nonce_length; i++)
        {
            char_array[i] = nonce_str[i - guide_length];
        }

        int len = guide_length + nonce_length;

        md5.add(char_array, len);
        string hash = md5.getHash();

        //string hash = md5(char_array, len);
        bool correct = true;
        for (size_t i = 0; i < difficulty && correct; i++)
        {
            if (hash[i] != 'b')
            {
                correct = false;
            }
        }

        if (correct)
        {
            //stop all running threads and return value
            stopThreads(thread_no);
            out = hash + " " + to_string(nonce);
            nonce_o = nonce;
            return;
        }
        else if (relNonce % 1000000 == 0 && !thread_no)
        {
            cout << hash << " " + to_string(nonce) << endl;
        }
        relNonce++;
    }
    return;
}