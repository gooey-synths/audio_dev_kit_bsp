#include "module_loader.hpp"

namespace graph_infrastructure {

///
/// Modules that utilize board HW
///
std::unordered_map<uint64_t, std::function<module_basics::ModuleInterface*(board::BoardInterface&)>> ModuleLoader::sHardwareModules = {

};

///
/// Modules that are hard coded into the FW
///
std::unordered_map<uint64_t, std::function<module_basics::ModuleInterface*()>> ModuleLoader::sStaticModules = {

};

///
/// Constructor.
/// @param board Reference to the board.
///
ModuleLoader::ModuleLoader(board::BoardInterface& board): mBoard(board) {
    ; // Do nothing.
}

///
/// Attempt to load a module.
/// @param modId Module ID to load.
/// @return Pointer to new module on the heap, NULL if not found.
///
module_basics::ModuleInterface* ModuleLoader::loadModule(uint64_t modId) {
    module_basics::ModuleInterface* mod = NULL;

    mod = loadHardwareModule(modId);

    if(!mod) {
        mod = loadStaticModule(modId);
    }

    return mod;
}

///
/// Attempt to load a hardware dependent module.
/// @param modId Module ID to load.
/// @return Pointer to new module on the heap, NULL if not found.
///
module_basics::ModuleInterface* ModuleLoader::loadHardwareModule(uint64_t modId) {
    if(sHardwareModules.count(modId) > 0) {
        return sHardwareModules[modId](mBoard);
    }
    return NULL;
}

///
/// Attempt to load a statically loaded module.
/// @param modId Module ID to load.
/// @return Pointer to new module on the heap, NULL if not found.
///
module_basics::ModuleInterface* ModuleLoader::loadStaticModule(uint64_t modId) {
    if(sStaticModules.count(modId) > 0) {
        return sStaticModules[modId]();
    }
    return NULL;
}

} // namespace graph_infrastructure
