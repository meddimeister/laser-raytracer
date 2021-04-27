#include "trace.h"

ostream &operator<<(ostream &stream, const TraceResult &traceResult) {
  return stream << "TraceResult: {t: " << traceResult.t
         << ", hit: " << traceResult.hit << "}";
}
