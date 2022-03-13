import PyNomad
import sys
import matplotlib.pyplot as plt

def handle_close(evt):
    plt.close()
    sys.exit()

min_x=0
max_x=50
min_f=0
max_f=10
delta_x = 10
delta_f = 10
fig = plt.figure()
fig.canvas.mpl_connect('close_event', handle_close)
plt.axis([min_x, max_x, min_f, max_f])
plt.ylabel('f')
plt.xlabel('evals')
plot_legend_imp_once = True
plot_legend_noimp_once = True

plt.ion()

eval = 0
fbest = 1E6

def bb(x):
    global eval, fbest
    global min_x, max_x,min_f,max_f
    global plot_legend_noimp_once,plot_legend_imp_once
    try:
        eval = eval + 1
        dim = x.get_n()
        f = x.get_coord(4)
        x.set_bb_output(0,f)
        g1 = sum([(x.get_coord(i)-1)**2 for i in range(dim)])-25
        x.set_bb_output(1,g1)
        g2 = 25-sum([(x.get_coord(i)+1)**2 for i in range(dim)])
        x.set_bb_output(2,g2)

        # Plot history
        adjust_axis = False
        if f < fbest and g1 < 0 and g2 < 0:
            fbest = f
            if eval > max_x:
                max_x = max_x + delta_x
                adjust_axis = True
            if fbest < min_f:
                min_f = min_f - delta_f
                adjust_axis = True
            elif fbest>max_f:
                max_f = max_f + delta_f
                adjust_axis = True
            if adjust_axis:
                plt.axis([min_x, max_x, min_f, max_f])
            plt.ion()
            if plot_legend_imp_once == True:
                plt.plot(eval, fbest,'bo',label='improving f')
                plt.legend()
                plot_legend_imp_once = False
            else:
                plt.plot(eval, fbest,'bo')
    
        else:
            if eval > max_x:
                max_x = max_x + delta_x
                adjust_axis = True
            if adjust_axis:
                plt.axis([min_x, max_x, min_f, max_f])
            if plot_legend_noimp_once == True:
                plt.plot(eval,f,'ro',label='not improving f')
                plt.legend()
                plot_legend_noimp_once = False
            else:
                plt.plot(eval,f,'ro')
        plt.pause(0.05)

    except:
        print ("Unexpected eval error", sys.exc_info()[0])
        return 0
    return 1



ub = [ 5,6,7,10,10]

params = ['BB_OUTPUT_TYPE OBJ PB EB','MAX_BB_EVAL 100','X0 * 0' , 'LOWER_BOUND * -6' ] 
[ x_return , f_return , h_return, nb_evals , nb_iters ,  stopflag ] = PyNomad.optimize(bb,[],[],ub,params)
print ('\n NOMAD outputs \n X_sol={} \n F_sol={} \n H_sol={} \n NB_evals={} \n NB_iters={} \n'.format(x_return,f_return,h_return,nb_evals,nb_iters))

plt.title('Close the window to end python process')

while True:
    plt.pause(0.1)
