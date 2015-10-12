
#include "sage3basic.h"

#include "KLT/MDCG/static-initializer.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/variable-declaration.hpp"

namespace KLT {

namespace MDCG {

// I hate using globals but I cannot see another way here
std::map<  SgVariableSymbol *, size_t> param_ids_map; // Filled by: registerParamAndDataIds, Used by: createParamIds
std::map<Descriptor::data_t *, size_t> data_ids_map;  // Filled by: registerParamAndDataIds, Used by: createDataIds

SgType * size_t_type = NULL;
SgType * get_size_t_type(MFB::Driver<MFB::Sage> & driver) {
  if (size_t_type == NULL) {
    SgGlobal * global_scope_across_files = driver.project->get_globalScopeAcrossFiles();
    assert(global_scope_across_files != NULL);
    SgTypedefSymbol * size_t_symbol = SageInterface::lookupTypedefSymbolInParentScopes("size_t", global_scope_across_files);
    assert(size_t_symbol != NULL);
    size_t_type = isSgType(size_t_symbol->get_type());
    assert(size_t_type != NULL);
  }
  return size_t_type;
}

void registerParamAndDataIds(const Kernel::kernel_t & original) {
  std::vector<SgVariableSymbol *>::const_iterator it_vsym;
  for (it_vsym = original.parameters.begin(); it_vsym != original.parameters.end(); it_vsym++)
    param_ids_map.insert(std::pair<SgVariableSymbol *, size_t>(*it_vsym, param_ids_map.size()));
  std::vector<Descriptor::data_t *>::const_iterator it_data;
  for (it_data = original.data.begin(); it_data != original.data.end(); it_data++)
    data_ids_map.insert(std::pair<Descriptor::data_t *, size_t>(*it_data, data_ids_map.size()));
}

void clearParamAndDataIds() {
  param_ids_map.clear();
  data_ids_map.clear();
}

std::pair<SgVarRefExp *, SgExprListExp *> createScalarArray(
  SgType * scalar_type, MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, size_t size
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);
  SgType * type = SageBuilder::buildArrayType(scalar_type, SageBuilder::buildUnsignedLongVal(size));
  SgVarRefExp * var_ref = SageBuilder::buildVarRefExp(::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, decl_name, scope, file_id, type, init, prepend));
  return std::pair<SgVarRefExp *, SgExprListExp *>(var_ref, expr_list);
}

SgExpression * createParamSizeOf(
  MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<SgVariableSymbol *> & parameters
) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createScalarArray(get_size_t_type(driver), driver, decl_name, scope, file_id, prepend, parameters.size());

  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++) {
    driver.useType((*it)->get_type(), file_id);
    res.second->append_expression(SageBuilder::buildSizeOfOp((*it)->get_type()));
  }

  return res.first;
}

SgExpression * createDataSizeOf(
  MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::data_t *> & data
) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createScalarArray(get_size_t_type(driver), driver, decl_name, scope, file_id, prepend, data.size());

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++) {
    driver.useType((*it)->base_type, file_id);
    res.second->append_expression(SageBuilder::buildSizeOfOp((*it)->base_type));
  }

  return res.first;
}

SgExpression * createDataNDims(
  MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::data_t *> & data
) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createScalarArray(get_size_t_type(driver), driver, decl_name, scope, file_id, prepend, data.size());

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++)
    res.second->append_expression(SageBuilder::buildIntVal((*it)->sections.size()));

  return res.first;
}

SgExpression * createParamIds(
  MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<SgVariableSymbol *> & parameters
) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createScalarArray(SageBuilder::buildIntType(), driver, decl_name, scope, file_id, prepend, parameters.size());

  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++) {
    std::map<SgVariableSymbol *, size_t>::const_iterator it_param = param_ids_map.find(*it);
    assert(it_param != param_ids_map.end());
    res.second->append_expression(SageBuilder::buildIntVal(it_param->second));
  }

  return res.first;
}

SgExpression * createDataIds(
  MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::data_t *> & data
) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createScalarArray(SageBuilder::buildIntType(), driver, decl_name, scope, file_id, prepend, data.size());

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++) {
    std::map<Descriptor::data_t *, size_t>::const_iterator it_data = data_ids_map.find(*it);
    assert(it_data != data_ids_map.end());
    res.second->append_expression(SageBuilder::buildIntVal(it_data->second));
  }

  return res.first;
}

SgExpression * createLoopIds(
  MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::loop_t *> & loops
) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createScalarArray(SageBuilder::buildIntType(), driver, decl_name, scope, file_id, prepend, loops.size());

  std::vector<Descriptor::loop_t *>::const_iterator it;
  for (it = loops.begin(); it != loops.end(); it++)
    res.second->append_expression(SageBuilder::buildIntVal((*it)->id));

  return res.first;
}

SgExpression * createDepsIds(
  MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::kernel_t *> & deps
) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createScalarArray(SageBuilder::buildIntType(), driver, decl_name, scope, file_id, prepend, deps.size());

  std::vector<Descriptor::kernel_t *>::const_iterator it;
  for (it = deps.begin(); it != deps.end(); it++)
    res.second->append_expression(SageBuilder::buildIntVal((*it)->id));

  return res.first;
}

size_t DataContainer::cnt[3] = {0,0,0};

SgExpression * DataContainer::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  SgScopeStatement * scope,
  size_t file_id,
  bool prepend
) {
  switch (field_id) {
    case 0:
    { // size_t num_param;
      return SageBuilder::buildIntVal(input.parameters.size());
    }
    case 1:
    { // size_t * sizeof_param;
      std::ostringstream decl_name; decl_name << "sizeof_param_" << cnt[0]++;
      return createParamSizeOf(driver, decl_name.str(), scope, file_id, prepend, input.parameters);
    }
    case 2:
    { // size_t num_data;
      return SageBuilder::buildIntVal(input.data.size());
    }
    case 3:
    { // size_t * sizeof_data;
      std::ostringstream decl_name; decl_name << "sizeof_data_" << cnt[1]++;
      return createDataSizeOf(driver, decl_name.str(), scope, file_id, prepend, input.data);
    }
    case 4:
    { // size_t * ndims_data;
      std::ostringstream decl_name; decl_name << "ndims_data_" << cnt[2]++;
      return createDataNDims(driver, decl_name.str(), scope, file_id, prepend, input.data);
    }
    default:
      assert(false);
  }
}

SgExpression * TileDesc::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  SgScopeStatement * scope,
  size_t file_id,
  bool prepend
) {
  switch (field_id) {
    case 0:
    { // int idx;
      return SageBuilder::buildIntVal(input->id);
    }
    case 1:
    { // enum tile_kind_e kind;
      return SageBuilder::buildIntVal(input->kind);
    }
    case 2:
    { // int param;
      if (input->param != NULL)
        return SageInterface::copyExpression(input->param);
      else
        return SageBuilder::buildIntVal(0);
    }
    default:
      assert(false);
  }
}

size_t LoopDesc::cnt[1] = {0};

SgExpression * LoopDesc::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  SgScopeStatement * scope,
  size_t file_id,
  bool prepend
) {
  switch (field_id) {
    case 0:
    { // int idx;
      return SageBuilder::buildIntVal(input->id);
    }
    case 1:
    { // int num_tiles;
      return SageBuilder::buildIntVal(input->tiles.size());
    }
    case 2:
    { // struct klt_tile_desc_t * tile_desc;
      std::ostringstream decl_name; decl_name << "tile_desc_" << cnt[0]++;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("tile_desc", "klt_tile_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<TileDesc>(
                 driver, field_class, input->tiles.size(), input->tiles.begin(), input->tiles.end(), scope, file_id, prepend, decl_name.str()
             );
    }
    default:
      assert(false);
  }
}

size_t TopLoopContainer::cnt[1] = {0};

SgExpression * TopLoopContainer::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  SgScopeStatement * scope,
  size_t file_id,
  bool prepend
) {
  switch (field_id) {
    case 0:
    { // int num_loops;
      return SageBuilder::buildIntVal(input.size());
    }
    case 1:
    { // int num_tiles;
      return SageBuilder::buildIntVal(0); // klt_loop_container_t for the original loop-tree => no tile
    }
    case 2:
    { // struct klt_loop_desc_t * loop_desc;
      std::ostringstream decl_name; decl_name << "top_loop_desc_" << cnt[0]++;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("loop_desc", "klt_loop_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<LoopDesc>(
                 driver, field_class, input.size(), input.begin(), input.end(), scope, file_id, prepend, decl_name.str()
             );
    }
    default:
      assert(false);
  }
}

size_t LoopContainer::cnt[1] = {0};

SgExpression * LoopContainer::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  SgScopeStatement * scope,
  size_t file_id,
  bool prepend
) {
  switch (field_id) {
    case 0:
    { // int num_loops;
      return SageBuilder::buildIntVal(input.loops.size());
    }
    case 1:
    { // int num_tiles;
      return SageBuilder::buildIntVal(input.tiles.size());
    }
    case 2:
    { // struct klt_loop_desc_t * loop_desc;
      std::ostringstream decl_name; decl_name << "loop_desc_" << cnt[0]++;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("loop_desc", "klt_loop_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<LoopDesc>(
                 driver, field_class, input.loops.size(), input.loops.begin(), input.loops.end(), scope, file_id, prepend, decl_name.str()
             );
    }
    default:
     assert(false);
  }
}

SgExpression * SubKernelDesc::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  SgScopeStatement * scope,
  size_t file_id,
  bool prepend
) {
  switch (field_id) {
    case 0:
    { // size_t id;
      return SageBuilder::buildIntVal(input.first->id);
    }
    case 1:
    { // enum klt_device_e device_kind;
      assert(input.first->target != ::KLT::Descriptor::e_target_unknown);
      return SageBuilder::buildIntVal(input.first->target);
    }
    case 2:
    { // struct klt_loop_container_t loop;
      ::MDCG::Model::class_t field_class = element->node->getBaseClass("loop", "klt_loop_container_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createInitializer<LoopContainer>(driver, field_class, *input.first, scope, file_id, prepend);
    }
    case 3:
    { // int num_params;
      return SageBuilder::buildIntVal(input.first->parameters.size());
    }
    case 4:
    { // int * param_ids;
      std::ostringstream decl_name; decl_name << "ids_param_" << input.first;
      return createParamIds(driver, decl_name.str(), scope, file_id, prepend, input.first->parameters);
    }
    case 5:
    { // int num_data;
      return SageBuilder::buildIntVal(input.first->data.size());
    }
    case 6:
    { // int * data_ids;
      std::ostringstream decl_name; decl_name << "ids_data_" << input.first;
      return createDataIds(driver, decl_name.str(), scope, file_id, prepend, input.first->data);
    }
    case 7:
    { // int num_loops;
      return SageBuilder::buildIntVal(input.first->loops.size());
    }
    case 8:
    { // int * loop_ids;
      std::ostringstream decl_name; decl_name << "ids_loop_" << input.first;
      return createLoopIds(driver, decl_name.str(), scope, file_id, prepend, input.first->loops);
    }
    case 9:
    { // int num_deps;
      return SageBuilder::buildIntVal(input.second.size());
    }
    case 10:
    { // int * deps_ids;
      std::ostringstream decl_name; decl_name << "ids_deps_" << input.first;
      return createDepsIds(driver, decl_name.str(), scope, file_id, prepend, input.second);
    }
    case 11:
    { // void * descriptor;
      assert(element->node->type != NULL);
      assert(element->node->type->node->kind == ::MDCG::Model::node_t< ::MDCG::Model::e_model_type>::e_class_type);
      assert(element->node->type->node->base_class != NULL);
      assert(element->node->type->node->base_class->scope->field_children.size() > 0);

      switch (input.first->target) {
        case ::KLT::Descriptor::e_target_host:
        case ::KLT::Descriptor::e_target_threads:
        {
          assert(element->node->type->node->base_class->scope->field_children[0] != NULL);
          assert(element->node->type->node->base_class->scope->field_children[0]->node->type != NULL);
          assert(element->node->type->node->base_class->scope->field_children[0]->node->type->node->type != NULL);
          SgType * type = element->node->type->node->base_class->scope->field_children[0]->node->type->node->type;

          SgVariableSymbol * symbol = ::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, input.first->kernel_name, scope, file_id, type, NULL, prepend);
          assert(symbol != NULL);

          SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(symbol->get_declaration()->get_parent());
            decl_stmt->get_declarationModifier().unsetDefault();
            decl_stmt->get_declarationModifier().get_storageModifier().setExtern();

          return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(symbol));
        }
        case ::KLT::Descriptor::e_target_opencl:
        case ::KLT::Descriptor::e_target_cuda:
        {
          return SageBuilder::buildStringVal(input.first->kernel_name);
        }
        default:
          assert(false);
      }
    }
    default:
      assert(false);
  }
}

SgExpression * DataSectionDesc::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  SgScopeStatement * scope,
  size_t file_id,
  bool prepend
) {
  switch (field_id) {
    case 0:
    { // int offset;
      return SageInterface::copyExpression(input->offset);
    }
    case 1:
    { // int length;
      return SageInterface::copyExpression(input->length);
    }
    default:
      assert(false);
  }
}

size_t DataDesc::cnt[1] = {0};

SgExpression * DataDesc::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  SgScopeStatement * scope,
  size_t file_id,
  bool prepend
) {
  switch (field_id) {
    case 0:
    { // void * ptr;
      SgExpression * expr = SageBuilder::buildVarRefExp(input.first->symbol);
      std::vector<Descriptor::section_t *>::const_iterator it;
      for (it = input.first->sections.begin(); it != input.first->sections.end(); it++)
        expr = SageBuilder::buildPntrArrRefExp(expr, SageBuilder::buildIntVal(0));
      return SageBuilder::buildAddressOfOp(expr);
    }
    case 1:
    { // size_t base_type_size;
      return SageBuilder::buildSizeOfOp(input.first->base_type);
    }
    case 2:
    { // size_t num_sections;
      return SageBuilder::buildIntVal(input.first->sections.size());
    }
    case 3:
    { // struct klt_data_section_t * sections;
      std::ostringstream decl_name; decl_name << "sections_" << cnt[0]++;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("sections", "klt_data_section_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<DataSectionDesc>(
                 driver, field_class, input.first->sections.size(), input.first->sections.begin(), input.first->sections.end(), scope, file_id, prepend, decl_name.str()
             );
    }
    case 4:
    { // enum klt_memory_mode_e mode;
      return SageBuilder::buildUnsignedLongVal((unsigned long)input.first->mode);
    }
    case 5:
    { // enum klt_liveness_e liveness;
      return SageBuilder::buildUnsignedLongVal((unsigned long)input.first->liveness);
    }
    default:
      assert(false);
  }
}

} // namespace KLT::MDCG

} // namespace KLT

