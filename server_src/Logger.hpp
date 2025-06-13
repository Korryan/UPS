

#ifndef Logger_hpp
#define Logger_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

using namespace std;
class Logger
{
public:
    static void log(string filename, string function_name, string text);
    
};

#endif /* Logger_hpp */
