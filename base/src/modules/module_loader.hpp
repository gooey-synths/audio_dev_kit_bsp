#pragma once

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

    virtual module_basics::ModuleInterface* loadModule(uint64_t moduleId) override;

protected:
    module_basics::ModuleInterface* loadHwModule(uint64_t moduleId);

    module_basics::ModuleInterface* loadDefaultModule(uint64_t moduleId);

    board::BoardInterface& mBoard;
};

} // namespace graph_infrastructure

