#include "debug.h"

#include "../core/logging.h"

#include <memory>
#include <sstream>

#if !defined __VS
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#endif

namespace carl {

#if defined __VS
// Windows
void printStacktrace(bool interaction) {
    // TODO implement for windows
}

std::string demangle(const char* name) {
    // TODO implement for windows
    return "Not implemented";
}

std::string callingFunction() {
    // TODO implement for windows
    return "Not implemented";
}
#else
// Linux
void printStacktrace(bool interaction) {
    std::stringstream cmd;
    cmd << "gdb --pid=" << getpid() << " -ex bt";
    if (!interaction)
        cmd << " --batch --quiet";
    int res = system(cmd.str().c_str());  // NOLINT
    (void)res;                            // ignore.
}

std::string demangle(const char* name) {
    int status = -4;
    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
    return (status == 0) ? res.get() : name;
}

std::string callingFunction() {
    void* frames[3];
    int cnt = backtrace(frames, sizeof(frames) / sizeof(void*));
    if (cnt == 0)
        return "<unknown, maybe corrupt>";
    std::unique_ptr<char*, void (*)(void*)> symbols{backtrace_symbols(frames, cnt), std::free};

    std::stringstream out;
    Dl_info info = {};
    if (dladdr(frames[2], &info) != 0 && info.dli_sname != nullptr) {
        out << demangle(info.dli_sname) << std::endl;
    } else {
        out << "??? " << demangle(symbols.get()[2]) << std::endl;
    }
    return out.str();
}
#endif

#ifndef NDEBUG

std::string last_assertion_string;
int last_assertion_code = 23;

/**
 * Actual signal handler.
 */
#ifdef __VS
__declspec(noreturn) static void handle_signal(int signal) {
#else
[[noreturn]] static void handle_signal(int signal) {
#endif
    // printStacktrace(false);
    std::cerr << std::endl << "Catched SIGABRT " << signal << ", exiting with " << (last_assertion_code % 256) << std::endl;
    if (!last_assertion_string.empty()) {
        std::cerr << "Last Assertion catched is: " << last_assertion_string << std::endl;
        std::cerr << "Please check if this is the assertion that is actually thrown." << std::endl;
    }
    exit(last_assertion_code % 256);
}
/**
 * Installs the signal handler.
 */
static bool install_signal_handler() noexcept {
    CARL_LOG_INFO("carl.util", "Installing signal handler for SIGABRT");
    std::signal(SIGABRT, handle_signal);
    return true;
}
/**
 * Static variable that ensures that install_signal_handler is called.
 */
static bool signal_installed = install_signal_handler();
#endif

}  // namespace carl
