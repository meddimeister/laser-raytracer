/*-------------------------------------------------------------------------------------*/
/*           example of a user search for the BEALE test function           */
/*-------------------------------------------------------------------------------------*/
#include "nomad.hpp"
using namespace std;
using namespace NOMAD;

/*------------------------------------------------*/
/*               The problem                      */
/*------------------------------------------------*/
/*       n=2, m=1                                 */
/*       Beale function                           */
/*------------------------------------------------*/
double f ( const Eval_Point & x )
{
    double f = 0.0;
    
    f += pow ( 1.5 - x[0].value()*(1-pow(x[1].value(),1.0)) , 2.0 );
    f += pow ( 2.25 - x[0].value()*(1-pow(x[1].value(),2.0)) , 2.0 );
    f += pow ( 2.625 - x[0].value()*(1-pow(x[1].value(),3.0)) , 2.0 );
    return f;
}

double dfdx0 ( const Eval_Point & x )
{
    double df0 = 0.0;
    df0 += 2.0 * ( x[0].value() * ( x[0].value() * (x[1].value()-1.0) + 1.5 ) );
    df0 += 2.0 * ( pow(x[1].value(), 2.0) -1.0 ) * ( x[0].value()*(pow(x[1].value(),2.0)-1.0 ) +9.0/4.0 );
    df0 += 2.0 * ( pow(x[1].value(),3.0 ) -1.0 ) * ( x[0].value() * ( pow(x[1].value(), 3.0 ) -1.0 ) + 21.0/8.0 );
    return df0;
}


double dfdx1 ( const Eval_Point & x )
{
    double df1 = 0.0;
    df1 += 2.0 * x[0].value() * (x[0].value()*(x[1].value()-1.0)+1.5);
    df1 += 4.0 * x[0].value() * x[1].value() * ( x[0].value() * (pow(x[1].value(),2.0) -1.0 ) + 9.0/4.0 );
    df1 += 6.0*x[0].value() * pow(x[1].value(),2.0) * ( x[0].value()*(pow(x[1].value(),3.0) -1.0 ) + 21.0/8.0 );
    return df1;
}

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
    double DF0 = dfdx0(*xk);
    double DF1 = dfdx1(*xk);
    
    // If df too small --> variable unchanged
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
    const int max_iter_abs = 1;
    if ( display_degree == NOMAD::FULL_DISPLAY )
    {
        out << "Max number of iterations for gradient descent poll frame interior line search: " << max_iter_abs <<endl;
    }
    
    while ( ! success && i < max_iter_abs )
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
    
    if ( !success && i == max_iter_abs && display_degree == NOMAD::FULL_DISPLAY )
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
    
    p.set_X0 ( Point ( 2 , 1.0 ) );  // starting point
    
    p.set_DISABLE_MODELS();
    
    p.set_SPECULATIVE_SEARCH ( false );
    
    p.set_INITIAL_MESH_SIZE        ( 1.0 );
    
    //p.set_DISPLAY_DEGREE ( NOMAD::FULL_DISPLAY );
    p.set_DISPLAY_DEGREE ( NOMAD::NORMAL_DISPLAY );
    
    p.set_DISPLAY_STATS ( "bbe sol obj" );
    
    p.set_MAX_BB_EVAL ( 50 );
    
    // parameters validation:
    p.check();
    
    // custom evaluator creation:
    My_Evaluator ev ( p );
    
    // algorithm creation:
    Mads mads ( p , &ev );
    
    // user search:
    My_Search my_search  ( p );
    mads.set_user_search ( &my_search );
    
    // algorithm execution:
    mads.run();
    
    Slave::stop_slaves ( out );
    end();
    
    return EXIT_SUCCESS;
}
