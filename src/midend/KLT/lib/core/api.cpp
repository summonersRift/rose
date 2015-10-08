
#include "sage3basic.h"

#include "MFB/utils.hpp"

#include "MDCG/Model/model.hpp"
#include "MDCG/Model/function.hpp"
#include "MDCG/Model/class.hpp"

#include "KLT/Core/api.hpp"
#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/utils.hpp"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace KLT {

namespace API {

////// KLT::API::host_t

void host_t::load(const MDCG::Model::model_t & model) {
  bool res = true;

  MDCG::Model::class_t class_;
  MDCG::Model::field_t field_;
  MDCG::Model::function_t function_;

    res = api_t::load(class_   , kernel_class              , model, "klt_kernel_t"       , NULL);   assert(res == true);
      res = api_t::load(field_ , kernel_device_id_field    , model,   "device_id"        , class_); assert(res == true);
      res = api_t::load(field_ , kernel_param_field        , model,   "param"            , class_); assert(res == true);
      res = api_t::load(field_ , kernel_data_field         , model,   "data"             , class_); assert(res == true);
      res = api_t::load(field_ , kernel_loops_field        , model,   "loops"            , class_); assert(res == true);
      res = api_t::load(field_ , kernel_num_threads_field  , model,   "num_threads"      , class_); assert(res == true);
      res = api_t::load(field_ , kernel_num_gangs_field    , model,   "num_gangs"        , class_); assert(res == true);
      res = api_t::load(field_ , kernel_num_workers_field  , model,   "num_workers"      , class_); assert(res == true);

    res = api_t::load(class_   , loop_class                , model, "klt_loop_t"         , NULL);   assert(res == true);
      res = api_t::load(field_ , loop_lower_field          , model,   "lower"            , class_); assert(res == true);
      res = api_t::load(field_ , loop_upper_field          , model,   "upper"            , class_); assert(res == true);
      res = api_t::load(field_ , loop_stride_field         , model,   "stride"           , class_); assert(res == true);

//  res = api_t::load(class_   , tile_class                , model, "klt_tile_t"         , NULL);   assert(res == true);
//    res = api_t::load(field_ , tile_length_field         , model,   "length"           , class_); assert(res == true);
//    res = api_t::load(field_ , tile_stride_field         , model,   "stride"           , class_); assert(res == true);

    res = api_t::load(class_   , data_class                , model, "klt_data_t"         , NULL);   assert(res == true);
      res = api_t::load(field_ , data_ptr_field            , model,   "ptr"              , class_); assert(res == true);
      res = api_t::load(field_ , data_base_type_size_field , model,   "base_type_size"   , class_); assert(res == true);
      res = api_t::load(field_ , data_num_sections_field   , model,   "num_sections"     , class_); assert(res == true);
      res = api_t::load(field_ , data_sections_field       , model,   "sections"         , class_); assert(res == true);
      res = api_t::load(field_ , data_mode_field           , model,   "mode"             , class_); assert(res == true);
      res = api_t::load(field_ , data_liveness_field       , model,   "liveness"         , class_); assert(res == true);

    res = api_t::load(class_   , section_class             , model, "klt_data_section_t" , NULL);   assert(res == true);
      res = api_t::load(field_ , section_offset_field      , model,   "offset"           , class_); assert(res == true);
      res = api_t::load(field_ , section_length_field      , model,   "length"           , class_); assert(res == true);

    res = api_t::load(function_, build_kernel_func         , model, "klt_build_kernel"   , NULL);   assert(res == true);
    res = api_t::load(function_, execute_kernel_func       , model, "klt_execute_kernel" , NULL);   assert(res == true);
    res = api_t::load(function_, allocate_data_func        , model, "klt_allocate_data"  , NULL);   assert(res == true);

    res = api_t::load(function_, push_dataenv_func         , model, "klt_push_data_environment"  , NULL);   assert(res == true);
    res = api_t::load(function_, pop_dataenv_func          , model, "klt_pop_data_environment"   , NULL);   assert(res == true);
}

//////

SgVariableSymbol * host_t::insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const {
  SgInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
                           build_kernel_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(kernel_id))
                         ));
  SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration("kernel", SageBuilder::buildPointerType(kernel_class->get_type()), init, scope);
  SageInterface::appendStatement(kernel_decl, scope);

  SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
  assert(kernel_sym != NULL);

  return kernel_sym;
}

void host_t::insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const {
  SageInterface::appendStatement(
    SageBuilder::buildFunctionCallStmt(
      SageBuilder::buildFunctionRefExp(execute_kernel_func),
      SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(kernel_sym))
    ), scope
  );
}

SgStatement * host_t::buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_param_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

SgStatement * host_t::buildDataPtrAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_ptr_field), rhs));
}

SgStatement * host_t::buildDataSectionOffsetAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
           MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_sections_field), SageBuilder::buildIntVal(dim)
         ), SageBuilder::buildVarRefExp(section_offset_field)), rhs));
}

SgStatement * host_t::buildDataSectionLengthAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
           MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_sections_field), SageBuilder::buildIntVal(dim)
         ), SageBuilder::buildVarRefExp(section_length_field)), rhs));
}

SgStatement * host_t::buildDataModeAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_mode_field), rhs));
}

SgStatement * host_t::buildDataLivenessAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_liveness_field), rhs));
}

SgStatement * host_t::buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loops_field, SageBuilder::buildIntVal(idx), loop_lower_field), rhs));
}

SgStatement * host_t::buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loops_field, SageBuilder::buildIntVal(idx), loop_upper_field), rhs));
}

SgStatement * host_t::buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loops_field, SageBuilder::buildIntVal(idx), loop_stride_field), rhs));
}

SgStatement * host_t::buildNumThreadsAssign(SgVariableSymbol * kernel_sym, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_num_threads_field, NULL, NULL), rhs));
}

SgStatement * host_t::buildNumGangsAssign(SgVariableSymbol * kernel_sym, size_t lvl, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_num_gangs_field, SageBuilder::buildIntVal(lvl), NULL), rhs));
}

SgStatement * host_t::buildNumWorkersAssign(SgVariableSymbol * kernel_sym, size_t lvl, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_num_workers_field, SageBuilder::buildIntVal(lvl), NULL), rhs));
}

SgStatement * host_t::buildDeviceIdAssign(SgVariableSymbol * kernel_sym, SgExpression * device_id) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
    SageBuilder::buildArrowExp(SageBuilder::buildVarRefExp(kernel_sym), SageBuilder::buildVarRefExp(kernel_device_id_field)),
    SageInterface::copyExpression(device_id)
  ));
}

SgStatement * host_t::buildDataAllocation(SgVariableSymbol * data_arr_sym, size_t data_idx, SgExpression * device_id) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildFunctionCallExp(allocate_data_func, SageBuilder::buildExprListExp(
    SageBuilder::buildAddressOfOp(SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(data_arr_sym), SageBuilder::buildIntVal(data_idx))),
    device_id != NULL ? SageInterface::copyExpression(device_id) : SageBuilder::buildIntVal(0)
  )));
}

SgStatement * host_t::buildPushDataEnvironment() const {
  return SageBuilder::buildExprStatement(SageBuilder::buildFunctionCallExp(push_dataenv_func, SageBuilder::buildExprListExp()));
}

SgStatement * host_t::buildPopDataEnvironment() const {
  return SageBuilder::buildExprStatement(SageBuilder::buildFunctionCallExp(pop_dataenv_func, SageBuilder::buildExprListExp()));
}

////// KLT::API::kernel_t

void kernel_t::load(const MDCG::Model::model_t & model, Descriptor::target_kind_e target) {
  bool res = true;

  MDCG::Model::class_t class_;
  res = api_t::load(class_, klt_loop_context_class, model, "klt_loop_context_t", NULL); assert(res == true);
  res = api_t::load(class_, klt_data_context_class, model, "klt_data_context_t", NULL); assert(res == true);

  MDCG::Model::function_t function_;
  res = api_t::load(function_,  get_loop_lower_fnct, model, "klt_get_loop_lower" , NULL); assert(res == true);
  res = api_t::load(function_,  get_loop_upper_fnct, model, "klt_get_loop_upper" , NULL); assert(res == true);
  res = api_t::load(function_, get_loop_stride_fnct, model, "klt_get_loop_stride", NULL); assert(res == true);
  res = api_t::load(function_, get_tile_length_fnct, model, "klt_get_tile_length", NULL); assert(res == true);
  res = api_t::load(function_, get_tile_stride_fnct, model, "klt_get_tile_stride", NULL); assert(res == true);

  if (target == Descriptor::e_target_opencl) {
    assert(SageInterface::getProject() != NULL);
    assert(SageInterface::getProject()->get_globalScopeAcrossFiles() != NULL);

    get_ocl_group_id_func = SageInterface::getProject()->get_globalScopeAcrossFiles()->lookup_function_symbol("get_group_id");
    assert(get_ocl_group_id_func != NULL);

    get_ocl_local_id_func = SageInterface::getProject()->get_globalScopeAcrossFiles()->lookup_function_symbol("get_local_id");
    assert(get_ocl_local_id_func != NULL);
  }
  else {
    get_ocl_group_id_func = NULL;
    get_ocl_local_id_func = NULL;
  }
}

SgInitializedName * kernel_t::buildConstantVariable(
  const std::string & name,
  SgType * type,
  SgInitializer * init,
  Descriptor::target_kind_e target
) const {
  if (target == Descriptor::e_target_opencl) {
    SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclConstant();
    type = mod_type;
  }
  return SageBuilder::buildInitializedName(name, type, init);
}

SgInitializedName * kernel_t::buildGlobalVariable(
  const std::string & name,
  SgType * type,
  SgInitializer * init,
  Descriptor::target_kind_e target
) const {
  if (target == Descriptor::e_target_opencl) {
    SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclGlobal();
    type = mod_type;
  }
  return SageBuilder::buildInitializedName(name, type, init);
}

SgInitializedName * kernel_t::buildLocalVariable(
  const std::string & name,
  SgType * type,
  SgInitializer * init,
  Descriptor::target_kind_e target
) const {
  if (target == Descriptor::e_target_opencl) {
    SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclLocal();
    type = mod_type;
  }
  return SageBuilder::buildInitializedName(name, type, init);
}

void kernel_t::applyKernelModifiers(
  SgFunctionDeclaration * kernel_decl,
  Descriptor::target_kind_e target
) const {
  if (target == Descriptor::e_target_opencl)
    kernel_decl->get_functionModifier().setOpenclKernel();
}

SgType * kernel_t::buildKernelReturnType(
  Descriptor::kernel_t & kernel
) const {
  return SageBuilder::buildVoidType();
}

//////

SgType * kernel_t::getLoopContextPtrType() const {
  return SageBuilder::buildPointerType(klt_loop_context_class->get_declaration()->get_type());
}

SgExpression * kernel_t::buildGetLoopLower (size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_lower_fnct);
}
SgExpression * kernel_t::buildGetLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_upper_fnct);
}
SgExpression * kernel_t::buildGetLoopStride(size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_stride_fnct);
}

SgExpression * kernel_t::buildGetTileLength(size_t tile_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(tile_id, ctx, get_tile_length_fnct);
}
SgExpression * kernel_t::buildGetTileStride(size_t tile_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(tile_id, ctx, get_tile_stride_fnct);
}

SgType * kernel_t::getDataContextPtrType() const {
  return SageBuilder::buildPointerType(klt_data_context_class->get_declaration()->get_type());
}

SgExpression * kernel_t::buildThreadTileIdx() const {
  assert(tid_symbol != NULL);
  return SageBuilder::buildVarRefExp(tid_symbol);
}

SgExpression * kernel_t::buildGangTileIdx(SgExpression * lvl, Descriptor::target_kind_e target) const {
  switch (target) {
    case Descriptor::e_target_opencl:
      assert(get_ocl_group_id_func != NULL);
      return SageBuilder::buildFunctionCallExp(get_ocl_group_id_func, SageBuilder::buildExprListExp(lvl));
    case Descriptor::e_target_cuda:
    default:
      assert(false);
  }
}

SgExpression * kernel_t::buildWorkerTileIdx(SgExpression * lvl, Descriptor::target_kind_e target) const {
  switch (target) {
    case Descriptor::e_target_opencl:
      assert(get_ocl_local_id_func != NULL);
      return SageBuilder::buildFunctionCallExp(get_ocl_local_id_func, SageBuilder::buildExprListExp(lvl));
    case Descriptor::e_target_cuda:
    default:
      assert(false);
  }
}

////// KLT::API::call_interface_t

call_interface_t::call_interface_t(::MFB::Driver< ::MFB::Sage> & driver_, kernel_t * kernel_api_) : driver(driver_), kernel_api(kernel_api_) {}

void call_interface_t::addKernelArgsForParameter(
  SgFunctionParameterList * param_list,
  const std::vector<SgVariableSymbol *> & parameters,
  Descriptor::target_kind_e target
) const {
  switch (target) {
    case Descriptor::e_target_host:
    case Descriptor::e_target_threads:
      param_list->append_arg(SageBuilder::buildInitializedName("param", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));
      break;
    case Descriptor::e_target_opencl:
    case Descriptor::e_target_cuda:
    {
      std::vector<SgVariableSymbol *>::const_iterator it;
      for (it = parameters.begin(); it != parameters.end(); it++) {
        SgVariableSymbol * param_sym = *it;
        std::string param_name = param_sym->get_name().getString();
        SgType * param_type = param_sym->get_type();
        param_list->append_arg(SageBuilder::buildInitializedName(param_name, param_type, NULL));
      }
      break;
    }
    default:
      assert(false);
  }
}

void call_interface_t::addKernelArgsForData(
  SgFunctionParameterList * param_list,
  const std::vector<Descriptor::data_t *> & data,
  Descriptor::target_kind_e target
) const {
  switch (target) {
    case Descriptor::e_target_host:
    case Descriptor::e_target_threads:
      param_list->append_arg(SageBuilder::buildInitializedName("data", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));
      break;
    case Descriptor::e_target_opencl:
    case Descriptor::e_target_cuda:
    {
      std::vector<Descriptor::data_t *>::const_iterator it;
      for (it = data.begin(); it != data.end(); it++) {
        SgVariableSymbol * data_sym = (*it)->symbol;
        std::string data_name = data_sym->get_name().getString();
        SgType * data_type = SageBuilder::buildPointerType((*it)->base_type);
        param_list->append_arg(kernel_api->buildGlobalVariable(data_name, data_type, NULL, target));
      }
      break;
    }
    default:
      assert(false);
  }
}

void call_interface_t::addKernelArgsForContext(SgFunctionParameterList * param_list, Descriptor::target_kind_e target) const {
  param_list->append_arg(kernel_api->buildConstantVariable("loop_ctx", kernel_api->getLoopContextPtrType(), NULL, target));
  param_list->append_arg(kernel_api->buildConstantVariable("data_ctx", kernel_api->getDataContextPtrType(), NULL, target));
}

SgFunctionParameterList * call_interface_t::buildKernelParamList(Descriptor::kernel_t & kernel) const {
  SgFunctionParameterList * res = SageBuilder::buildFunctionParameterList();

  std::cerr << "[Info] call_interface_t::buildKernelParamList " << kernel.target << std::endl;
  if (kernel.target == Descriptor::e_target_threads) {
    std::cerr << "[Info] Create 'tid' argument for Threads kernel..." << std::endl;
    res->append_arg(SageBuilder::buildInitializedName("tid", SageBuilder::buildIntType(), NULL));
  }

  addKernelArgsForParameter(res, kernel.parameters, kernel.target);
  addKernelArgsForData     (res, kernel.data, kernel.target);
  addKernelArgsForContext  (res, kernel.target);

  return res;
}

void call_interface_t::getContextSymbol(SgFunctionDefinition * func_defn, Utils::symbol_map_t & symbol_map) const {
  symbol_map.loop_context = func_defn->lookup_variable_symbol("loop_ctx"); assert(symbol_map.loop_context != NULL);
  symbol_map.data_context = func_defn->lookup_variable_symbol("data_ctx"); assert(symbol_map.data_context != NULL);
}

void call_interface_t::createLoopIterator(const std::vector<Descriptor::loop_t *> & loops, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
  std::vector<Descriptor::loop_t *>::const_iterator it;
  for (it = loops.begin(); it != loops.end(); it++) {
    std::ostringstream oss; oss << "l_" << (*it)->id;
    SgVariableSymbol * symbol = MFB::Utils::getExistingSymbolOrBuildDecl(oss.str(), (*it)->iterator->get_type(), bb);
    symbol_map.iter_loops.insert(std::pair<size_t, SgVariableSymbol *>((*it)->id, symbol));
    symbol_map.orig_loops.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>((*it)->iterator, symbol));
  }
}

void call_interface_t::createTileIterator(
  const std::vector<Descriptor::tile_t *> & tiles,
  Utils::symbol_map_t & symbol_map,
  SgBasicBlock * bb,
  Descriptor::target_kind_e target
) const {
  std::vector<Descriptor::tile_t *>::const_iterator it;
  for (it = tiles.begin(); it != tiles.end(); it++) {
    std::ostringstream oss; oss << "t_" << (*it)->id;
    SgVariableSymbol * iter_sym = MFB::Utils::getExistingSymbolOrBuildDecl(oss.str(), SageBuilder::buildIntType(), bb);
    symbol_map.iter_tiles.insert(std::pair<size_t, SgVariableSymbol *>((*it)->id, iter_sym));
    if ((*it)->kind > Descriptor::e_last_loop_tile) {
      SgExpression * tile_idx = NULL;
      switch ((*it)->kind) {
        case Descriptor::e_thread_tile:
          tile_idx = kernel_api->buildThreadTileIdx();
          break;
        case Descriptor::e_gang_tile:
          tile_idx = kernel_api->buildGangTileIdx((*it)->param, target);
          break;
        case Descriptor::e_worker_tile:
          tile_idx = kernel_api->buildWorkerTileIdx((*it)->param, target);
          break;
        default:
          assert(false);
      }
      assert(tile_idx != NULL);
      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildVarRefExp(iter_sym),
        SageBuilder::buildMultiplyOp(tile_idx, kernel_api->buildGetTileStride((*it)->id, symbol_map.loop_context))
      ), bb);
    }
  }
}

SgBasicBlock * call_interface_t::generateKernelBody(Descriptor::kernel_t & kernel, SgFunctionDefinition * kernel_defn, Utils::symbol_map_t & symbol_map) const {
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();
  kernel_defn->set_body(bb);
  bb->set_parent(kernel_defn);

  if (kernel.target == Descriptor::e_target_threads) {
    kernel_api->tid_symbol = kernel_defn->lookup_variable_symbol("tid");
    assert(kernel_api->tid_symbol != NULL);
  }

  getContextSymbol(kernel_defn, symbol_map);

  getSymbolForParameter(kernel_defn, kernel.parameters, symbol_map, bb, kernel.target);

  getSymbolForData(kernel_defn, kernel.data, symbol_map, bb, kernel.target);

  createLoopIterator(kernel.loops, symbol_map, bb);

  createTileIterator(kernel.tiles, symbol_map, bb, kernel.target);

#if VERBOSE
  std::cerr << "[Info] (KLT::call_interface_t::generateKernelBody) Found " << kernel.loops.size() << " loops and " << kernel.tiles.size() << " tiles." << std::endl;
#endif

  return bb;
}

void call_interface_t::getSymbolForParameter(
  SgFunctionDefinition * kernel_defn,
  const std::vector<SgVariableSymbol *> & parameters,
  Utils::symbol_map_t & symbol_map,
  SgBasicBlock * bb,
  Descriptor::target_kind_e target
) const {
  switch (target) {
    case Descriptor::e_target_host:
    case Descriptor::e_target_threads:
    {
      SgVariableSymbol * arg_param_sym = kernel_defn->lookup_variable_symbol("param");
      assert(arg_param_sym != NULL);
      int cnt = 0;

      std::vector<SgVariableSymbol *>::const_iterator it;
      for (it = parameters.begin(); it != parameters.end(); it++) {
        SgVariableSymbol * param_sym = *it;
        std::string param_name = param_sym->get_name().getString();
        SgType * param_type = param_sym->get_type();

        driver.useType(param_type, kernel_defn);

        SgExpression * init = SageBuilder::buildPointerDerefExp(SageBuilder::buildCastExp(
                                  SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_param_sym), SageBuilder::buildIntVal(cnt++)),
                                  SageBuilder::buildPointerType(param_type)
                              ));
        SageInterface::prependStatement(SageBuilder::buildVariableDeclaration(param_name, param_type, SageBuilder::buildAssignInitializer(init), bb), bb);

        SgVariableSymbol * new_sym = bb->lookup_variable_symbol(param_name);
        assert(new_sym != NULL);

        symbol_map.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, new_sym));
      }
      break;
    }
    case Descriptor::e_target_opencl:
    case Descriptor::e_target_cuda:
    {
      std::vector<SgVariableSymbol *>::const_iterator it;
      for (it = parameters.begin(); it != parameters.end(); it++) {
        SgVariableSymbol * param_sym = *it;
        std::string param_name = param_sym->get_name().getString();
        SgType * param_type = param_sym->get_type();

        driver.useType(param_type, kernel_defn);

        SgVariableSymbol * new_sym = kernel_defn->lookup_variable_symbol(param_name);
        assert(new_sym != NULL);

        symbol_map.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, new_sym));
      }
      break;
    }
    default:
      assert(false);
  }
}

void call_interface_t::getSymbolForData(
  SgFunctionDefinition * kernel_defn,
  const std::vector<Descriptor::data_t *> & data,
  Utils::symbol_map_t & symbol_map,
  SgBasicBlock * bb,
  Descriptor::target_kind_e target
) const {
  switch (target) {
    case Descriptor::e_target_host:
    case Descriptor::e_target_threads:
    {
      SgVariableSymbol * arg_data_sym = kernel_defn->lookup_variable_symbol("data");
      assert(arg_data_sym != NULL);
      int cnt = 0;

      std::vector<Descriptor::data_t *>::const_iterator it;
      for (it = data.begin(); it != data.end(); it++) {
        SgVariableSymbol * data_sym = (*it)->symbol;
        std::string data_name = data_sym->get_name().getString();
        SgType * data_type = (*it)->base_type;

        driver.useType(data_type, kernel_defn);

        data_type = SageBuilder::buildPointerType(data_type);

        SgExpression * init = SageBuilder::buildCastExp(
                                SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_data_sym), SageBuilder::buildIntVal(cnt++)), data_type
                              );
        SageInterface::prependStatement(SageBuilder::buildVariableDeclaration(data_name, data_type, SageBuilder::buildAssignInitializer(init), bb), bb);

        SgVariableSymbol * new_sym = bb->lookup_variable_symbol(data_name);
        assert(new_sym != NULL);

        symbol_map.data.insert(std::pair<SgVariableSymbol *, Descriptor::data_t *>(data_sym, *it));
        symbol_map.data_trans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, new_sym));
        symbol_map.data_rtrans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(new_sym, data_sym));
      }
      break;
    }
    case Descriptor::e_target_opencl:
    case Descriptor::e_target_cuda:
    {
      std::vector<Descriptor::data_t *>::const_iterator it;
      for (it = data.begin(); it != data.end(); it++) {
        SgVariableSymbol * data_sym = (*it)->symbol;
        std::string data_name = data_sym->get_name().getString();
        SgType * data_type = (*it)->base_type;

        driver.useType(data_type, kernel_defn);

        SgVariableSymbol * new_sym = kernel_defn->lookup_variable_symbol(data_name);
        assert(new_sym != NULL);

        symbol_map.data.insert(std::pair<SgVariableSymbol *, Descriptor::data_t *>(data_sym, *it));
        symbol_map.data_trans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, new_sym));
        symbol_map.data_rtrans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(new_sym, data_sym));
      }
      break;
    }
    default:
      assert(false);
  }
}

} // namespace KLT::API

} // namespace KLT

