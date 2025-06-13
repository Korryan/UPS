#include "Logger.hpp"

// Logs messages with a timestamp, source file, and function name.
void Logger::log(string file, string function, string text) {
    // Get the current time.
    auto now = time(nullptr);
    char buffer[80];
    // Format the timestamp as YYYY-MM-DD HH:MM:SS.
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Print the formatted log message to the standard output.
    cout << buffer << " [" << file << "] " << function << " : " << text << endl;
}
