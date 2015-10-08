
#ifndef __KLT_API_HPP__
#define __KLT_API_HPP__

#include "KLT/Core/descriptor.hpp"

#include "MDCG/Tools/api.hpp"

#include <cstddef>

class SgVariableSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgType;
class SgInitializedName;
class SgExpression;
class SgStatement;
class SgScopeStatement;
class SgFunctionParameterList;
class SgBasicBlock;

namespace MFB {
  template <template <class Object> class Model>  class Driver;
  template <class Object> class Sage;
}
namespace KLT {
  namespace Utils {
    struct symbol_map_t;
  }

namespace API {

struct host_t : public ::MDCG::Tools::api_t {
  protected:
    SgClassSymbol * kernel_class;
      SgVariableSymbol * kernel_param_field;
      SgVariableSymbol * kernel_device_id_field;
      SgVariableSymbol * kernel_data_field;
      SgVariableSymbol * kernel_loops_field;
      SgVariableSymbol * kernel_num_threads_field;
      SgVariableSymbol * kernel_num_gangs_field;
      SgVariableSymbol * kernel_num_workers_field;

    SgClassSymbol * loop_class;
      SgVariableSymbol * loop_lower_field;
      SgVariableSymbol * loop_upper_field;
      SgVariableSymbol * loop_stride_field;

//  SgClassSymbol * tile_class;
//    SgVariableSymbol * tile_length_field;
//    SgVariableSymbol * tile_stride_field;

    SgClassSymbol * data_class;
      SgVariableSymbol * data_ptr_field;
      SgVariableSymbol * data_base_type_size_field;
      SgVariableSymbol * data_num_sections_field;
      SgVariableSymbol * data_sections_field;
      SgVariableSymbol * data_mode_field;
      SgVariableSymbol * data_liveness_field;

    SgClassSymbol * section_class;
      SgVariableSymbol * section_offset_field;
      SgVariableSymbol * section_length_field;

    SgFunctionSymbol * build_kernel_func;
    SgFunctionSymbol * allocate_data_func;
    SgFunctionSymbol * execute_kernel_func;

    SgFunctionSymbol * push_dataenv_func;
    SgFunctionSymbol * pop_dataenv_func;

  public:
    void load(const MDCG::Model::model_t & model);

  public:
    SgVariableSymbol * insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const;
    void insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const;

    SgStatement * buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;

    SgStatement * buildDataPtrAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildDataSectionOffsetAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const;
    SgStatement * buildDataSectionLengthAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const;
    SgStatement * buildDataModeAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildDataLivenessAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;

    SgStatement * buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;

    SgStatement * buildNumThreadsAssign(SgVariableSymbol * kernel_sym, SgExpression * rhs) const;

    SgStatement * buildNumGangsAssign(SgVariableSymbol * kernel_sym, size_t lvl, SgExpression * rhs) const;
    SgStatement * buildNumWorkersAssign(SgVariableSymbol * kernel_sym, size_t lvl, SgExpression * rhs) const;

    SgStatement * buildDeviceIdAssign(SgVariableSymbol * kernel_sym, SgExpression * device_id) const;

    SgStatement * buildDataAllocation(SgVariableSymbol * data_arr_sym, size_t data_idx, SgExpression * device_id) const;

    SgStatement * buildPushDataEnvironment() const;
    SgStatement * buildPopDataEnvironment() const;
};

struct kernel_t : public ::MDCG::Tools::api_t {
  protected:
    SgClassSymbol * klt_loop_context_class;

    SgFunctionSymbol * get_loop_lower_fnct;
    SgFunctionSymbol * get_loop_upper_fnct;
    SgFunctionSymbol * get_loop_stride_fnct;

    SgFunctionSymbol * get_tile_length_fnct;
    SgFunctionSymbol * get_tile_stride_fnct;

    SgClassSymbol * klt_data_context_class;

    // Thread

    SgVariableSymbol * tid_symbol;

    // OpenCL Specific

    SgFunctionSymbol * get_ocl_group_id_func;
    SgFunctionSymbol * get_ocl_local_id_func;

  public:
    // target: select to load either OpenCL or CUDA symbols
    void load(const MDCG::Model::model_t & model, Descriptor::target_kind_e target);

  public:
    virtual SgInitializedName * buildConstantVariable(const std::string & name, SgType * type, SgInitializer * init, Descriptor::target_kind_e target) const;
    virtual SgInitializedName * buildGlobalVariable(const std::string & name, SgType * type, SgInitializer * init, Descriptor::target_kind_e target) const;
    virtual SgInitializedName * buildLocalVariable(const std::string & name, SgType * type, SgInitializer * init, Descriptor::target_kind_e target) const;

    // default: none
    virtual void applyKernelModifiers(SgFunctionDeclaration * kernel_decl, Descriptor::target_kind_e target) const;

    // default: void
    virtual SgType * buildKernelReturnType(Descriptor::kernel_t & kernel) const;

  public:
    // Loop Context and Getters

    SgType * getLoopContextPtrType() const;

    SgExpression * buildGetLoopLower (size_t loop_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetLoopStride(size_t loop_id, SgVariableSymbol * ctx) const;

    SgExpression * buildGetTileLength(size_t tile_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetTileStride(size_t tile_id, SgVariableSymbol * ctx) const;

    // Data Context and Getters (NIY)

    SgType * getDataContextPtrType() const;

    // Threads

    SgExpression * buildThreadTileIdx() const;

    // Accelerator

    SgExpression * buildGangTileIdx(SgExpression * lvl, Descriptor::target_kind_e target) const;
    SgExpression * buildWorkerTileIdx(SgExpression * lvl, Descriptor::target_kind_e target) const;

  friend struct call_interface_t;
};

struct call_interface_t {
  protected:
    ::MFB::Driver< ::MFB::Sage> & driver;
    kernel_t * kernel_api;

  public:
    call_interface_t(::MFB::Driver< ::MFB::Sage> & driver_, kernel_t * kernel_api_);

    SgFunctionParameterList * buildKernelParamList(Descriptor::kernel_t & kernel) const;
    SgBasicBlock * generateKernelBody(Descriptor::kernel_t & kernel, SgFunctionDefinition * kernel_defn, Utils::symbol_map_t & symbol_map) const;

  protected:
    // default: klt_loop_context_t & loop_ctx (TODO klt_data_context_t & data_ctx)
    void addKernelArgsForContext(SgFunctionParameterList * param_list, Descriptor::target_kind_e target) const;
    void getContextSymbol(SgFunctionDefinition * func_defn, Utils::symbol_map_t & symbol_map) const;

    // default: "loop_it_'loop.id'"
    void createLoopIterator(const std::vector<Descriptor::loop_t *> & loops, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;

    // default: "tile_it_'tile.id'"
    void createTileIterator(const std::vector<Descriptor::tile_t *> & tiles, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb, Descriptor::target_kind_e target) const;

    void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters, Descriptor::target_kind_e target) const;
    void addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data, Descriptor::target_kind_e target) const;

    void getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb, Descriptor::target_kind_e target) const;
    void getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb, Descriptor::target_kind_e target) const;
};

} // namespace KLT::API

} // namespace KLT

#endif /* __KLT_API_HPP__ */

