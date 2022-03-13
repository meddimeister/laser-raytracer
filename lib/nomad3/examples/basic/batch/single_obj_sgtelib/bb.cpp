

#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

// Crescent 10
#define N  10
#define N2 100

int main ( int argc , char ** argv ) {

  if ( argc < 2 ) {
    cout << 1e+20 << " " << 1e+20 << " " << 1e+20 << endl;
    return 1;
  }

  double z , g1 = 0.0 , g2 = 0.0;
  int    i;

  if ( argc == N+1 ) {
    for ( i = 1 ; i <= N ; i++ ) {
      z = atof(argv[i]);
      g1 += (z-1)*(z-1);
      g2 += (z+1)*(z+1);
    }
  }

  else {

    ifstream in ( argv[1] );
    if ( in.fail() ) {
      cout << 1e+20 << " " << 1e+20 << " " << 1e+20 << endl;
      return 1;
    }


    for ( i = 0 ; i < N ; i++ ) {
      in >> z;
	if (z<-9)
	 cout<<"z<-9"<<endl;
	if (z>11) 
	  cout<<"z>11"<<endl;
      g1 += (z-1)*(z-1);
      g2 += (z+1)*(z+1);
    }

    if ( in.fail() ) {
      cout << 1e+20 << " " << 1e+20 << " " << 1e+20 << endl;
      return 1;
    }

    in.close();

  }

  cout << z << " " << g1-N2 << " " << N2-g2 << endl;

  return 0;
}

