#pragma once

#include "module_basics.hpp"

namespace graph_infrastructure {

///
/// Interface for module loaders.
///
class ModuleLoaderInterface {
public:

    ///
    /// Default destructor.
    ///
    virtual ~ModuleLoaderInterface() = default;

    ///
    /// Load and return a new module.
    /// @param moduleId Id of the module to load.
    /// @return Module pointer or NULL if module could not be found.
    ///
    virtual module_basics::ModuleInterface* loadModule(uint64_t moduleId) = 0;
};

} // namespace graph_infrastructure
