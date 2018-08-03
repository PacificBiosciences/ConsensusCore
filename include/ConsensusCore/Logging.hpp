// Author: David Alexander

#include <cpplog/cpplog.hpp>

namespace ConsensusCore {
class Logging
{
public:
    static void EnableDiagnosticLogging();

#ifndef SWIG
    static cpplog::StdErrLogger* slog;
    static cpplog::FilteringLogger* flog;
#endif  // SWIG
};
}

#define LTRACE LOG_TRACE(*Logging::flog)
#define LDEBUG LOG_DEBUG(*Logging::flog)
#define LINFO LOG_INFO(*Logging::flog)
#define LWARN LOG_WARN(*Logging::flog)
#define LERROR LOG_ERROR(*Logging::flog)
#define LFATAL LOG_FATAL(*Logging::flog)
