
#ifndef HALCON_BINDINGS_H_
#define HALCON_BINDINGS_H_

#include <vector>
#include <string>
#include <iostream>
#include "halconcpp/HalconCpp.h"

#define HDEV // TODO not here

#ifdef HDEV
#include "hdevengine/HDevEngineCpp.h"
#endif


#define BOOST_PP_VARIADICS 1
#include <boost/preprocessor/tuple/to_seq.hpp>

#ifdef HDEV
class ProcedureCall {
    const std::string path;
    const std::string name;
    std::vector<const HalconCpp::HObject *> iconic_inputs;
    std::vector<const HalconCpp::HTuple *> control_inputs;
    std::vector<HalconCpp::HObject *> iconic_outputs;
    std::vector<HalconCpp::HTuple* > control_outputs;
    HDevEngineCpp::HDevProcedureCall m_proc_call;
    HDevEngineCpp::HDevProcedure proc;
public:
    ProcedureCall(const std::string &p, const std::string &n); // : path(p), name(n) {}

    ProcedureCall& operator()(const HalconCpp::HObject & val){ iconic_inputs.push_back(&val);return *this;}
    ProcedureCall& operator()(const HalconCpp::HTuple & val){ control_inputs.push_back(&val);return *this;}
    ProcedureCall& operator()(HalconCpp::HObject * val){ iconic_outputs.push_back(val);return *this;}
    ProcedureCall& operator()(HalconCpp::HTuple * val){ control_outputs.push_back(val);return *this;}
    ProcedureCall& operator()(){ return *this;} // for void

    void execute();
};

#  define HALCON_PROCEDURE(path, name, ...) (ProcedureCall(path, #name) BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))
#  define CALL_PROCEDURE(path, name, ...) HALCON_PROCEDURE( path, name, __VA_ARGS__ ).execute()

#  define HALCON_PROCEDURE_SEQ(path, name, seq) (ProcedureCall(path, #name) seq)
#  define CALL_PROCEDURE_SEQ(path, name, seq) HALCON_PROCEDURE_SEQ(path, name, seq).execute()

#  define HALCON_PROCEDURE_ARRAY(path, name, array) (ProcedureCall(path, #name) BOOST_PP_TUPLE_TO_SEQ array )
#  define CALL_PROCEDURE_ARRAY(path, name, array) HALCON_PROCEDURE_ARRAY(path, name, array).execute()

#else
    #include <boost/function.hpp>
    #include <boost/bind.hpp>
    #include <boost/preprocessor/seq/transform.hpp>
    #include <boost/preprocessor/seq/push_front.hpp>
    #include <boost/preprocessor/seq/to_tuple.hpp>

    class ProcedureCall {
        boost::function< void(void)> func;
    public:
        ProcedureCall(boost::function< void(void)> f) : func(f) {}
        void execute() { func(); }
    };
#  define PROCEDURE_ELEM_REF(s, data, elem) boost::cref(elem)
#  define HALCON_BIND(name, seq) BOOST_PP_SEQ_TO_TUPLE(BOOST_PP_SEQ_PUSH_FRONT(BOOST_PP_SEQ_TRANSFORM(PROCEDURE_ELEM_REF, 0, seq), name))

#  define HALCON_PROCEDURE(path, name, ...) ProcedureCall(boost::bind HALCON_BIND(name, BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)) ))
#  define CALL_PROCEDURE(path, name, ...) name ( __VA_ARGS__ )

#  define HALCON_PROCEDURE_SEQ(path, name, seq) ProcedureCall(boost::bind HALCON_BIND(name, seq ))
#  define CALL_PROCEDURE_SEQ(path, name, seq) name BOOST_PP_SEQ_TO_TUPLE(seq)

#  include <boost/preprocessor/array/to_tuple.hpp>
#  include <boost/preprocessor/array/to_seq.hpp>
#  define HALCON_PROCEDURE_ARRAY(path, name, array) ProcedureCall(boost::bind HALCON_BIND(name, BOOST_PP_ARRAY_TO_SEQ(array) ))
#  define CALL_PROCEDURE_ARRAY(path, name, array) name BOOST_PP_ARRAY_TO_TUPLE(array)
#endif


#endif // HALCON_BINDINGS_H_

