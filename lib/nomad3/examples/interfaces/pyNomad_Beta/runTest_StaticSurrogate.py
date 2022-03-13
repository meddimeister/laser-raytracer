import PyNomad
import sys

# This example of blackbox function is for a single process
# The blackbox output must be put in the Eval_Point passed as argument
def bb(x,sgte_eval):
    try:
        dim = x.get_n()
        if ( sgte_eval ):
            f = sum([x.get_coord(i)**2 for i in range(dim)])
        else:    
            f = sum([(x.get_coord(i)-0.01)**2.0 for i in range(dim)])    
        x.set_bb_output(0, f )
    except:
        print ("Unexpected eval error", sys.exc_info()[0])
        return 0 # 1: success 0: failed evaluation
    return 1

x0 = [0.71,0.51,0.51]
lb = [-1,-1,-1]
ub=[]

params = ['BB_OUTPUT_TYPE OBJ','MAX_BB_EVAL 100','UPPER_BOUND * 1','DISABLE MODELS','HAS_SGTE yes'] 

[ x_return , f_return , h_return, nb_evals , nb_iters ,  stopflag ] = PyNomad.optimize(bb,x0,lb,ub,params)
print ('\n NOMAD outputs \n X_sol={} \n F_sol={} \n H_sol={} \n NB_evals={} \n NB_iters={} \n'.format(x_return,f_return,h_return,nb_evals,nb_iters))
