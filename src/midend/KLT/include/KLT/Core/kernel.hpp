
#ifndef __KLT_KERNEL_HPP__
#define __KLT_KERNEL_HPP__

#include "KLT/Core/descriptor.hpp"

#include <cstddef>
#include <vector>
#include <map>

class SgVariableSymbol;
class SgForStatement;
class SgStatement;
namespace KLT {
  namespace Descriptor {
    struct data_t;
  }
  namespace LoopTree {
    struct node_t;
    struct loop_t;
  }

namespace Kernel {

class kernel_t {
  public:
    typedef ::KLT::Descriptor::data_t data_t;
    typedef ::KLT::LoopTree::node_t node_t;
    typedef ::KLT::LoopTree::loop_t loop_t;
    typedef SgVariableSymbol vsym_t;

    typedef std::vector<vsym_t *> vsym_list_t;
    typedef std::vector<data_t *> data_list_t;

  public:
    node_t * root;

    Descriptor::target_kind_e target;
    SgExpression * device_id;

    vsym_list_t parameters;
    data_list_t data;

    SgExpression * num_threads;
    SgExpression * num_gangs[3];
    SgExpression * num_workers[3];

  protected:
    kernel_t();
    kernel_t(const kernel_t & original, node_t * new_root);

  public:
    kernel_t * copy(node_t * new_root) const;
    static kernel_t * extract(SgStatement * stmt, const data_list_t & data, std::map<SgForStatement *, size_t> & loop_map);
};

}

}

#endif /* __KLT_KERNEL_HPP__ */

