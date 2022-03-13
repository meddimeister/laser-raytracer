#pragma once
#include "nomad.hpp"
#include "vecn.h"
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

using namespace std;

template <size_t N> class My_Evaluator : public NOMAD::Multi_Obj_Evaluator {
public:
  function<float(const vecn<float, N>)> f1;
  function<float(const vecn<float, N>)> f2;

  My_Evaluator(function<float(const vecn<float, N> &)> functional1,
               function<float(const vecn<float, N> &)> functional2,
               const NOMAD::Parameters &p)
      : NOMAD::Multi_Obj_Evaluator(p), f1(functional1), f2(functional2) {}

  ~My_Evaluator() {}

  bool eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max,
              bool &count_eval) const {

    vecn<float, N> x_curr;
    for (size_t i = 0; i < N; ++i) {
      x_curr[i] = float(x[i].value());
    }
    float f1_curr = f1(x_curr);
    float f2_curr = f2(x_curr);
    x.set_bb_output(0, NOMAD::Double(f1_curr));
    x.set_bb_output(1, NOMAD::Double(f2_curr));

    count_eval = true; // count a black-box evaluation
    return true;       // the evaluation succeeded
  }
};

template <size_t N>
vector<vecn<float, N>> runNomad(function<float(const vecn<float, N> &)> f1,
                                function<float(const vecn<float, N> &)> f2,
                                const vecn<float, N> &xStart,
                                const vecn<float, N> &lowerBounds,
                                const vecn<float, N> &upperBounds) {

  vector<vecn<float, N>> bf_ret;

  NOMAD::Display out(std::cout);
  out.precision(NOMAD::DISPLAY_PRECISION_STD);

  try {

    // parameters creation:
    NOMAD::Parameters p(out);

    p.set_DIMENSION(N);
    p.set_MAX_BB_EVAL(1000);
    p.set_MAX_EVAL(1000);

    NOMAD::Point x_0(N, 0.0);
    NOMAD::Point lb(N, 0.0);
    NOMAD::Point ub(N, 0.0);

    for (size_t i = 0; i < N; ++i) {
      x_0[i] = NOMAD::Double(xStart[i]);
      lb[i] = NOMAD::Double(lowerBounds[i]);
      ub[i] = NOMAD::Double(upperBounds[i]);
    }

    p.set_X0(x_0);
    p.set_LOWER_BOUND(lb);
    p.set_UPPER_BOUND(ub);

    vector<NOMAD::bb_output_type> bbot(2); // definition of
    bbot[0] = NOMAD::OBJ;
    bbot[1] = NOMAD::OBJ;
    p.set_BB_OUTPUT_TYPE(bbot);
    p.set_DIRECTION_TYPE(NOMAD::ORTHO_2);
    p.set_DISPLAY_STATS("bbe ( sol ) obj");
    p.set_DISPLAY_DEGREE(2);
    // p.set_SOLUTION_FILE("sol.txt");

    // parameters validation:
    p.check();

    // custom evaluator creation:
    My_Evaluator<N> ev(f1, f2, p);

    // algorithm creation and execution:
    NOMAD::Mads mads(p, &ev);
    mads.multi_run();
    auto evalPoint = mads.get_best_feasible();
    vecn<float, N> solution;
    for (size_t i = 0; i < N; ++i) {
      solution[i] = float(evalPoint->value(i));
    }
    bf_ret.push_back(solution);

  } catch (exception &e) {
    cerr << "\nNOMAD has been interrupted (" << e.what() << ")\n\n";
  }

  return bf_ret;
}