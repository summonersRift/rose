
#include "sage3basic.h"

#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/driver.hpp"
#include "MFB/KLT/driver.hpp"

#include "MDCG/Model/model.hpp"
#include "MDCG/Tools/model-builder.hpp"

#include "KLT/Core/generator.hpp"
#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/api.hpp"

namespace KLT {

Generator::Generator(
  MFB::Driver<MFB::KLT::KLT> & driver_,
  ::MDCG::Tools::ModelBuilder & model_builder_,
  const std::string & klt_inc_dir,
  const std::string & basename,
  bool target_threads_,
  bool target_opencl_,
  bool target_cuda_
) :
  driver(driver_), model_builder(model_builder_),
  klt_model(model_builder.create()),
  static_filename(basename + "-static.c"),
  static_file_id(driver.create(boost::filesystem::path(static_filename))),
  host_kernel_filename(basename + "-host-kernel.c"),
  host_kernel_file_id(driver.create(boost::filesystem::path(host_kernel_filename))),
  target_threads(target_threads_),
  threads_kernel_filename(basename + "-threads-kernel.c"),
  threads_kernel_file_id(target_threads ? driver.create(boost::filesystem::path(threads_kernel_filename)) : -1),
  target_opencl(target_opencl_),
  opencl_kernel_filename(basename + "-opencl-kernel.cl"),
  opencl_kernel_file_id(target_opencl ? driver.create(boost::filesystem::path(opencl_kernel_filename)) : -1),
  target_cuda(target_cuda_),
  cuda_kernel_filename(basename + "-cuda-kernel.cu"),
  cuda_kernel_file_id(target_cuda ? driver.create(boost::filesystem::path(cuda_kernel_filename)) : -1),
  host_api(),
  kernel_api(),
  call_interface(driver, &kernel_api),
  kernel_map(), kernel_rmap()
{
  std::cerr << "[Info] Create KLT::Core::Generator with " << target_threads << target_opencl << target_cuda << std::endl;

  loadModel(klt_inc_dir);

  driver.setUnparsedFile(static_file_id);
  driver.setCompiledFile(static_file_id);
  getHostAPI().use(driver, static_file_id);

  driver.setUnparsedFile(host_kernel_file_id);
  driver.setCompiledFile(host_kernel_file_id);
  getKernelAPI().use(driver, host_kernel_file_id);

  if (target_threads) {
    driver.setUnparsedFile(threads_kernel_file_id);
    driver.setCompiledFile(threads_kernel_file_id);
    getKernelAPI().use(driver, threads_kernel_file_id);
  }

  if (target_opencl) {
    driver.setUnparsedFile(opencl_kernel_file_id);
    getKernelAPI().use(driver, opencl_kernel_file_id);
  }

  if (target_cuda) {
    driver.setUnparsedFile(cuda_kernel_file_id);
    getKernelAPI().use(driver, cuda_kernel_file_id);
  }
}

void Generator::loadModel(const std::string & klt_inc_dir) {
  model_builder.add(klt_model, "data-environment",    klt_inc_dir + "/KLT/RTL", "h");
  model_builder.add(klt_model, "data",    klt_inc_dir + "/KLT/RTL", "h");
  model_builder.add(klt_model, "tile",    klt_inc_dir + "/KLT/RTL", "h");
  model_builder.add(klt_model, "loop",    klt_inc_dir + "/KLT/RTL", "h");
  model_builder.add(klt_model, "kernel",  klt_inc_dir + "/KLT/RTL", "h");
  model_builder.add(klt_model, "context", klt_inc_dir + "/KLT/RTL", "h");

  host_api.load(model_builder.get(klt_model));

  Descriptor::target_kind_e target = Descriptor::e_target_host;

       if (target_opencl) target = Descriptor::e_target_opencl;
  else if (target_cuda)   target = Descriptor::e_target_cuda;

  kernel_api.load(model_builder.get(klt_model), target);
}

MFB::Driver<MFB::KLT::KLT> & Generator::getDriver() { return driver; }
const MFB::Driver<MFB::KLT::KLT> & Generator::getDriver() const { return driver; }
::MDCG::Tools::ModelBuilder & Generator::getModelBuilder() { return model_builder; }
const ::MDCG::Tools::ModelBuilder & Generator::getModelBuilder() const { return model_builder; }

const API::host_t & Generator::getHostAPI() const { return host_api; }
const std::string & Generator::getStaticFileName() const { return static_filename; }
MFB::file_id_t Generator::getStaticFileID() const { return static_file_id; }

const API::kernel_t & Generator::getKernelAPI() const {
  return kernel_api;
}

API::call_interface_t & Generator::getCallInterface() {
  return call_interface;
}

const API::call_interface_t & Generator::getCallInterface() const {
  return call_interface;
}

const std::string & Generator::getKernelFileName(Descriptor::target_kind_e target) const {
  switch (target) {
    case Descriptor::e_target_host:    return host_kernel_filename;
    case Descriptor::e_target_threads: return threads_kernel_filename;
    case Descriptor::e_target_opencl:  return opencl_kernel_filename;
    case Descriptor::e_target_cuda:    return cuda_kernel_filename;
    case Descriptor::e_target_unknown:
    default:                           assert(false);
  }
}

MFB::file_id_t Generator::getKernelFileID(Descriptor::target_kind_e target) const {
  switch (target) {
    case Descriptor::e_target_host:    return host_kernel_file_id;
    case Descriptor::e_target_threads: return threads_kernel_file_id;
    case Descriptor::e_target_opencl:  return opencl_kernel_file_id;
    case Descriptor::e_target_cuda:    return cuda_kernel_file_id;
    case Descriptor::e_target_unknown:
    default:                           assert(false);
  }
}

size_t Generator::getKernelID(Kernel::kernel_t * kernel) {
  std::map<Kernel::kernel_t *, size_t>::const_iterator it = kernel_map.find(kernel);
  if (it == kernel_map.end()) {
    size_t id = kernel_map.size();
    kernel_map.insert(std::pair<Kernel::kernel_t *, size_t>(kernel, id));
    kernel_rmap.insert(std::pair<size_t, Kernel::kernel_t *>(id, kernel));
    return id;
  }
  else return it->second;
}

size_t Generator::getKernelID(Kernel::kernel_t * kernel) const {
  std::map<Kernel::kernel_t *, size_t>::const_iterator it = kernel_map.find(kernel);
  assert(it != kernel_map.end());
  return it->second;
}

Kernel::kernel_t * Generator::getKernelByID(size_t id) const {
  std::map<size_t, Kernel::kernel_t *>::const_iterator it = kernel_rmap.find(id);
  assert(it != kernel_rmap.end());
  return it->second;
}

SgBasicBlock * Generator::insertDataEnvironment(SgScopeStatement * region, const std::vector<std::pair<Descriptor::data_t *, SgExpression *> > & datas) const {
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();
  SageInterface::replaceStatement(region, bb);

  SageInterface::appendStatement(host_api.buildPushDataEnvironment(), bb);
  ::MDCG::Model::class_t data_class = model_builder.get(klt_model).lookup< ::MDCG::Model::class_t>("klt_data_t");
  SgVariableSymbol * data_arr_sym = ::MDCG::Tools::StaticInitializer::addArrayDeclaration< ::KLT::MDCG::DataDesc>(
      driver, data_class, datas.size(), datas.begin(), datas.end(), bb, static_file_id, false, std::string("data")
  );

  std::vector<std::pair<Descriptor::data_t *, SgExpression *> >::const_iterator it_data;
  size_t idx = 0;
  for (it_data = datas.begin(); it_data != datas.end(); it_data++)
    SageInterface::appendStatement(host_api.buildDataAllocation(data_arr_sym, idx++, it_data->second), bb);

  SageInterface::appendStatement(region, bb);

  SageInterface::appendStatement(host_api.buildPopDataEnvironment(), bb);

  return bb;
}

void Generator::solveDataFlow(
  Kernel::kernel_t * kernel,
  const std::vector<Kernel::kernel_t *> & subkernels,
  Utils::kernel_deps_map_t & kernel_deps_map,
  const std::map<Kernel::kernel_t *, Descriptor::kernel_t *> & translation_map,
  const std::map<Descriptor::kernel_t *, Kernel::kernel_t *> & rtranslation_map
) const {
  // Simplest implementation I can think of: enforce text order...

  Descriptor::kernel_t * previous = NULL;
  Descriptor::kernel_t * current = NULL;

  Utils::kernel_deps_map_t::iterator it_kernel_deps;
  std::map<Kernel::kernel_t *, Descriptor::kernel_t *>::const_iterator it_trans;

  std::vector<Kernel::kernel_t *>::const_iterator it;
  for (it = subkernels.begin(); it != subkernels.end(); it++) {
    it_trans = translation_map.find(*it);
    assert(it_trans != translation_map.end());
    current = it_trans->second;

    it_kernel_deps = kernel_deps_map.find(current);
    assert(it_kernel_deps != kernel_deps_map.end());
    std::vector<Descriptor::kernel_t *> & deps = it_kernel_deps->second;

    if (previous != NULL)
      deps.push_back(previous);

    previous = current;
  }
}

} // namespace KLT

