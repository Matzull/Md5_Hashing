#include <iostream> //for std::cout
#include <string> //for std::string
#include <fstream>
#include <atomic>
#include "nmd5.h"
#include <climits>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
using namespace std::chrono;
using namespace std;
void fasterHashing(int thread_no, string& out, const string& guide, long long int& nonce_o);
void initBlock();
bool stopThreads(int thread_no);

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

    for (int i = 0; i < threadcount; i++)
    {
        hashing_threads.push_back(jthread(fasterHashing, i, ref(out), ref(guide), ref(nonce)));
    }
    for (auto& t : hashing_threads)
    {
        t.join();
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Block found in:" << duration.count() / 1000.0 << " seconds." << endl;
    cout << "Average hashing power was: " << nonce / duration.count() << " KH/S" << endl;
    cout << "Out: " << out << endl;
    return 0;
}

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

bool stopThreads(int thread_no)
{
    cout << "Attempting to kill all running threads" << endl;
    exit_thread_flag = true;
    return true;
}