#include <vector>
#include<iostream>
using namespace std;

void test()
{
    vector<int> v1(5,10);
    vector<int> v2;
    v2.reserve(v1.size());

    copy(v1.begin(),v1.begin()+5,v2.begin(),v2.end());
    for(auto it:v2)
    {
        std::cout<<it<<endl;
    }

}

int main()
{
    test(); 
    return 0;  
}