
#ifndef __KLT_RUNTIME_HPP__
#define __KLT_RUNTIME_HPP__

#include <cstddef>
#include <string>
#include <vector>

#include "KLT/Core/looptree.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/api.hpp"

#include "MFB/KLT/driver.hpp"

#include "MDCG/Tools/model-builder.hpp"

#include "KLT/MDCG/static-initializer.hpp"

class SgFunctionDeclaration;
class SgFunctionParameterList;
class SgBasicBlock;
class SgVariableSymbol;

namespace MFB {
  namespace KLT {
    template <class Object> class KLT;
  }
}
namespace MDCG {
  namespace Tools {
    class ModelBuilder;
    class StaticInitializer;
  }
}
namespace KLT {
  namespace Descriptor {
    struct kernel_t;
    struct loop_t;
    struct tile_t;
    struct data_t;
  }
  namespace API {
    struct kernel_t;
    struct host_t;
    struct call_interface_t;
  }
  namespace Kernel {
    struct kernel_t;
  }
  namespace Utils {
    struct symbol_map_t;
  }
}
namespace KLT {

class Generator {
  protected:
    MFB::Driver<MFB::KLT::KLT> & driver;
    ::MDCG::Tools::ModelBuilder & model_builder;

    size_t klt_model;

    std::string static_filename;
    MFB::file_id_t static_file_id;

    std::string host_kernel_filename;
    MFB::file_id_t host_kernel_file_id;

    bool target_threads;
    std::string threads_kernel_filename;
    MFB::file_id_t threads_kernel_file_id;

    bool target_opencl;
    std::string opencl_kernel_filename;
    MFB::file_id_t opencl_kernel_file_id;

    bool target_cuda;
    std::string cuda_kernel_filename;
    MFB::file_id_t cuda_kernel_file_id;

    API::host_t host_api;

    API::kernel_t kernel_api;
    API::call_interface_t call_interface;

    std::map<Kernel::kernel_t *, size_t> kernel_map;
    std::map<size_t, Kernel::kernel_t *> kernel_rmap;

  protected:
    void loadModel(const std::string & klt_inc_dir);

  public:
    Generator(
      MFB::Driver<MFB::KLT::KLT> & driver_,
      ::MDCG::Tools::ModelBuilder & model_builder_,
      const std::string & klt_inc_dir,
      const std::string & basename,
      bool target_threads,
      bool target_opencl,
      bool target_cuda
    );

    MFB::Driver<MFB::KLT::KLT> & getDriver();
    const MFB::Driver<MFB::KLT::KLT> & getDriver() const;

    ::MDCG::Tools::ModelBuilder & getModelBuilder();
    const ::MDCG::Tools::ModelBuilder & getModelBuilder() const;

    const API::host_t & getHostAPI() const;
    const std::string & getStaticFileName() const;
    MFB::file_id_t getStaticFileID() const;

    const API::kernel_t & getKernelAPI() const;
    API::call_interface_t & getCallInterface();
    const API::call_interface_t & getCallInterface() const;
    const std::string & getKernelFileName(Descriptor::target_kind_e target) const;
    MFB::file_id_t getKernelFileID(Descriptor::target_kind_e target) const;

  public:
    size_t getKernelID(Kernel::kernel_t * kernel);
    size_t getKernelID(Kernel::kernel_t * kernel) const;

    Kernel::kernel_t * getKernelByID(size_t id) const;

    template <class language_tpl>
    SgBasicBlock * instanciateOnHost(typename language_tpl::directive_t * directive, Kernel::kernel_t * original, const std::vector<Descriptor::loop_t *> & loops) const;

  public:
    template <class language_tpl>
    void addToStaticData(const std::map<typename language_tpl::directive_t *, Utils::subkernel_result_t<language_tpl> > & kernel_directive_translation_map) const;

  public:
    template <class language_tpl>
    static bool createTiles(LoopTree::loop_t * loop, const std::map<size_t, typename language_tpl::tile_parameter_t *> & tiling, LoopTree::tile_t * & first, LoopTree::tile_t * & last, size_t & tile_cnt);

  public:
    SgBasicBlock * insertDataEnvironment(SgScopeStatement * region, const std::vector<std::pair<Descriptor::data_t *, SgExpression *> > & datas) const;

  public:
    virtual void solveDataFlow(
      Kernel::kernel_t * kernel,
      const std::vector<Kernel::kernel_t *> & subkernels,
      Utils::kernel_deps_map_t & kernel_deps_map,
      const std::map<Kernel::kernel_t *, Descriptor::kernel_t *> & translation_map,
      const std::map<Descriptor::kernel_t *, Kernel::kernel_t *> & rtranslation_map
    ) const;
};

template <class language_tpl>
SgBasicBlock * Generator::instanciateOnHost(typename language_tpl::directive_t * directive, Kernel::kernel_t * original, const std::vector<Descriptor::loop_t *> & loops) const {
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();

  SgVariableSymbol * kernel_sym = host_api.insertKernelInstance("kernel", getKernelID(original), bb);

  size_t param_cnt = 0;
  std::vector<SgVariableSymbol *>::const_iterator it_param;
  for (it_param = original->parameters.begin(); it_param != original->parameters.end(); it_param++) {
    SgExpression * ref = SageBuilder::buildVarRefExp(*it_param);
    SageInterface::appendStatement(host_api.buildParamAssign(kernel_sym, param_cnt++, SageBuilder::buildAddressOfOp(ref)), bb);
  }

  size_t data_cnt = 0;
  std::vector<Descriptor::data_t *>::const_iterator it_data;
  for (it_data = original->data.begin(); it_data != original->data.end(); it_data++) {
    SgExpression * ref = SageBuilder::buildVarRefExp((*it_data)->symbol);
    size_t dim_cnt = 0;
    while (dim_cnt < (*it_data)->sections.size()) {
      ref = SageBuilder::buildPntrArrRefExp(ref, SageBuilder::buildIntVal(0));
      dim_cnt++;
    }
    SageInterface::appendStatement(host_api.buildDataPtrAssign(kernel_sym, data_cnt, SageBuilder::buildAddressOfOp(ref)), bb);
    dim_cnt = 0;
    std::vector<Descriptor::section_t *>::const_iterator it_section;
    for (it_section = (*it_data)->sections.begin(); it_section != (*it_data)->sections.end(); it_section++) {
      SageInterface::appendStatement(host_api.buildDataSectionOffsetAssign(kernel_sym, data_cnt, dim_cnt, (*it_section)->offset), bb);
      SageInterface::appendStatement(host_api.buildDataSectionLengthAssign(kernel_sym, data_cnt, dim_cnt, (*it_section)->length), bb);
      dim_cnt++;
    }
    SageInterface::appendStatement(host_api.buildDataModeAssign(kernel_sym, data_cnt, SageBuilder::buildUnsignedLongVal((unsigned long)(*it_data)->mode)), bb);
    SageInterface::appendStatement(host_api.buildDataLivenessAssign(kernel_sym, data_cnt, SageBuilder::buildUnsignedLongVal((unsigned long)(*it_data)->liveness)), bb);
    data_cnt++;
  }

  size_t loop_cnt = 0;
  std::vector<Descriptor::loop_t *>::const_iterator it_loop;
  for (it_loop = loops.begin(); it_loop != loops.end(); it_loop++) {
    SageInterface::appendStatement(host_api.buildLoopLowerAssign(kernel_sym, loop_cnt, (*it_loop)->lb), bb);
    SageInterface::appendStatement(host_api.buildLoopUpperAssign(kernel_sym, loop_cnt, (*it_loop)->ub), bb);
    SageInterface::appendStatement(host_api.buildLoopStrideAssign(kernel_sym, loop_cnt, (*it_loop)->stride), bb);
    loop_cnt++;
  }

  if (original->target == Descriptor::e_target_host) {
    assert(original->num_threads    == NULL);
    assert(original->num_gangs[0]   == NULL);
    assert(original->num_workers[0] == NULL);
    assert(original->num_gangs[1]   == NULL);
    assert(original->num_workers[1] == NULL);
    assert(original->num_gangs[2]   == NULL);
    assert(original->num_workers[2] == NULL);
  }
  else if (original->target == Descriptor::e_target_threads) {
    assert(original->num_threads    != NULL);
    assert(original->num_gangs[0]   == NULL);
    assert(original->num_workers[0] == NULL);
    assert(original->num_gangs[1]   == NULL);
    assert(original->num_workers[1] == NULL);
    assert(original->num_gangs[2]   == NULL);
    assert(original->num_workers[2] == NULL);

    SageInterface::appendStatement(host_api.buildNumThreadsAssign(kernel_sym, original->num_threads), bb);
  }
  else if (original->target == Descriptor::e_target_opencl || original->target == Descriptor::e_target_cuda) {
    assert(original->num_threads    == NULL);
    assert(original->num_gangs[0]   != NULL);
    assert(original->num_workers[0] != NULL);

    for (size_t i = 0; i < 3; i++) {
      if (original->num_gangs[i] != NULL)
        SageInterface::appendStatement(host_api.buildNumGangsAssign(kernel_sym, i, original->num_gangs[i]), bb);
      else
        SageInterface::appendStatement(host_api.buildNumGangsAssign(kernel_sym, i, SageBuilder::buildIntVal(1)), bb);

      if (original->num_workers[i] != NULL)
        SageInterface::appendStatement(host_api.buildNumWorkersAssign(kernel_sym, i, original->num_workers[i]), bb);
      else
        SageInterface::appendStatement(host_api.buildNumWorkersAssign(kernel_sym, i, SageBuilder::buildIntVal(1)), bb);
    }
  }
  else {
    assert(original->target == Descriptor::e_target_unknown);
    assert(false);
  }

  SageInterface::appendStatement(host_api.buildDeviceIdAssign(kernel_sym, original->device_id), bb);

  host_api.insertKernelExecute(kernel_sym, bb);

  return bb;
}

template <class language_tpl>
void Generator::addToStaticData(const std::map<typename language_tpl::directive_t *, Utils::subkernel_result_t<language_tpl> > & kernel_directive_translation_map) const {
  ::MDCG::Model::class_t kernel_desc_class = model_builder.get(klt_model).lookup< ::MDCG::Model::class_t>("klt_kernel_desc_t");
  ::MDCG::Tools::StaticInitializer::addArrayDeclaration< ::KLT::MDCG::KernelContainer<language_tpl> >(
      driver, kernel_desc_class, kernel_directive_translation_map.size(),
      kernel_directive_translation_map.begin(), kernel_directive_translation_map.end(),
      NULL, static_file_id, false, std::string("klt_kernel_desc")
  );
}

template <class language_tpl>
bool Generator::createTiles(
  LoopTree::loop_t * loop, const std::map<size_t, typename language_tpl::tile_parameter_t *> & tiling, LoopTree::tile_t * & first, LoopTree::tile_t * & last, size_t & tile_cnt
) {
  typename std::map<size_t, typename language_tpl::tile_parameter_t *>::const_iterator it_tile;
  for (it_tile = tiling.begin(); it_tile != tiling.end(); it_tile++) {
    LoopTree::tile_t * current = new LoopTree::tile_t(tile_cnt++, it_tile->second->kind, it_tile->second->order, it_tile->second->param, loop, it_tile->first);
      current->parent = last;

    if (last != NULL) {
      last->next_tile = current;
      last->next_node = NULL;
    }
    else first = current;

    last = current;
  }
  return true;
}

} // namespace KLT

#endif /* __KLT_RUNTIME_HPP__ */

