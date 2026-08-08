#pragma once

#include "module_basics.hpp"
#include "functional"
#include "module_loader_interface.hpp"
#include <board/board_interface.hpp>

namespace graph_infrastructure {

///
/// Class for loading modules
///
class ModuleLoader: public ModuleLoaderInterface {

public:
    ModuleLoader(board::BoardInterface& board);

    virtual module_basics::ModuleInterface* loadModule(uint64_t moduleId) override;

protected:
    static std::unordered_map<uint64_t, std::function<module_basics::ModuleInterface*(board::BoardInterface&)>> sHardwareModules;
    static std::unordered_map<uint64_t, std::function<module_basics::ModuleInterface*()>> sStaticModules;

    module_basics::ModuleInterface* loadHardwareModule(uint64_t moduleId);

    module_basics::ModuleInterface* loadStaticModule(uint64_t moduleId);

    board::BoardInterface& mBoard; ///< Reference to the board HW.
};

} // namespace graph_infrastructure

