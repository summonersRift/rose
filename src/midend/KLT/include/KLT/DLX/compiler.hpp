
#ifndef __KLT_DLX_COMPILER_HPP__
#define __KLT_DLX_COMPILER_HPP__

// DLX

#include "DLX/Core/parser.hpp"
#include "DLX/Core/compiler.hpp"

// MFB

#include "MFB/KLT/driver.hpp"

// MDCG

#include "MDCG/Tools/model-builder.hpp"

// KLT

#include "KLT/Core/generator.hpp"
#include "KLT/Core/looptree.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/api.hpp"
#include "KLT/Core/data.hpp"

#ifndef OUTPUT_SUBKERNELS_GRAPHVIZ
#define OUTPUT_SUBKERNELS_GRAPHVIZ 1
#endif
#if OUTPUT_SUBKERNELS_GRAPHVIZ
#include <iostream>
#include <fstream>
#include <sstream>
#endif

#include <map>

template <class A, class B, class C>
void maps_composition(const std::map<A, B> & map_A_B, const std::map<B, C> & map_B_C, std::map<A, C> & map_A_C) {
  typename std::map<A, B>::const_iterator it_A_B;
  for (it_A_B = map_A_B.begin(); it_A_B != map_A_B.end(); it_A_B++) {
    typename std::map<B, C>::const_iterator it_B_C = map_B_C.find(it_A_B->second);
    if (it_B_C != map_B_C.end())
      map_A_C.insert(std::pair<A, C>(it_A_B->first, it_B_C->second));
  }
}

namespace KLT {

namespace DLX {

template <class language_tpl>
class Compiler : public ::DLX::Compiler<language_tpl> {
  public:
    typedef std::map<SgForStatement *, size_t> loop_id_map_t;
    typedef std::pair<Kernel::kernel_t *, loop_id_map_t> extracted_kernel_t;

    typedef std::pair<SgScopeStatement *, std::vector<std::pair<Descriptor::data_t *, SgExpression *> > > extracted_dataenv_t;

    typedef ::DLX::data_sections_t data_sections_t;
    typedef KLT::Descriptor::data_t data_t;

    typedef typename language_tpl::directive_t directive_t;
    typedef typename language_tpl::construct_t construct_t;
    typedef typename language_tpl::dataenv_construct_t dataenv_construct_t;
    typedef typename language_tpl::kernel_construct_t kernel_construct_t;
    typedef typename language_tpl::loop_construct_t loop_construct_t;
    typedef typename language_tpl::clause_t clause_t;
    typedef typename language_tpl::data_clause_t data_clause_t;
    typedef typename language_tpl::tile_clause_t tile_clause_t;

  protected:
    MFB::Driver<MFB::KLT::KLT> driver;
    ::MDCG::Tools::ModelBuilder model_builder;

    std::string klt_rtl_path;

    KLT::Generator generator;

    ::KLT::Descriptor::target_kind_e threads_target;
    ::KLT::Descriptor::target_kind_e accelerator_target;

  public:
    Compiler(
      SgProject * project,
      const std::string & klt_rtl_path_,
      const std::string & basename,
      ::KLT::Descriptor::target_kind_e threads_target_,
      ::KLT::Descriptor::target_kind_e accelerator_target_
    ) :
      ::DLX::Compiler<language_tpl>(), driver(project), model_builder(driver), klt_rtl_path(klt_rtl_path_),
      generator(KLT::Generator(
        driver, model_builder, klt_rtl_path + "/include", basename,
        (threads_target_     == ::KLT::Descriptor::e_target_threads) ? true : false,
        (accelerator_target_ == ::KLT::Descriptor::e_target_opencl)  ? true : false,
        (accelerator_target_ == ::KLT::Descriptor::e_target_cuda)    ? true : false
      )),
      threads_target(threads_target_), accelerator_target(accelerator_target_)
    {
#if VERBOSE
      std::cerr << "[Info] Create KLT::DLX::Compiler with " << threads_target << " " << accelerator_target << std::endl;
#endif
    }
    
    MFB::Driver<MFB::KLT::KLT> & getDriver() { return driver; }
    const MFB::Driver<MFB::KLT::KLT> & getDriver() const { return driver; }

  protected: // Extract Data
    // It requires language_tpl to have KLT's data extension ('language_tpl::has_klt_data')
    Descriptor::data_t * dataFromSections(const data_sections_t & data_section) const;
  public:
    // It requires language_tpl to have KLT's data extension ('language_tpl::has_klt_data')
    void convertDataList (const std::vector<clause_t *> & clauses, std::vector<Descriptor::data_t *> & data) const;
    void convertAllocList(const std::vector<clause_t *> & clauses, std::vector<std::pair<Descriptor::data_t *, SgExpression *> > & data) const;

  public: // Extract KLT representation
    // It requires language_tpl to have KLT's kernel extension ('language_tpl::has_klt_kernel'), loop extension ('language_tpl::has_klt_loop'), and data extension ('language_tpl::has_klt_data')
    void extractDirectives(
      const std::vector<directive_t *> & directives,
      std::map<directive_t *, SgForStatement *> & loop_directive_map,
      std::map<directive_t *, extracted_kernel_t> & kernel_directives_map,
      std::map<directive_t *, extracted_dataenv_t > & dataenv_directives_map
    ) const;

  public: // Tiling
    //
    typedef typename language_tpl::tile_parameter_t tile_parameter_t;
    //
    typedef Utils::tiling_info_t<language_tpl> tiling_info_t;
  public:
    //
    LoopTree::node_t * applyTiling(LoopTree::node_t * node, const tiling_info_t & tiling_info, LoopTree::node_t * parent, size_t & tile_cnt) const;

  public: // Subkernels
    // 
    typedef std::map<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> > kernel_deps_map_t;
    //
    typedef Utils::subkernel_result_t<language_tpl> subkernel_result_t;
    //
    typedef std::map<directive_t *, subkernel_result_t> kernel_directive_translation_map_t;
  protected:
    // 
    void splitKernelRoot(Kernel::kernel_t * kernel, const tiling_info_t & tiling_info, std::vector<Kernel::kernel_t *> & kernels) const;
    // It requires language_tpl to have KLT's loop extension ('language_tpl::has_klt_loop') and tile extension ('language_tpl::has_klt_tile')
    void applyLoopTiling(
      Kernel::kernel_t * kernel,
      const std::map<directive_t *, size_t> & directive_loop_id_map,
      std::map<tiling_info_t *, std::vector<Kernel::kernel_t *> > & tiled_kernels
    ) const;
  public:
    // It requires language_tpl to have KLT's kernel extension ('language_tpl::has_klt_kernel'), loop extension ('language_tpl::has_klt_loop'), and tile extension ('language_tpl::has_klt_tile')
    void generateAllKernels(
      const std::map<directive_t *, SgForStatement *> & loop_directive_map,
      const std::map<directive_t *, extracted_kernel_t> & kernel_directives_map,
      kernel_directive_translation_map_t & kernel_directive_translation_map
    );

  public:
    //
    void compile(SgNode * node);
};

////////////////////////////////////////////////// Extract Data

template <class language_tpl>
KLT::Descriptor::data_t * Compiler<language_tpl>::dataFromSections(const data_sections_t & data_section) const {
  SgVariableSymbol * data_sym = data_section.first;

  SgType * base_type = data_sym->get_type();
  std::vector< ::DLX::section_t>::const_iterator it_section;
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++) {
         if (isSgPointerType(base_type)) base_type = ((SgPointerType *)base_type)->get_base_type();
    else if (isSgArrayType  (base_type)) base_type = ((SgArrayType   *)base_type)->get_base_type();
    else assert(false);
    assert(base_type != NULL);
  }
  KLT::Descriptor::data_t * data = new KLT::Descriptor::data_t(data_sym, base_type);
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++)
    data->sections.push_back(new KLT::Descriptor::section_t(it_section->lower_bound, it_section->size));

  return data;
}

template <class language_tpl>
void Compiler<language_tpl>::convertDataList(const std::vector<clause_t *> & clauses, std::vector<Descriptor::data_t *> & datas) const {
  typename std::vector<typename language_tpl::clause_t *>::const_iterator it_clause;
  for (it_clause = clauses.begin(); it_clause != clauses.end(); it_clause++) {
    typename language_tpl::data_clause_t * data_clause = language_tpl::isDataClause(*it_clause);
    if (data_clause != NULL) {
      Descriptor::data_t * data = dataFromSections(language_tpl::getDataSection(data_clause));
      data->mode = (::KLT::Descriptor::mode_e)language_tpl::getMode(data_clause);
      data->liveness = (::KLT::Descriptor::liveness_e)language_tpl::getLiveness(data_clause);
      datas.push_back(data);
    }
  }
}

template <class language_tpl>
void Compiler<language_tpl>::convertAllocList(const std::vector<clause_t *> & clauses, std::vector<std::pair<Descriptor::data_t *, SgExpression *> > & datas) const {
  typename std::vector<typename language_tpl::clause_t *>::const_iterator it_clause;
  for (it_clause = clauses.begin(); it_clause != clauses.end(); it_clause++) {
    typename language_tpl::alloc_clause_t * alloc_clause = language_tpl::isAllocClause(*it_clause);
    if (alloc_clause != NULL) {
      Descriptor::data_t * data = dataFromSections(language_tpl::getDataSection(alloc_clause));
      data->mode = (::KLT::Descriptor::mode_e)language_tpl::getMode(alloc_clause);
      data->liveness = (::KLT::Descriptor::liveness_e)language_tpl::getLiveness(alloc_clause);
      SgExpression * device_id = language_tpl::getDeviceID(alloc_clause);
      datas.push_back(std::pair<Descriptor::data_t *, SgExpression *>(data, device_id));
    }
  }
}

////////////////////////////////////////////////// Extract Kernels & Loops

template <class language_tpl>
void Compiler<language_tpl>::extractDirectives(
  const std::vector<directive_t *> & directives,
  std::map<directive_t *, SgForStatement *> & loop_directive_map,
  std::map<directive_t *, extracted_kernel_t> & kernel_directives_map,
  std::map<directive_t *, extracted_dataenv_t > & dataenv_directives_map
) const {
  typename std::vector<directive_t *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    directive_t * directive = *it_directive;

    dataenv_construct_t * dataenv_construct = language_tpl::isDataEnvironmentConstruct(directive->construct);
    kernel_construct_t * kernel_construct = language_tpl::isKernelConstruct(directive->construct);
    loop_construct_t * loop_construct = language_tpl::isLoopConstruct(directive->construct);

    if (dataenv_construct != NULL) {
      std::vector<std::pair<Descriptor::data_t *, SgExpression *> > & datas = dataenv_directives_map.insert(
        std::pair<directive_t *, extracted_dataenv_t>(
          directive,
          extracted_dataenv_t(
            language_tpl::getDataEnvironmentRegion(dataenv_construct),
            std::vector<std::pair<Descriptor::data_t *, SgExpression *> >()
      ))).first->second.second;
      convertAllocList(directive->clause_list, datas);
    }
    else if (kernel_construct != NULL) {
      // Where to store the result
      extracted_kernel_t & kernel_directive = kernel_directives_map[directive];

      // Associated code region.
      SgScopeStatement * region = language_tpl::getKernelRegion(kernel_construct);

      // Associated data
      std::vector<Descriptor::data_t *> data;
      convertDataList(directive->clause_list, data);

      // Extract kernel
      kernel_directive.first = KLT::Kernel::kernel_t::extract(region, data, kernel_directive.second);

      // Target kind and device ID
      std::pair< ::DLX::target_e, SgExpression *> dev_info = language_tpl::getKernelDeviceInfo(directive->clause_list);
      switch (dev_info.first) {
        case ::DLX::e_target_unknown:
          assert(false);
        case ::DLX::e_target_host:
          kernel_directive.first->target = Descriptor::e_target_host;
          break;
        case ::DLX::e_target_threads:     
          language_tpl::collectKernelNumThreads(directive->clause_list, kernel_directive.first->num_threads);
          kernel_directive.first->target = threads_target;
          break;
        case ::DLX::e_target_accelerator:
          language_tpl::collectKernelNumGangsAndWorkers(directive->clause_list, kernel_directive.first->num_gangs, kernel_directive.first->num_workers);
          kernel_directive.first->target = accelerator_target;
          break;
        default:
          assert(false);
      }
      kernel_directive.first->device_id = dev_info.second;
    }
    else if (loop_construct != NULL) {
      loop_directive_map.insert(std::pair<directive_t *, SgForStatement *>(directive, language_tpl::getLoopStatement(loop_construct)));
    }
    else assert(false);
  }
}

////////////////////////////////////////////////// Tiling

template <class language_tpl>
LoopTree::node_t * Compiler<language_tpl>::applyTiling(LoopTree::node_t * node, const tiling_info_t & tiling_info, LoopTree::node_t * parent, size_t & tile_cnt) const {
  typedef std::vector<LoopTree::node_t *> node_list_t;
  typedef node_list_t::const_iterator node_list_citer_t;

  switch (node->kind) {
    case LoopTree::e_block:
    {
      const node_list_t & children = ((LoopTree::block_t *)node)->children;
      LoopTree::block_t * block = new LoopTree::block_t();
        block->parent = parent;
      for (node_list_citer_t it = children.begin(); it != children.end(); it++)
        block->children.push_back(applyTiling(*it, tiling_info, block, tile_cnt));
      return block;
    }
    case LoopTree::e_cond:
    {
      LoopTree::cond_t * cond = new LoopTree::cond_t(*(LoopTree::cond_t *)node);
        cond->parent = parent;
      cond->branch_true = applyTiling(((LoopTree::cond_t *)node)->branch_true, tiling_info, cond, tile_cnt);
      cond->branch_false = applyTiling(((LoopTree::cond_t *)node)->branch_false, tiling_info, cond, tile_cnt);
      return cond;
    }
    case LoopTree::e_loop:
    {
      LoopTree::loop_t * loop = (LoopTree::loop_t *)node;
      typename std::map<size_t, std::map<size_t, tile_parameter_t *> >::const_iterator it = tiling_info.tiling_map.find(loop->id);
      if (it != tiling_info.tiling_map.end()) {
        const std::map<size_t, tile_parameter_t *> & tiling = it->second;

        LoopTree::tile_t * first = NULL;
        LoopTree::tile_t * last = NULL;

        bool success = ::KLT::Generator::createTiles<language_tpl>(loop, tiling, first, last, tile_cnt);
        assert(success == true);

        assert(first != NULL);
        first->parent = parent;

        assert(last != NULL);
        last->next_tile = NULL;
        last->next_node = applyTiling(loop->body, tiling_info, first, tile_cnt);

        return first;
      }
      else {
        LoopTree::loop_t * loop = new LoopTree::loop_t(*(LoopTree::loop_t *)node);
          loop->parent = parent;
        loop->body = applyTiling(((LoopTree::loop_t *)node)->body, tiling_info, loop, tile_cnt);
        return loop;
      }
      assert(false);
    }
    case LoopTree::e_stmt:
    {
      LoopTree::stmt_t * stmt = new LoopTree::stmt_t(*(LoopTree::stmt_t *)node);
        stmt->parent = parent;
      return stmt;
    }
    case LoopTree::e_tile:
    case LoopTree::e_ignored:
    case LoopTree::e_unknown:
    default:
      assert(false);
  }
  assert(false);
}

template <class language_tpl>
void Compiler<language_tpl>::splitKernelRoot(Kernel::kernel_t * kernel, const tiling_info_t & tiling_info, std::vector<Kernel::kernel_t *> & kernels) const {
  size_t tile_cnt = 0;
  LoopTree::node_t * root = applyTiling(kernel->root, tiling_info, NULL, tile_cnt);
  root = root->finalize();

  // if root is a block containing more than one loop-nest, it could be splitted in multiple subkernels
  Kernel::kernel_t * res = kernel->copy(root);
  kernels.push_back(res);
}

template <class language_tpl>
void Compiler<language_tpl>::applyLoopTiling(
  Kernel::kernel_t * kernel,
  const std::map<directive_t *, size_t> & directive_loop_id_map,
  std::map<tiling_info_t *, std::vector<Kernel::kernel_t *> > & tiled_kernels
) const {
  tiling_info_t * tiling_info = new tiling_info_t();

  tiling_info->target = kernel->target;

  typename std::map<directive_t *, size_t>::const_iterator it_loop;
  for (it_loop = directive_loop_id_map.begin(); it_loop != directive_loop_id_map.end(); it_loop++) {
    directive_t * directive = it_loop->first;
    size_t loop_id = it_loop->second;

    loop_construct_t * loop_construct = language_tpl::isLoopConstruct(directive->construct);
    assert(loop_construct != NULL);

    typename std::vector<clause_t *>::const_iterator it_clause;
    size_t tile_cnt = 0;
    for (it_clause = directive->clause_list.begin(); it_clause != directive->clause_list.end(); it_clause++) {
      tile_clause_t * tile_clause = language_tpl::isTileClause(*it_clause);
      if (tile_clause != NULL) {
        tiling_info->tiling_map[loop_id][tile_cnt++] = new tile_parameter_t(tile_clause->parameters);
      }
    }
  }

  splitKernelRoot(kernel, *tiling_info, tiled_kernels[tiling_info]);
}

//////////////////////////////////////  Generate Kernels

template <class language_tpl>
void Compiler<language_tpl>::generateAllKernels(
  const std::map<directive_t *, SgForStatement *> & loop_directive_map,
  const std::map<directive_t *, extracted_kernel_t> & kernel_directives_map,
  kernel_directive_translation_map_t & kernel_directive_translation_map
) {

  typename std::map<directive_t *, extracted_kernel_t>::const_iterator it_kernel_directive;
  for (it_kernel_directive = kernel_directives_map.begin(); it_kernel_directive != kernel_directives_map.end(); it_kernel_directive++) {
    directive_t * directive = it_kernel_directive->first;
    assert(language_tpl::isKernelConstruct(directive->construct));

    KLT::Kernel::kernel_t * kernel = it_kernel_directive->second.first;
    assert(kernel != NULL);

    // Link directives to loop-ID through SgForStatement
    std::map<directive_t *, size_t> directive_loop_id_map;
    maps_composition(loop_directive_map, it_kernel_directive->second.second, directive_loop_id_map);

    // Associated the kernel, its loops, and the map to store tilled kernels to the directive.
    std::map<const LoopTree::loop_t *, Descriptor::loop_t *> loop_translation_map;
    kernel_directive_translation_map[directive].original = kernel;
    kernel->root->collectLoops(kernel_directive_translation_map[directive].loops, loop_translation_map);
    std::map<tiling_info_t *, kernel_deps_map_t> & tiled_generated_kernels = kernel_directive_translation_map[directive].tiled;

    // 'loops' should be sorted by ID and IDs should range from 0 to |loops|-1
    assert(kernel_directive_translation_map[directive].loops.front()->id == 0);    
    assert(kernel_directive_translation_map[directive].loops.back()->id == kernel_directive_translation_map[directive].loops.size() - 1);

    // Should not find any tile at this point
    { std::vector<Descriptor::tile_t *> tiles; kernel->root->collectTiles(tiles, loop_translation_map); assert(tiles.size() == 0); }

    // Generate 'tiling_info' and associated sub-kernels
    std::map<tiling_info_t *, std::vector<KLT::Kernel::kernel_t *> > tiled_kernels;
    applyLoopTiling(kernel, directive_loop_id_map, tiled_kernels);

    // For each 'tiling_info'
    typename std::map<tiling_info_t *, std::vector<KLT::Kernel::kernel_t *> >::const_iterator it_tiled_kernel;
    for (it_tiled_kernel = tiled_kernels.begin(); it_tiled_kernel != tiled_kernels.end(); it_tiled_kernel++) {
      std::map<KLT::Kernel::kernel_t *, KLT::Descriptor::kernel_t *> translation_map;
      std::map<KLT::Descriptor::kernel_t *, KLT::Kernel::kernel_t *> rtranslation_map;

      tiling_info_t * tiling_info = it_tiled_kernel->first;
      const std::vector<KLT::Kernel::kernel_t *> & subkernels = it_tiled_kernel->second;

      // To store the generated sub-kernels and their dependencies
      kernel_deps_map_t & kernels = tiled_generated_kernels[tiling_info];

      // Build all the sub-kernels for one tiling_info
      std::vector<KLT::Kernel::kernel_t *>::const_iterator it_kernel;
      for (it_kernel = subkernels.begin(); it_kernel != subkernels.end(); it_kernel++) {
        KLT::Kernel::kernel_t * subkernel = *it_kernel;

        assert(subkernel != NULL);
        assert(subkernel->root != NULL);
        assert(dynamic_cast<KLT::LoopTree::node_t *>(subkernel->root) != NULL);

        assert(subkernel->target != ::KLT::Descriptor::e_target_unknown);

#if VERBOSE
        std::cerr << "[Info] (Compiler<language_tpl>::generateAllKernels) Build subkernel for target " << subkernel->target << std::endl;
#endif

        // Descriptor for kernel builder
        MFB::Driver<MFB::KLT::KLT>::kernel_desc_t kernel_desc(subkernel->root, subkernel->target, subkernel->parameters, subkernel->data, &generator);

        // Call builder
        KLT::Descriptor::kernel_t * result = driver.build<KLT::Kernel::kernel_t>(kernel_desc);

        // Insert result in containers
        kernels.insert(std::pair<KLT::Descriptor::kernel_t *, std::vector<KLT::Descriptor::kernel_t *> >(result, std::vector<KLT::Descriptor::kernel_t *>()));
        translation_map.insert(std::pair<KLT::Kernel::kernel_t *, KLT::Descriptor::kernel_t *>(subkernel, result));
        rtranslation_map.insert(std::pair<KLT::Descriptor::kernel_t *, KLT::Kernel::kernel_t *>(result, subkernel));
      }

      // Figures out the dependencies between sub-kernels
      generator.solveDataFlow(kernel, subkernels, kernels, translation_map, rtranslation_map);
    }
  }
}

//////////////////////////////////////

template <class language_tpl>
void Compiler<language_tpl>::compile(SgNode * node) {
  std::map<directive_t *, extracted_dataenv_t> dataenv_directives_map;
  kernel_directive_translation_map_t kernel_directive_translation_map;

  std::map<SgScopeStatement *, SgScopeStatement *> stmt_replacement_map;

  bool parsed = ::DLX::Compiler<language_tpl>::parse(node);
  assert(parsed);

  {
    std::map<directive_t *, SgForStatement *> loop_directive_map;
    std::map<directive_t *, extracted_kernel_t> kernel_directives_map;

    // Extract KLT's representation 
    extractDirectives(::DLX::Compiler<language_tpl>::frontend.directives, loop_directive_map, kernel_directives_map, dataenv_directives_map);

    // Apply tiling (and other transformations). Generate multiple versions of each kernel. Each version can be made of multiple subkernels.
    generateAllKernels(loop_directive_map, kernel_directives_map, kernel_directive_translation_map);
  }

  {
    typename kernel_directive_translation_map_t::const_iterator it_directive;
#if OUTPUT_SUBKERNELS_GRAPHVIZ
    for (it_directive = kernel_directive_translation_map.begin(); it_directive != kernel_directive_translation_map.end(); it_directive++) {
      std::ostringstream oss; oss << "subkernels_" << it_directive->first << ".dot";
      std::ofstream out(oss.str().c_str(), std::ofstream::out);
      it_directive->second.toGraphViz(out);
      out.close();
    }
#endif
    // Replace kernel directives by host-code for generated kernel
    for (it_directive = kernel_directive_translation_map.begin(); it_directive != kernel_directive_translation_map.end(); it_directive++) {
      directive_t * directive = it_directive->first;
      const subkernel_result_t & subkernel_result = it_directive->second;

      kernel_construct_t * kernel_construct = language_tpl::isKernelConstruct(directive->construct);
      assert(kernel_construct != NULL);
      SgScopeStatement * region = language_tpl::getKernelRegion(kernel_construct);

      generator.getHostAPI().use(driver, region); // Add to the file containing 'region' 
      generator.getKernelID(subkernel_result.original); // enable to use 'KLT::Generator::getKernelID() const' when inside 'KLT::Generator::instanciateOnHost<language_tpl>(..) const'

      // Replace directive by generated host code: create kernel, configure, launch
      SgBasicBlock * bb = generator.instanciateOnHost<language_tpl>(directive, subkernel_result.original, subkernel_result.loops);
      assert(bb != NULL);

      stmt_replacement_map.insert(std::pair<SgScopeStatement *, SgScopeStatement *>(region, bb));
      SageInterface::replaceStatement(region, bb);
    }
  }
  {
    typename std::map<directive_t *, extracted_dataenv_t>::const_iterator it_directive;
    for (it_directive = dataenv_directives_map.begin(); it_directive != dataenv_directives_map.end(); it_directive++) {
      const extracted_dataenv_t & extracted_dataenv = it_directive->second;
      SgScopeStatement * region = extracted_dataenv.first;
      std::map<SgScopeStatement *, SgScopeStatement *>::const_iterator it;
      while ((it = stmt_replacement_map.find(region)) != stmt_replacement_map.end())
        region = it->second;
      SgBasicBlock * bb = generator.insertDataEnvironment(region, extracted_dataenv.second);
      stmt_replacement_map.insert(std::pair<SgScopeStatement *, SgScopeStatement *>(region, bb));
    }
  }

  // Add the description of this kernel to the static data (all subkernels of all versions)
  generator.addToStaticData<language_tpl>(kernel_directive_translation_map);

/*
char * opencl_kernel_file = "/media/ssd/projects/rose-release-2015/rose/projects/TileK/tests/rtl/opencl/kernel_0.cl";
char * opencl_kernel_options = "-I/media/ssd/projects/rose-release-2015/rose/src/midend/KLT/include/";
char * opencl_klt_runtime_lib = "/media/ssd/projects/rose-release-2015/rose/src/midend/KLT/lib/rtl/context.c";
*/

  if (accelerator_target == ::KLT::Descriptor::e_target_opencl) {
    MFB::file_id_t static_file_id = generator.getStaticFileID();
    SgType * char_ptr_type = SageBuilder::buildPointerType(SageBuilder::buildCharType());
    ::MDCG::Tools::StaticInitializer::instantiateDeclaration(
      driver, std::string("opencl_kernel_file"),
      NULL, static_file_id, char_ptr_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal(generator.getKernelFileName(accelerator_target))),
      false
    );
    ::MDCG::Tools::StaticInitializer::instantiateDeclaration(
      driver, std::string("opencl_kernel_options"),
      NULL, static_file_id, char_ptr_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal(std::string("-I") + klt_rtl_path + std::string("/include/"))),
      false
    );
    ::MDCG::Tools::StaticInitializer::instantiateDeclaration(
      driver, std::string("opencl_klt_runtime_lib"),
      NULL, static_file_id, char_ptr_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal(klt_rtl_path + std::string("/lib/rtl/context.c"))),
      false
    );
  }

  // Removes all pragma from language_tpl

  std::vector<SgPragmaDeclaration * > pragma_decls = SageInterface::querySubTree<SgPragmaDeclaration>(node);
  std::vector<SgPragmaDeclaration * >::iterator it_pragma_decl;
  for (it_pragma_decl = pragma_decls.begin(); it_pragma_decl != pragma_decls.end(); it_pragma_decl++) {
    std::string directive = (*it_pragma_decl)->get_pragma()->get_pragma();
    if (directive.find(language_tpl::language_label) == 0)
      SageInterface::removeStatement(*it_pragma_decl);
  }
}

} // namespace KLT::DLX

} // namespace KLT

#endif /* __KLT_DLX_COMPILER_HPP__ */

