#include "rawbmpwidget.h"
#include <QApplication>

#include <iostream>

using namespace std;

void print_hex(string str)
{
    cout<<str<<" = { ";
    for(int i=0; i<str.size(); i++)
    {
        cout<<std::hex<<str.at(i)<<", ";
    }
    cout<<" }"<<endl;
}

void print_hex(char *str)
{
    int N = strlen(str);
    int i = 0;
    printf("len= %d,  \"%s\"= { ", N, str);
    for(i=0; i<N; i++)
    {
        printf("%x, ", str[i]);
    }
    printf("}\n");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RawBmpWidget rbw;
    rbw.show();
//    string a1 = string(argv[1]);
//    system((string("copy ") + argv[1] + " C:\\temp\\disp.bmp").c_str());
    if(argc >= 2)
    {
        rbw.open(string(argv[1]));
//        rbw.open("C:\\temp\\disp.bmp");
    }

    return a.exec();
}
