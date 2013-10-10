#pragma once

#include <vector>

#include <stagezeug/stagezeug.h>
#include <stagezeug/AbstractStage.h>

namespace zeug {

class STAGEZEUG_API Pipeline
{
public:
    Pipeline();
    virtual ~Pipeline();

    virtual void execute();

    void addStage(AbstractStage* stage);
    template<typename T, typename... Args>
    void addStages(T stage, Args... stages);
protected:
    std::vector<AbstractStage*> _stages;

    void sortDependencies();
    void addStages() {}
};

template<typename T, typename... Args>
void Pipeline::addStages(T stage, Args... stages)
{
	addStage(stage);
	addStages(stages...);
}

} // namespace zeug