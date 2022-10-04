#include <iostream> //for std::cout
#include <string> //for std::string
#include <fstream>
#include "MD5.h"
#include <climits>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
using namespace std::chrono;
using namespace std;
void fasterHashing(int thread_no, string& out, const string& guide);
void initBlock();
bool stopThreads(int thread_no);

string guide;
const char lf = 0x0A;
const int threadcount = 1;
vector<jthread> hashing_threads;

int main(int argc, char* argv[])
{
    bool found = false;
    string hash = "";
    cout << "Mining..." << endl;
    initBlock();
    auto start = high_resolution_clock::now();

    string out;

    for (int i = 0; i < threadcount; i++)
    {
        hashing_threads.push_back(jthread(fasterHashing, i, ref(out), ref(guide)));
    }
    for (auto& t : hashing_threads)
    {
        t.join();
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Block found in:" << duration.count() / 1000.0 << " seconds." << endl;
    //cout << "Average hashing power was: " << i / (duration.count() / 1000.0) << " H/S" << endl;
    cout << out << endl;
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

void fasterHashing(int thread_no, string& out, const string& guide)
{
    long long int relNonce = 0;
    for (size_t nonce = thread_no; nonce < LLONG_MAX; nonce += threadcount)
    {
        string text = guide + to_string(nonce) + lf;

        int len = text.length();
        char char_array[1000];

        strcpy_s(char_array, len + 1, text.c_str());
        string hash = md5(char_array, len);
        if (hash.substr(0, 7) == "bbbbbbb")
        {
            //stop all running threads and return value
            stopThreads(thread_no);
            out = hash + " " + to_string(nonce);
            return;
        }
        else if (relNonce % 100000 == 0)
        {
            cout << hash << " " + to_string(nonce) << " Thread: " << thread_no << endl;
        }
        relNonce++;
    }
    return;
}

bool stopThreads(int thread_no)
{
    try
    {
        for (size_t i = 0; i < hashing_threads.size() && i != thread_no; i++)
        {
            hashing_threads[i].request_stop();
        }
    }
    catch (const std::exception&)
    {
        cout << "Couldn't stop threads" << endl;
        return false;
    }
}