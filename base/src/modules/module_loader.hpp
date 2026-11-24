#pragma once

#include <functional>
#include "module_basics.hpp"
#include "module_loader_interface.hpp"
#include <board/board_interface.hpp>

namespace graph_infrastructure {

///
/// Class for loading modules
///
class ModuleLoader: public ModuleLoaderInterface {

public:
    ModuleLoader(board::BoardInterface& board);

    virtual module_basics::ModuleInterface* loadModule(module_basics::ModuleIdType moduleId) override;

protected:
    static std::unordered_map<module_basics::ModuleIdType, std::function<module_basics::ModuleInterface*(board::BoardInterface&)>> sHardwareModules;
    static std::unordered_map<module_basics::ModuleIdType, std::function<module_basics::ModuleInterface*()>> sStaticModules;

    module_basics::ModuleInterface* loadHardwareModule(module_basics::ModuleIdType moduleId);

    module_basics::ModuleInterface* loadStaticModule(module_basics::ModuleIdType moduleId);

    board::BoardInterface& mBoard; ///< Reference to the board HW.
};

} // namespace graph_infrastructure

