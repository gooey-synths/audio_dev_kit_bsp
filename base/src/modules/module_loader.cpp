#include "module_loader.hpp"

namespace graph_infrastructure {

///
/// Modules that utilize board HW
///
std::unordered_map<module_basics::ModuleIdType, std::function<module_basics::ModuleInterface*(board::BoardInterface&)>> ModuleLoader::sHardwareModules = {
    // Add hardware modules as they become available.
};

///
/// Modules that are hard coded into the FW
///
std::unordered_map<module_basics::ModuleIdType, std::function<module_basics::ModuleInterface*()>> ModuleLoader::sStaticModules = {
    // Add static modules as they become available.
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
module_basics::ModuleInterface* ModuleLoader::loadModule(module_basics::ModuleIdType modId) {
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
module_basics::ModuleInterface* ModuleLoader::loadHardwareModule(module_basics::ModuleIdType modId) {
    auto it = sHardwareModules.find(modId);
    if (it != sHardwareModules.end()) {
        return it->second(mBoard);
    }
    return NULL;
}

///
/// Attempt to load a statically loaded module.
/// @param modId Module ID to load.
/// @return Pointer to new module on the heap, NULL if not found.
///
module_basics::ModuleInterface* ModuleLoader::loadStaticModule(module_basics::ModuleIdType modId) {
    auto it = sStaticModules.find(modId);
    if (it != sStaticModules.end()) {
        return it->second();
    }
    return NULL;
}

} // namespace graph_infrastructure
