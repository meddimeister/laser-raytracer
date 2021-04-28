#include "trace.h"

ostream &operator<<(ostream &stream, const TraceResult2D &traceResult) {
  return stream << "TraceResult2D: {t: " << traceResult.t
                << ", hit: " << traceResult.hit << ", normal: ["
                << traceResult.normal.x << ", " << traceResult.normal.y << "]"
                << "}";
}

ostream &operator<<(ostream &stream, const TraceResult3D &traceResult) {
  return stream << "TraceResult3D: {t: " << traceResult.t
                << ", hit: " << traceResult.hit << ", normal: ["
                << traceResult.normal.x << ", " << traceResult.normal.y << ", "
                << traceResult.normal.z << "]"
                << "}";
}
