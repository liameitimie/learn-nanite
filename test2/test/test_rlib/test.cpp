#include <fstream>
#include <string>
#include <iostream>
using namespace std;

struct AAA{
    int a;

    int bbb(){
        static int a=0;
        a++;
        return a;
    }
};

int main(){
    // ifstream in("format.txt",ios::in);
    // ifstream out("output_format.txt",ios::out);
    // ifstream in("aspect.txt",ios::in);
    // if(!in){
    //     cout<<"no"<<endl;
    //     return 0;
    // }
    // string s,s1,s2,s3;
    // int t;
    // while(in>>s>>s1>>s2){
    //     s3=s2.substr(14);
    //     s3.pop_back();
    //     if(s3=="ImageAspects::COLOR") s3="ImageAspect::Color";
    //     if(s3=="ImageAspects::DEPTH") s3="ImageAspect::Depth";
    //     if(s3=="ImageAspects::STENCIL") s3="ImageAspect::Stencil";
    //     cout<<"case Format::"<<s.substr(6)<<": return "<<s3<<";"<<endl;
    // }

    AAA t1,t2,t3;

    cout<<t1.bbb()<<' '<<t2.bbb()<<' '<<t3.bbb()<<endl;
    
    return 0;
}