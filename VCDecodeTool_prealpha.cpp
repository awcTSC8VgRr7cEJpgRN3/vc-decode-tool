// sumafile.cpp -- functions with an array argument
#include <iostream>
#include <fstream>          // file I/O support
#include <cstdlib>          // support for exit()
#include <string>

using std::string;

const int headSize = 16;
const int blockSize = 4;
const char blockKey[blockSize] = {char(0x5D), char(0x6E), char(0xAF), char(0x45)};
const char headCredential[] = {'C', 'O', 'D', 'E'};
const int headCredOffset = 0;
char textKey[blockSize];
const int textKeyOffset = 12;
const string fileNameSuffix = ".png";

bool head_is_certified(const char* header)
{
    for (int i=0; i<(sizeof headCredential/sizeof(char)); i++)
        if (header[headCredOffset+i]!=headCredential[i]) return false;

    for (int i=0; i<blockSize; i++)
        textKey[i] = header[i+textKeyOffset];
    return true;
}
//bitset或許是更好的選擇
void block_decrypt(const char* cBlock, char* pBlock)
{
    for (int i=0; i<blockSize; i++)
        pBlock[i] = cBlock[i]^blockKey[i];
    unsigned char uchFlag = 0x0;
    unsigned char uchTmp;
    for (int i=0; i<blockSize; i++)
    {
        uchTmp = (unsigned char)pBlock[i];
        pBlock[i] = char((unsigned char)pBlock[i]-(unsigned char)textKey[i]-uchFlag);
        if (uchTmp == (unsigned char)pBlock[i]);
        else uchFlag = (uchTmp > (unsigned char)pBlock[i]) ? 0x0 : 0x1;
    }
    return;
}

int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    using std::ifstream;
    using std::ofstream;
    using std::ios;

    string inFileName;
    string outFileName;
    ifstream inFile;        // object for handling file input
    ofstream outFile;

    cout << "Enter name of data file: ";
    getline(cin, inFileName);
    inFile.open(inFileName, ios::in | ios::binary);  // associate inFile with a file
    if (!inFile.is_open())  // failed to open file
    {
        cout << "Could not open the file " << inFileName << endl;
        cout << "Program terminating.\n";
        cin.get();    // keep window open
        exit(EXIT_FAILURE);
    }
    outFileName = inFileName + fileNameSuffix;
    outFile.open(outFileName, ios::out | ios::binary);
    if (!outFile.is_open())  // failed to open file
    {
        cout << "Could not write the file " << outFileName << endl;
        cout << "Program terminating.\n";
        cin.get();    // keep window open
        exit(EXIT_FAILURE);
    }

    char chHead[headSize];
    char cipherBlock[blockSize];
    char plainBlock[blockSize];
    int iBlockCount = 0;          // number of items read
    int iCharCount = 0;
    char chIn;
    int oBlockCount = 0;

    inFile.get(chIn);    // get first value
    for (int i=0; inFile.good() && (i<headSize); i++)
    {
        chHead[i] = chIn;
        inFile.get(chIn);    // get next value
    }
    if (!inFile.good() || !head_is_certified(chHead))
    {
        cout << "File head certified mismatch." << inFileName << endl;
        cout << "Program terminating.\n";
        cin.get();    // keep window open
        exit(EXIT_FAILURE);
    }

    //inFile.get(chIn);    // get first value
    while (inFile.good())   // while input good and not at EOF
    {
        iBlockCount += (++iCharCount)/blockSize;
        iCharCount %= blockSize;            // one more item read
        if (iCharCount)
            cipherBlock[iCharCount-1] = chIn;
        else
        {
            cipherBlock[blockSize-1] = chIn;
            block_decrypt(cipherBlock, plainBlock);
            for (int i=0; i<blockSize; i++)
                outFile << plainBlock[i];
            ++oBlockCount;       // calculate running total
            //for (int i=0; i<blockSize; i++)
            //    printf("%02X ", (unsigned char)cipherBlock[i]);
            //cout << "→ ";
            //for (int i=0; i<blockSize; i++)
            //    printf("%02X ", (unsigned char)plainBlock[i]);
            //cout << endl;
        }
        inFile.get(chIn);    // get next value
    }
    for (int i=0; i<iCharCount; i++)
    {
        plainBlock[i] = cipherBlock[i];
        outFile << plainBlock[i];
    }
    //for (int i=0; i<blockSize; i++)
    //    if (i<iCharCount) printf("%02X ", (unsigned char)cipherBlock[i]);
    //    else              printf("   ", (unsigned char)cipherBlock[i]);
    //cout << "→ ";
    //for (int i=0; i<blockSize; i++)
    //    if (i<iCharCount) printf("%02X ", (unsigned char)plainBlock[i]);
    //    else              printf("   ", (unsigned char)plainBlock[i]);
    //cout << endl;

    if (inFile.eof())
        cout << "End of file reached.\n";
    else if (inFile.fail())
        cout << "Input terminated by data mismatch.\n";
    else
        cout << "Input terminated for unknown reason.\n";
    if (iBlockCount == 0 && iCharCount == 0)
        cout << "No data processed.\n";
    else
    {
        cout << "Input processed: "  << iBlockCount << " blocks and " << iCharCount << " bytes." << endl;
        cout << "Output processed: " << oBlockCount << " blocks." << endl;
    }
    inFile.close();         // finished with the file
    outFile.close();         // finished with the file
    cin.get();
    return 0;
}
