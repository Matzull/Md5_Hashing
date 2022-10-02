#include <iostream> //for std::cout
#include <string> //for std::string
#include <string_view>
#include <fstream>
#include "MD5.h"
#include <climits>
#include <chrono>
using namespace std::chrono;
using namespace std;

string process(long long int& nonce);
string faster(long long int& nonce);
void initBlock();

int main(int argc, char* argv[])
{
    bool found = false;
    string hash = "";
    cout << "Mining..." << endl;
    initBlock();
    auto start = high_resolution_clock::now();
    for (long long int i = 0; i < LLONG_MAX && !found; i++)
    {
        hash = faster(i);
        if (hash.substr(0, 7) == "bbbbbbb")//Aqui se define la complejidad
        {
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start); 
            cout <<"Block found in:" << duration.count()/1000.0<< " seconds." << endl;
            cout << "Average hashing power was: " << i / (duration.count() / 1000.0) << " H/S" << endl;

            found = true;
            cout << hash << endl;
        }
        else if(i % 100000 == 0)
        {
            cout << hash << endl;
        }
    }
    
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

string faster(long long int& nonce)
{
    string text = guide + to_string(nonce) + lf;

    int len = text.length();
    char char_array[1000];

    // copying the contents of the
    // string to char array
    strcpy_s(char_array, len + 1, text.c_str());
    string Temp = md5(char_array, len);
    return Temp + " " + to_string(nonce);
}