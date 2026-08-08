#include "graph_loader.hpp"

namespace graph_infrastructure {

// JSON keys for graph parsing.
static const char* scModulesJsonKey = "modules";
static const char* scModuleIdJsonKey = "id";
static const char* scModuleArgsJsonKey = "args";
static const char* scConnectionsJsonKey = "connections";
static const char* scConnectionsInputModJsonKey = "input_mod";
static const char* scConnectionsOutputModJsonKey = "output_mod";
static const char* scConnectionsInputPortNameJsonKey = "input_port_name";
static const char* scConnectionsOutputPortNameJsonKey = "output_port_name";

// Shared loading failure messages.
static const char* scInvalidConnectionInputMod = "Invalid connection input module";
static const char* scInvalidConnectionOutputMod = "Invalid connection output module";

///
/// Constructor.
/// @param moduleLoader Module loader to use for loading modules
///
GraphLoader::GraphLoader(ModuleLoaderInterface& moduleLoader): mModuleLoader(moduleLoader) {
    ; // Do nothing.
}

///
/// Destructor.
///
GraphLoader::~GraphLoader() {
    clearGraph(&mGraphA);
    clearGraph(&mGraphB);

    mLoadingGraph = NULL;
    mRunnableGraph = NULL;
}

///
/// Clear a graph.
/// @param graph The graph to clear.
///
void GraphLoader::clearGraph(Graph* graph) {
    for(module_basics::ModuleInterface* mod : graph->mods) {
        delete mod;
    }

    graph->mods.clear();
    graph->cons.clear();
}

///
/// Validate the graph that has been loaded.
/// @param graph The graph to validate.
///
void GraphLoader::validateGraph(Graph* graph) {

    // Check that all module pointers are not NULL
     for(module_basics::ModuleInterface* mod : graph->mods) {
        if(mod == NULL) {
            throw "Invalid module pointer";
        }
    }

    // Check that all connections have valid module indices.
    for(Connection& con : graph->cons) {
        if(con.inModIdx >= graph->mods.size()) {
            throw scInvalidConnectionInputMod;
        }

        if(con.outModIdx >= graph->mods.size()) {
            throw scInvalidConnectionOutputMod;
        }
    }

    // Check that alll connections have a valid module port indices.
    for(Connection& con : graph->cons) {
        module_basics::ModuleInterface* inMod = graph->mods[con.inModIdx];
        module_basics::ModuleInterface* outMod = graph->mods[con.outModIdx];

        if(!inMod->outputExists(con.inPortIdx)) {
            throw "Invalid connection input port";
        }

        if(!outMod->inputExists(con.outPortIdx)) {
            throw "Invalid connection output port";
        }
    }
}

///
/// Load a new graph from input.
/// @param buf Buffer to parse for graph description.
/// @param len Length of buffer.
/// @return New graph ready to be run.
/// @note The GraphLoader is responsible for the memory management of this graph.
///
Graph* GraphLoader::load(char* buf, size_t len) {
    ArduinoJson::JsonDocument doc;
    ArduinoJson::DeserializationError err = deserializeJson(doc, buf, len);

    ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();

    if(err) {
        throw "JSON parsing error";
    }

    // Prepare graph for loading.
    clearGraph(mLoadingGraph);

    parseModules(obj);
    parseConnections(obj);

    // No errors occured during loading, swap loading and runnable graph and return.
    Graph* temp = mRunnableGraph;
    mRunnableGraph = mLoadingGraph;
    mLoadingGraph = temp;

    return mRunnableGraph;
}

///
/// Parse modules and load into graph.
/// @param obj JSON object to parse modules from.
///
void GraphLoader::parseModules(ArduinoJson::JsonObject& obj) {
    ArduinoJson::JsonArray modules = obj[scModulesJsonKey];
    if(modules.isNull()) {
        throw "Invalid modules key";
    }

    // Parse Modules
    for(ArduinoJson::JsonObject mod : modules) {
        if(mod.isNull()) {
            throw "Invalid module structure";
        }

        // Parse module id.
        if(!mod[scModuleIdJsonKey].is<module_basics::ModuleIdType>()) {
            throw "Invalid module ID type";
        }
        module_basics::ModuleIdType moduleId = mod[scModuleIdJsonKey];

        // Parse module arguments
        ArduinoJson::JsonObject args = mod[scModuleArgsJsonKey];
        if(args.isNull()) {
            throw "Invalid args type";
        }

        std::unordered_map<std::string, std::string> moduleArgs;

        for (ArduinoJson::JsonPair kv : args) {
            // JSON keys are always strings
            std::string key = kv.key().c_str();

            if(!kv.value().is<ArduinoJson::JsonString>()) {
                throw "Module arguments must be strings";
            }
            std::string value = kv.value();

            moduleArgs[key] = value;
        }

        module_basics::ModuleInterface* newModule = mModuleLoader.loadModule(moduleId);
        if(!newModule) {
            throw "Module lookup failed";
        }
        mLoadingGraph->mods.push_back(newModule);
        newModule->configure(moduleArgs);
    }
}

///
/// Parse connections and load into graph.
/// @param obj JSON object to parse connections from.
/// @note All modules should be loaded into the graph before calling this.
///
void GraphLoader::parseConnections(ArduinoJson::JsonObject& obj) {
    ArduinoJson::JsonArray connections = obj[scModulesJsonKey];
    if(connections.isNull()) {
        throw "Invalid connections key";
    }

    // Parse Connections
    for(ArduinoJson::JsonObject connection : connections) {
        if(connection.isNull()) {
            throw "Invalid connection structure";
        }

        // Check that all of the types are expected
        if(!connection[scConnectionsInputModJsonKey].is<ArduinoJson::JsonInteger>()) {
            throw "Invalid connection input module type";
        }

        if(!connection[scConnectionsOutputModJsonKey].is<ArduinoJson::JsonInteger>()) {
            throw "Invalid connection output module type";
        }

        if(!connection[scConnectionsInputPortNameJsonKey].is<ArduinoJson::JsonString>()) {
            throw "Invalid connection input port type";
        }

        if(!connection[scConnectionsOutputPortNameJsonKey].is<ArduinoJson::JsonString>()) {
            throw "Invalid connection output port type";
        }

        // Read and check module indices
        size_t inputModIdx = connection[scConnectionsInputModJsonKey];
        size_t outputModIdx = connection[scConnectionsOutputModJsonKey];

        if(inputModIdx >= mLoadingGraph->mods.size()) {
            throw scInvalidConnectionInputMod;
        }

        if(outputModIdx >= mLoadingGraph->mods.size()) {
            throw scInvalidConnectionOutputMod;
        }

        // Lookup port indices
        std::string inputPortName = connection[scConnectionsInputPortNameJsonKey];
        std::string outputPortName = connection[scConnectionsOutputPortNameJsonKey];

        size_t inputPortIdx = mLoadingGraph->mods[inputModIdx]->getInputIdx(inputPortName);
        size_t outputPortIdx = mLoadingGraph->mods[outputModIdx]->getOutputIdx(outputPortName);

        // Create new connection
        Connection newConnection = {
            .inModIdx = inputModIdx,
            .inPortIdx = inputPortIdx,
            .outModIdx = outputModIdx,
            .outPortIdx = outputPortIdx
        };

        mLoadingGraph->cons.push_back(newConnection);
    }
}

} // namespace graph_infrastructure

