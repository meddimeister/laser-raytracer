#pragma once
#include "Cache/CacheBase.hpp"
#include "Nomad/nomad.hpp"
#include "vecn.h"
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

using namespace std;

template <size_t N> class My_Evaluator : public NOMAD::Evaluator {
public:
  function<double(const vecn<double, N>)> f;

  My_Evaluator(function<double(const vecn<double, N> &)> functional,
               const shared_ptr<NOMAD::EvalParameters> &evalParams)
      : NOMAD::Evaluator(evalParams, NOMAD::EvalType::BB), f(functional) {}

  ~My_Evaluator() {}

  bool eval_x(NOMAD::EvalPoint &x, const NOMAD::Double &hMax,
              bool &countEval) const override {
    bool eval_ok = false;
    vecn<double, N> x_curr;
    for (size_t i = 0; i < N; ++i) {
      x_curr[i] = double(x[i].todouble());
    }

    try {
      double f_curr = f(x_curr);
      auto bbOutputType =
          _evalParams->getAttributeValue<NOMAD::BBOutputTypeList>(
              "BB_OUTPUT_TYPE");
      string bbo = to_string(f_curr);

      x.setBBO(bbo);

      eval_ok = true;
    } catch (exception &e) {
      string err("Exception: ");
      err += e.what();
      throw logic_error(err);
    }

    countEval = true;
    return eval_ok;
  }
};

template <size_t N>
vector<vecn<double, N>> runNomad(function<double(const vecn<double, N> &)> f,
                                const vecn<double, N> &xStart,
                                const vecn<double, N> &lowerBounds,
                                const vecn<double, N> &upperBounds) {
  NOMAD::MainStep TheMainStep;

  auto allParams = make_shared<NOMAD::AllParameters>();
  // Parameters creation
  // Number of variables
  allParams->setAttributeValue("DIMENSION", N);
  // The algorithm terminates after
  // this number of black-box evaluations
  allParams->setAttributeValue("MAX_BB_EVAL", 1000);
  allParams->setAttributeValue("MAX_EVAL", 1000);
  allParams->setAttributeValue("NB_THREADS_OPENMP", 1);
  // Starting point and bounds
  NOMAD::Point x_0(N, 0.0);
  NOMAD::ArrayOfDouble lb(N, 0.0);
  NOMAD::ArrayOfDouble ub(N, 0.0);

  for (size_t i = 0; i < N; ++i) {
    x_0[i] = NOMAD::Double(xStart[i]);
    lb[i] = NOMAD::Double(lowerBounds[i]);
    ub[i] = NOMAD::Double(upperBounds[i]);
  }

  allParams->setAttributeValue("X0", x_0);
  allParams->setAttributeValue("LOWER_BOUND", lb);
  allParams->setAttributeValue("UPPER_BOUND", ub);

  allParams->getPbParams()->setAttributeValue(
      "GRANULARITY", NOMAD::ArrayOfDouble(N, 0.0000001));

  // Constraints and objective
  NOMAD::BBOutputTypeList bbOutputTypes;
  bbOutputTypes.push_back(NOMAD::BBOutputType::OBJ); // f
  allParams->setAttributeValue("BB_OUTPUT_TYPE", bbOutputTypes);
  allParams->setAttributeValue("DIRECTION_TYPE",
                               NOMAD::DirectionType::ORTHO_2N);
  allParams->setAttributeValue("DISPLAY_DEGREE", 2);
  allParams->setAttributeValue("DISPLAY_UNSUCCESSFUL", false);

  // Parameters validation
  allParams->checkAndComply();

  TheMainStep.setAllParameters(allParams);

  unique_ptr<My_Evaluator<N>> ev(
      new My_Evaluator<N>(f, allParams->getEvalParams()));
  TheMainStep.setEvaluator(std::move(ev));

  vector<vecn<double, N>> bf_ret;
  try {
    TheMainStep.start();
    TheMainStep.run();
    TheMainStep.end();
    vector<NOMAD::EvalPoint> bf;
    NOMAD::CacheBase::getInstance()->findBestFeas(
        bf, NOMAD::Point(N), NOMAD::EvalType::BB, NOMAD::ComputeType::STANDARD,
        nullptr);
    for (const auto &point : bf) {

      vecn<double, N> point_ret;
      for (size_t i = 0; i < N; ++i) {
        point_ret[i] = double(point[i].todouble());
      }
      bf_ret.push_back(point_ret);
    }
  } catch (std::exception &e) {
    std::cerr << "\nNOMAD has been interrupted (" << e.what() << ")\n\n";
  }

  return bf_ret;
}