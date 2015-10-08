
#ifndef __KLT_MDCG_STATIC_INITIALIZER_HPP__
#define __KLT_MDCG_STATIC_INITIALIZER_HPP__

#include "MDCG/Tools/static-initializer.hpp"

#include "KLT/Core/kernel.hpp"
#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/utils.hpp"

namespace KLT {

namespace MDCG {

struct DataContainer {
  typedef Kernel::kernel_t input_t;

/* IN:
      struct kernel_t {
        node_t * root;
        vsym_list_t parameters;
        data_list_t data;
      };                                            */

/* OUT:
      struct klt_data_container_t {
        size_t num_param;
        size_t * sizeof_param;

        size_t num_data;
        size_t * sizeof_data;
        size_t * ndims_data;
      };                            */

  static size_t cnt[3];
  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

struct TileDesc {
  typedef Descriptor::tile_t * input_t;

/* IN: 
      struct tile_t {
        size_t id;
        tile_kind_e kind;
        size_t order;
        SgExpression * param;
      };                             */

/* OUT:
      struct klt_tile_desc_t {
        int idx;
        enum tile_kind_e kind;
        int param;
      };                                    */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

struct LoopDesc {
  typedef Descriptor::loop_t * input_t;

/* IN: 
      struct loop_t {
        size_t id;
        SgExpression * lb;
        SgExpression * ub;
        SgExpression * stride;
        std::vector<tile_t *> tiles;
        SgVariableSymbol * iterator;
      };                             */

/* OUT:
      struct klt_loop_desc_t {
        int idx;
        int num_tiles;
        struct klt_tile_desc_t * tile_desc;
      };                                    */

  static size_t cnt[1];
  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

struct TopLoopContainer {
  typedef std::vector<Descriptor::loop_t *> input_t;

/* IN: std::vector<Descriptor::loop_t *> */

/* OUT:
      struct klt_loop_container_t {
        int num_loops;
        int num_tiles;
        struct klt_loop_desc_t * loop_desc;
      };                                    */

  static size_t cnt[1];
  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

struct LoopContainer {
  typedef Descriptor::kernel_t input_t;

/* IN:
      struct kernel_t {
        size_t id;
        std::string kernel_name;
        std::vector<loop_t *> loops;
        std::vector<tile_t *> tiles;
        std::vector<SgVariableSymbol *> parameters;
        std::vector<data_t *> data;
      };                                            */

/* OUT:
      struct klt_loop_container_t {
        int num_loops;
        int num_tiles;
        struct klt_loop_desc_t * loop_desc;
      };                                    */

  static size_t cnt[1];
  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

std::pair<SgVarRefExp *, SgExprListExp *> createScalarArray(SgType * scalar_type, MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, size_t size);

SgExpression * createParamSizeOf (MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<SgVariableSymbol *> & parameters);
SgExpression * createDataSizeOf  (MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::data_t *> & data);
SgExpression * createDataNDims   (MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::data_t *> & data);
SgExpression * createParamIds    (MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<SgVariableSymbol *> & parameters);
SgExpression * createDataIds     (MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::data_t *> & data);
SgExpression * createLoopIds     (MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::loop_t *> & loops);
SgExpression * createDepsIds     (MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, SgScopeStatement * scope, size_t file_id, bool prepend, const std::vector<Descriptor::kernel_t *> & deps);

struct SubkernelConfig {
  typedef Descriptor::kernel_t input_t;

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

struct SubKernelDesc {
  typedef std::pair<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> > input_t;

/* IN: 
       input.first  = Descriptor::kernel_t *
       input.second = std::vector<Descriptor::kernel_t *>      */

/* OUT:
      struct klt_subkernel_desc_t {
        struct klt_loop_container_t loop;
        int num_params;
        int * param_ids;
        int num_data;
        int * data_ids;
        int num_loops;
        int * loop_ids;
        int num_deps;
        int * deps_ids;
        struct klt_subkernel_config_t * config;
      };                                                  */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

template <class language_tpl>
struct VersionSelector {
  typedef Utils::tiling_info_t<language_tpl> input_t;

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  ) {
    assert(false); // TileK does not support version selection so 'klt_version_selector_t' is an empty structure => this should not be called
    return NULL;
  }
};

template <class language_tpl>
struct VersionDesc {
  typedef std::pair<Utils::tiling_info_t<language_tpl> *, Utils::kernel_deps_map_t> input_t;

/* IN: 
       input.first  = Utils::tiling_info_t<language_tpl> * = [ LANGUAGE DEPENDENT ]
       input.second = std::map<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> >      */

/* OUT:
      struct klt_version_desc_t {
        int num_subkernels;
        struct klt_subkernel_desc_t * subkernels;
        struct klt_version_selector_t * version_selector;
      };                                                  */

  static SgExpression * createFieldInitializer(
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
      { // enum klt_device_e device_kind;
        assert(input.first->target != ::KLT::Descriptor::e_target_unknown);
        return SageBuilder::buildIntVal(input.first->target);
      }
      case 1:
      { // struct klt_version_selector_t * version_selector;
        std::ostringstream decl_name; decl_name << "selector_" << input.first;
        ::MDCG::Model::class_t field_class = element->node->getBaseClass("version_selector", "klt_version_selector_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createInitializer<VersionSelector<language_tpl> >(driver, field_class, *input.first, scope, file_id, prepend);
      }
      case 2:
      { //  int num_subkernels;
        return SageBuilder::buildIntVal(input.second.size());
      }
      case 3:
      { // struct klt_subkernel_desc_t * subkernels;
        std::ostringstream decl_name; decl_name << "subkernels_" << input.first;
        ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("subkernels", "klt_subkernel_desc_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createArrayPointer<SubKernelDesc>(
                   driver, field_class, input.second.size(), input.second.begin(), input.second.end(), scope, file_id, prepend, decl_name.str()
               );
      }
      default:
        assert(false);
    }
  }
};

void registerParamAndDataIds(const Kernel::kernel_t & original);
void clearParamAndDataIds();

template <class language_tpl>
struct KernelContainer {
  typedef std::pair<typename language_tpl::directive_t *, Utils::subkernel_result_t<language_tpl> > input_t;

/* IN:
      struct subkernel_result_t {
        Kernel::kernel_t * original;
        std::vector<Descriptor::loop_t *> loops;
        std::map<tiling_info_t<language_tpl> *, kernel_deps_map_t> tiled;
      };                                                                  */

/* OUT:
      struct klt_kernel_desc_t {
        struct klt_data_container_t data;
        struct klt_loop_container_t loop;
        int num_versions;
        struct klt_version_desc_t * versions;
      };                                      */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  ) {
    const Utils::subkernel_result_t<language_tpl> & subkernel_result = input.second;
    switch (field_id) {
      case 0:
      { // struct klt_data_container_t data;
        ::MDCG::Model::class_t field_class = element->node->getBaseClass("data", "klt_data_container_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createInitializer<DataContainer>(driver, field_class, *(subkernel_result.original), scope, file_id, prepend);
      }
      case 1:
      { // struct klt_loop_container_t loop;
        ::MDCG::Model::class_t field_class = element->node->getBaseClass("loop", "klt_loop_container_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createInitializer<TopLoopContainer>(driver, field_class, subkernel_result.loops, scope, file_id, prepend);
      }
      case 2:
      { // int num_versions;
        return SageBuilder::buildIntVal(subkernel_result.tiled.size());
      }
      case 3:
      { // struct klt_version_desc_t * versions;
        registerParamAndDataIds(*(subkernel_result.original));
        std::ostringstream decl_name; decl_name << "versions_" << subkernel_result.original;
        ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("versions", "klt_version_desc_t"); assert(field_class != NULL);
        SgExpression * res = ::MDCG::Tools::StaticInitializer::createArrayPointer<VersionDesc<language_tpl> >(
                                 driver, field_class, subkernel_result.tiled.size(), subkernel_result.tiled.begin(), subkernel_result.tiled.end(), scope, file_id, prepend, decl_name.str()
                             );
        clearParamAndDataIds();
        return res;
      }
      default:
        assert(false);
    }
  }
};

struct DataSectionDesc {
  typedef Descriptor::section_t * input_t;

/* IN: 
      struct Descriptor::section_t {
        SgExpression * offset;
        SgExpression * length;
      };                             */

/* OUT:
      struct klt_data_section_t {
        int offset;
        int length;
      };                          */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

struct DataDesc {
  typedef std::pair<Descriptor::data_t *, SgExpression *> input_t;

/* IN: 
      struct Descriptor::data_t {
        SgVariableSymbol * symbol;
        SgType * base_type;
        std::vector<section_t *> sections;
        e_mode mode;
        e_liveness liveness;
      };                                   */

/* OUT:
      struct klt_data_t {
        void * ptr;

        size_t base_type_size;

        size_t num_sections;
        struct klt_data_section_t * sections;

        enum klt_memory_mode_e mode;
        enum klt_liveness_e liveness;
      };                                      */

  static size_t cnt[1];
  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    SgScopeStatement * scope,
    size_t file_id,
    bool prepend
  );
};

} // namespace KLT::MDCG

} // namespace KLT

#endif /* __KLT_MDCG_STATIC_INITIALIZER_HPP__ */

