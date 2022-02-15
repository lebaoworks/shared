#include <Windows.h>

#include <iostream>
#include <sstream>

#include "zip.h"

using namespace std;

DWORD read_file(wstring file_path, stringstream& output)
{
    output.str("");
    FILE* f;
    _wfopen_s(&f, file_path.c_str(), L"rb");
    if (f == NULL)
        return 1;

    char buffer[4096];
    int nread;
    while (nread = fread(buffer, 1, 4086, f))
        output.write(buffer, nread);
    fclose(f);
    return 0;
}

class A
{
public:
    A() {cout << "A construct" << endl;}
    ~A() { cout << "A destruct" << endl; }
};

class B
{
public:
    A a;
    B() { cout << "B construct" << endl; }
    ~B() { cout << "B destruct" << endl; }
};
#include <fstream>
int main()
{
    Zip zip;
    if (!zip.add(L"Bảo.txt", L"Bảo.txt", ZipCompressionMethod::NoCompression))
    {
        cout << errno;
        return 1;
    }
    stringstream out;
    zip.to_bytes(out);

    FILE* f;
    fopen_s(&f, "HxD.zip", "wb");
    if (f == NULL)
        return 1;
    fwrite(out.str().c_str(), 1, out.str().length(), f);
    fclose(f);
    
    return 0;
}
