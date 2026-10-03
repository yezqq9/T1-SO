#include <iostream>
using namespace std;
int main(int argc, char* argv[]){
cout<<"argumentos: " <<argc<<"\n";
for(int i=0; i<argc; i++){
cout<<"Argumento nro "<<i<<" : " <<argv[i]<<"\n";
}
return 0;
}
