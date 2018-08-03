// Author: David Alexander

#include <ConsensusCore/Logging.hpp>
#include <cpplog/cpplog.hpp>

namespace ConsensusCore {
void Logging::EnableDiagnosticLogging()
{
    delete flog;
    flog = new cpplog::FilteringLogger(LL_TRACE, slog);
}

cpplog::StdErrLogger* Logging::slog = new cpplog::StdErrLogger();
cpplog::FilteringLogger* Logging::flog = new cpplog::FilteringLogger(LL_WARN, slog);
}
