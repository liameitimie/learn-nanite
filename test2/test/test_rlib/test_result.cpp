#include <result.h>
#include <vector>
#include <utility>

using namespace std;

Result<int,int> f1(){
    return Err(1);
}

// Result<int,int> f2(){
//     int t=Try(f1());
//     return Ok(t);
// }

int main(){
    Result<int,int> r1=Ok(3);
    auto val=r1.unwrap();
    cout<<val<<endl;

    vector<pair<int,int>> v={{1,20},{2,4}};

    for(auto&[x,y]:v){
        cout<<x<<' '<<y<<endl;
    }

    for(auto&[x,y]:v){
        x=1,y=1;
    }

    for(auto&[x,y]:v){
        cout<<x<<' '<<y<<endl;
    }
    return 0;
}