
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#define N 19

string itos ( int i );

/*----------------------*/
/*     main function    */
/*----------------------*/
int main ( int argc , char ** argv ) {

  if ( argc != 2 ) {
    cerr << "error #1: argc != 2" << endl;
    return 1;
  }

  ifstream in ( argv[1] );
  if ( in.fail() ) {
    cerr << "error #2: problem with file " << argv[1] << endl;
    return 1;
  }

  // variable names:
  string var_names[N];
  var_names[ 0] = "elasmu";
  var_names[ 1] = "A0";
  var_names[ 2] = "gA0";
  var_names[ 3] = "dgA";
  var_names[ 4] = "alphad";
  var_names[ 5] = "alphac";
  var_names[ 6] = "dk";
  var_names[ 7] = "phid0";
  var_names[ 8] = "gphid0";
  var_names[ 9] = "dgphid";
  var_names[10] = "phic0";
  var_names[11] = "gphic0";
  var_names[12] = "dgphic";
  var_names[13] = "thetad0";
  var_names[14] = "gthetad0";
  var_names[15] = "dgthetad";
  var_names[16] = "thetac0";
  var_names[17] = "gthetac0";
  var_names[18] = "dgthetac";

  int    seed , tag , i;
  double x;
  in >> seed >> tag;

  // create a temporary directory:
  string path = "tmp_" + itos(seed) + string("_") + itos(tag);
  string cmd  = string("mkdir ") + path;
  system ( cmd.c_str() );

  // create the GAMS input file (argv[1] --> input.txt):
  string file_name = path + "\\input.txt";
  ofstream out ( file_name.c_str() );
  if ( out.fail() ) {
    in.close();
    cerr << "error #3: problem with file " << file_name << endl;
    return 1;
  }

  for ( i = 0 ; i < N ; ++i ) {
    in  >> x;
    out << var_names[i] << " /" << x << "/" << endl;
  }

  in.close();
  out.close();

  // GAMS execution:
  if ( !in.fail() && !out.fail() ) {
    cmd = string("chdir ") + path + " & gams ..\\bb.gms lo=2";
    system ( cmd.c_str() );
    cmd = string("more ") + path + "\\output.txt";
    system ( cmd.c_str() );
  }
  else
    cerr << "error #4" << endl;

  // delete the temporary directory:
  cmd = string("del "  ) + path + "\\*.* /Q";
  system ( cmd.c_str() );
  cmd = string("rmdir ") + path;
  system ( cmd.c_str() );

  return 0;
}

/*----------------------*/
/*          itos        */
/*----------------------*/
string itos ( int i ) {
  ostringstream oss;
  oss << i;
  return oss.str();
}
