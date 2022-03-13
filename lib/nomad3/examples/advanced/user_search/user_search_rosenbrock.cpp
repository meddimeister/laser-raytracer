/*-------------------------------------------------------------------------------------*/
/*           example of a user search for the Rosenbrock test function           */
/*-------------------------------------------------------------------------------------*/
#include "nomad.hpp"
using namespace std;
using namespace NOMAD;

double f( const Eval_Point & x)
{
    return 100*pow(x[1].value()-x[0].value()*x[0].value(),2)
    +pow(1-x[0].value(),2);
}

double df0 ( const Eval_Point & x )
{
    return 2.0*x[0].value() - 400.0 * x[0].value() * ( x[1].value() - pow( x[0].value() ,2.0 ) ) -2.0;
}


double df1 ( const Eval_Point & x )
{
    return -200.0 * ( pow(x[0].value(),2.0) + x[1].value() );
}

/*------------------------------------------------*/
/*               The problem                      */
/*------------------------------------------------*/
/*       n=2, m=1                                 */
/*       Rosenbrock function                           */
/*------------------------------------------------*/
class My_Evaluator : public Evaluator {
    
public:
    
    // ctor:
    My_Evaluator  ( const Parameters & p ) :
    Evaluator ( p ) {}
    
    // dtor:
    ~My_Evaluator ( void ) {}
    
    // evaluation of a point:
    bool eval_x ( Eval_Point          & x          ,
                 const NOMAD::Double & h_max      ,
                 bool                & count_eval   ) const
    {
       
        x.set_bb_output ( 0 , f(x) );
       
        count_eval = true;
        
        return true;
    }
    
    
    
    
};

/*------------------------------------------------*/
/*                   user search                  */
/*------------------------------------------------*/
class My_Search : public Search {
    
public:
    
    // ctor:
    My_Search ( Parameters & p )
    : Search ( p , USER_SEARCH ) {}
    
    // dtor:
    ~My_Search ( void ) {}
    
    // the search:
    void search (  Mads              & mads           ,
                 int               & nb_search_pts  ,
                 bool              & stop           ,
                 stop_type         & stop_reason    ,
                 success_type      & success        ,
                 bool              & count_search   ,
                 const Eval_Point *& new_feas_inc   ,
                 const Eval_Point *& new_infeas_inc   );
    
    
};

/*-------------------*/
/*  the user search  */
/*-------------------*/
void My_Search::search ( Mads              & mads           ,
                        int               & nb_search_pts  ,
                        bool              & stop           ,
                        stop_type         & stop_reason    ,
                        success_type      & success        ,
                        bool              & count_search   ,
                        const Eval_Point *& new_feas_inc   ,
                        const Eval_Point *& new_infeas_inc   ) {
    
    nb_search_pts = 0;
    success       = UNSUCCESSFUL;
    count_search  = false;
    
    // current feasible incumbent:
    const Eval_Point * xk = mads.get_best_feasible();
    
    if ( ! xk )
        return;
    
    
    // get a signature:
    Signature * signature = xk->get_signature();
    if ( !signature )
        return;
    
    // Projection maybe needed
    const NOMAD::Display & out= _p.out();
    NOMAD::dd_type display_degree = out.get_search_dd();
    
    // count the search:
    count_search  = true;
    
    // construct the search point  (gradient descent tk = xk + gamma * gradf with gamma such that new point is on the poll frame)
    NOMAD::Point Delta = signature->get_mesh()->get_Delta();
    NOMAD::Point delta = signature->get_mesh()->get_delta();
    double DF0 = df0(*xk);
    double DF1 = df1(*xk);
    
    bool keepX0 = false;
    if ( std::fabs(DF0) < NOMAD::DEFAULT_EPSILON )
        keepX0 = true;
    
    bool keepX1 = false;
    if ( std::fabs(DF1) < NOMAD::DEFAULT_EPSILON )
        keepX1 = true;
    
    
    // Get gamma from gradf and the poll size
    double gamma0 = Delta[0].value()/std::fabs(DF0);
    double gamma1 = Delta[1].value()/std::fabs(DF1);
    Double gamma = ( (gamma0 < gamma1) ? gamma0 : gamma1 );
    
    int i = 0;
    int max_iter_mesh = static_cast<int>(Delta[0].value()/delta[0].value());
    const int max_iter_abs = 4;
    if ( display_degree == NOMAD::FULL_DISPLAY )
    {
        out << "Max number of iterations for gradient descent poll frame interior line search: " << (( max_iter_mesh < max_iter_abs ) ? max_iter_mesh:max_iter_abs) <<endl;
    }
    
    while ( ! success && i < max_iter_mesh && i < max_iter_abs )
    {
     
        Eval_Point * tk = new Eval_Point;
        tk->set ( 2 , 1 );
        tk->set_signature  ( signature );
        
        i++;
        
        
        if ( keepX0 )
            (*tk)[0] = (*xk)[0] ;
        else
            (*tk)[0] = (*xk)[0] - gamma * DF0 / double(i)  ;
        
        if ( keepX1 )
            (*tk)[1] = (*xk)[1] ;
        else
            (*tk)[1] = (*xk)[1] - gamma * DF1 / double(i) ;
        
        if ( display_degree == NOMAD::FULL_DISPLAY )
        {
            out << "candidate";
            out << " (before projection)";
            out << ": ( " << *tk << " )" << std::endl;
        }
        
        // Project to the mesh
        tk->project_to_mesh(*xk,signature->get_mesh()->get_delta(),signature->get_lb(),signature->get_ub() );
        
        if ( display_degree == NOMAD::FULL_DISPLAY )
        {
            out << "candidate";
            out << " (after projection)";
            out << ": ( " << *tk << " )" << std::endl;
        }
        
        
        
        // Evaluator_Control:
        Evaluator_Control & ev_control = mads.get_evaluator_control();
        
        // add the new point to the ordered list of search trial points:
        ev_control.add_eval_point ( tk                       ,
                                   _p.out().get_search_dd() ,
                                   false                    ,
                                   Double()                 ,
                                   Double()                  ,
                                   Double()                 ,
                                   Double()                 );
        
        nb_search_pts = 1;
        
        // evaluation:
        new_feas_inc = new_infeas_inc = NULL;
        ev_control.eval_list_of_points ( _type                   ,
                                        mads.get_true_barrier() ,
                                        mads.get_sgte_barrier() ,
                                        mads.get_pareto_front() ,
                                        stop                    ,
                                        stop_reason             ,
                                        new_feas_inc            ,
                                        new_infeas_inc          ,
                                        success                   );
        
        
    }
    
    if ( !success && ( i == max_iter_mesh || i == max_iter_abs ) && display_degree == NOMAD::FULL_DISPLAY )
    {
        out << "No success but max number of iterations reached for gradient descent poll frame line search" << endl;
    }
    
}

/*------------------------------------------*/
/*            NOMAD main function           */
/*------------------------------------------*/
int main ( int argc , char ** argv ) {
    
    // NOMAD initializations:
    begin ( argc , argv );
    
    // display:
    Display out ( std::cout );
    out.precision ( DISPLAY_PRECISION_STD );
    
    //  parameters creation:
    Parameters p ( out );
    
    p.set_DIMENSION (2);             // number of variables
    
    vector<bb_output_type> bbot (1); // definition of
    bbot[0] = OBJ;                   // output types
    p.set_BB_OUTPUT_TYPE ( bbot );
    
    NOMAD::Point  x0 ( 2 , 0.5 );
    x0[0] = -0.5;
    p.set_X0 ( x0 );  // starting point
    p.set_LOWER_BOUND( NOMAD::Point ( 2 , -5.12 ) );
    p.set_UPPER_BOUND( NOMAD::Point ( 2 , 5.12 ) );
    
    p.set_DISABLE_MODELS();
    
    p.set_SPECULATIVE_SEARCH ( false );
    
    p.set_INITIAL_MESH_SIZE        ( 1.0 );
    
    //p.set_DISPLAY_DEGREE ( NOMAD::FULL_DISPLAY );
    p.set_DISPLAY_DEGREE ( NOMAD::NORMAL_DISPLAY );
    
    p.set_DISPLAY_STATS ( "bbe sol obj" );
    
    p.set_MAX_BB_EVAL ( 1000 );
    
    // parameters validation:
    p.check();
    
    // custom evaluator creation:
    My_Evaluator ev ( p );
    
    // algorithm creation:
    Mads mads ( p , &ev );
    
    // user search:
    My_Search my_search  ( p );
    // mads.set_user_search ( &my_search );
    
    // algorithm execution:
    mads.run();
    
    Slave::stop_slaves ( out );
    end();
    
    return EXIT_SUCCESS;
}
