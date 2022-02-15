#include <Windows.h>

#include <iostream>
#include <sstream>

#include "zip.h"

using namespace std;

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
