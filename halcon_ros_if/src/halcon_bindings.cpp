
#include "halcon_ros_if/halcon_bindings.h"

#include "my_hdevoperatorimpl.h"


using namespace HalconCpp;
using namespace HDevEngineCpp;

class MyHDevEngine: public HDevEngine
{
public:
	MyHDevEngine(): HDevEngine()
	{
		SetHDevOperatorImpl(new MyHDevOperatorImpl);
	}


};

ProcedureCall::ProcedureCall(const std::string &p, const std::string &n) : path(p), name(n)
{
	static MyHDevEngine hdev_engine;
	hdev_engine.UnloadProcedure(n.c_str());
	//hdev_engine.SetProcedurePath(path.c_str());
	hdev_engine.AddProcedurePath(path.c_str());
	//proc = new HDevEngineCpp::HDevProcedure (name.c_str());
	//m_proc_call = proc->CreateCall();
	//hdev_engine.UnloadAllProcedures();

	proc.LoadProcedure(name.c_str());
	m_proc_call = proc.CreateCall();
}

void ProcedureCall::execute(){

    // print function name and arguments
    size_t s = iconic_inputs.size() + control_inputs.size() + iconic_outputs.size() + control_outputs.size();
    std::cout << "call " <<  name << " from " << path <<  " with " << control_inputs.size()<< "input and" << control_outputs.size() << " arguments";
    std::cout << std::endl;

    // add control inputs
    int i = 1;
    for(std::vector<const HalconCpp::HTuple *>::iterator it = control_inputs.begin(); it != control_inputs.end(); ++it)
    {
    	m_proc_call.SetInputCtrlParamTuple(i, **it);
    	i++;
    }

    // add iconic inputs
    i = 1;
    for(std::vector<const HalconCpp::HObject *>::iterator it = iconic_inputs.begin(); it != iconic_inputs.end(); ++it)
    {
    	m_proc_call.SetInputIconicParamObject(i, **it);
    	i++;
    }

    // execute
    m_proc_call.Execute();

    // get iconic outputs

    for(int j = 0; j < iconic_outputs.size(); j++)
    {
    	*iconic_outputs[j] = m_proc_call.GetOutputIconicParamObject(j+1);
    }

    // get control outputs

    for(int j = 0; j < control_outputs.size(); j++)
    {

    	m_proc_call.GetOutputCtrlParamTuple(j+1, control_outputs[j]);
    	std::cout << "result = " << control_outputs[j]->I() << std::endl;
    }
}
