
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/looptree.hpp"
#include "KLT/Core/descriptor.hpp"

#include <set>

#include <cassert>

namespace KLT {

namespace Kernel {

kernel_t::kernel_t() :
  root(NULL),
  target(Descriptor::e_target_unknown),
  device_id(NULL),
  parameters(),
  data(),
  num_threads(NULL),
  num_gangs({NULL, NULL, NULL}),
  num_workers({NULL, NULL, NULL})
{}

kernel_t::kernel_t(const kernel_t & original, node_t * new_root) :
  root(new_root),
  target(original.target),
  device_id(original.device_id),
  parameters(original.parameters),
  data(original.data),
  num_threads(original.num_threads),
  num_gangs({NULL, NULL, NULL}),
  num_workers({NULL, NULL, NULL})
{
  num_gangs[0] = original.num_gangs[0];
  num_gangs[1] = original.num_gangs[1];
  num_gangs[2] = original.num_gangs[2];
  num_workers[0] = original.num_workers[0];
  num_workers[1] = original.num_workers[1];
  num_workers[2] = original.num_workers[2];
}

kernel_t * kernel_t::copy(node_t * new_root) const {
  return new kernel_t(*this, new_root);
}

kernel_t * kernel_t::extract(SgStatement * stmt, const data_list_t & data_, std::map<SgForStatement *, size_t> & loop_map) {
  typedef SgVariableSymbol vsym_t;
  typedef std::set<vsym_t *> vsym_set_t;

  kernel_t * res = new kernel_t();
    res->data = data_;

  data_list_t::const_iterator it_data;
  vsym_set_t data_syms;
  for (it_data = res->data.begin(); it_data != res->data.end(); it_data++)
    data_syms.insert((*it_data)->symbol);

  LoopTree::extraction_context_t ctx(loop_map, data_syms);

  assert(stmt != NULL);
  res->root = LoopTree::node_t::extract(stmt, ctx);
  assert(res->root != NULL);

  res->parameters.insert(res->parameters.end(), ctx.getParameters().begin(), ctx.getParameters().end());

  return res;
}

}

}

