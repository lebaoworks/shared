#include <stdint.h>

typedef char* (*PLUGINFUNC) (
    uint32_t,   // argc
    char**      // argv    
);

/**
 * @brief Load plugin
 * 
 * @param path path to plugin
 * @param err null-terminated string describes error, needs to be freed
 * 
 * @return handle to plugin, if NULL, see err
 */
static void* PluginOpen(const char* path, char** err);

/**
 * @brief Close plugin
 * 
 * @param h handle to plugin
 * @param err null-terminated string describes error, needs to be freed
 * 
 * @return 0 if success, error code otherwise, see err
 */
static int PluginClose(void* h, char** err);

/**
 * @brief Call plugin function
 * 
 * @param h handle to plugin
 * @param name function name
 * @param argc number of parameters
 * @param argv parameters
 * @param err null-terminated string describes error, needs to be freed
 * 
 * @return address of the return data
 */
static char* PluginCall(void* h, char* name, uint32_t argc, char** argv, char** err);