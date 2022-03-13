/*-----------------------------------------------------*/
/*  how to use the NOMAD library with a user function  */
/*-----------------------------------------------------*/
#include "nomad.hpp"
using namespace std;
// using namespace NOMAD; avoids putting NOMAD:: everywhere

/*----------------------------------------*/
/*               The problem              */
/*----------------------------------------*/
class My_Evaluator : public NOMAD::Evaluator {
public:
    My_Evaluator  ( const NOMAD::Parameters & p ) :
    NOMAD::Evaluator ( p ) {}
    
    ~My_Evaluator ( void ) {}
    
    // For Crescent
    bool eval_x ( NOMAD::Eval_Point   & x          ,
                 const NOMAD::Double & h_max      ,
                 bool                & count_eval   ) const
    {
    
        NOMAD::Double g1 = 0.0 , g2 = 0.0;

        int N = x.size();
        
        for ( int i = 0 ; i < N ; i++ )
        {
            g1 += (x[i]-1)*(x[i]-1);
            g2 += (x[i]+1)*(x[i]+1);
        }
        x.set_bb_output  ( 0 , x[N-1]  ); // objective value
        x.set_bb_output  ( 1 , g1-100 ); // constraint 1
        x.set_bb_output  ( 2 , 100-g2 ); // constraint 2
        
        count_eval = true; // count a black-box evaluation
        
        return true;       // the evaluation succeeded
    }
    
    
};

/*------------------------------------------*/
/*            NOMAD main function           */
/*------------------------------------------*/
int main ( int argc , char ** argv )
{
    
    
    // display:
    NOMAD::Display out ( std::cout );
    out.precision ( NOMAD::DISPLAY_PRECISION_STD );
    
    try {
        
        // NOMAD initializations:
        NOMAD::begin ( argc , argv );
        
        // parameters creation:
        NOMAD::Parameters p ( out );
        
        p.read("sgtelib_param.txt");
        
        // parameters validation:
        p.check();
        
        // custom evaluator creation:
        My_Evaluator ev   ( p );
        
        // algorithm creation and execution:
        NOMAD::Mads mads ( p , &ev );
        mads.run();
    }
    catch ( exception & e ) 
    {
        
        cerr << "\nNOMAD has been interrupted (" << e.what() << ")\n\n";
    }
    
    NOMAD::Slave::stop_slaves ( out );
    NOMAD::end();
    
    return EXIT_SUCCESS;
}
