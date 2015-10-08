
#ifndef __KLT_DESCRIPTOR_HPP__
#define __KLT_DESCRIPTOR_HPP__

#include <vector>
#include <string>

#include <cstddef>

class SgExpression;
class SgVariableSymbol;
class SgType;

namespace KLT {

namespace Descriptor {

enum mode_e {
  e_mode_unknown = 0,
  e_mode_ro = 1,
  e_mode_wo = 2,
  e_mode_rw = 3
};

enum liveness_e {
  e_live_unknown = 0,
  e_live_in = 1,
  e_live_out = 2,
  e_live_inout = 3,
  e_live_not = 4
};

enum target_kind_e {
  e_target_unknown = 0,
  e_target_host = 1,
  e_target_threads = 2,
  e_target_opencl = 3,
  e_target_cuda = 4
};

enum tile_kind_e {
  e_not_tile = -1,
  e_static_tile = 0,
  e_dynamic_tile = 1,
  e_last_loop_tile = e_dynamic_tile,
  e_thread_tile = 2,
  e_gang_tile = 3,
  e_worker_tile = 4,
};

struct tile_t {
  size_t id;
  tile_kind_e kind;
  size_t order;
  SgExpression * param;

  tile_t();
  tile_t(size_t id_, tile_kind_e kind_, size_t order_, SgExpression * param_);
};

struct loop_t {
  size_t id;
  SgExpression * lb;
  SgExpression * ub;
  SgExpression * stride;
  std::vector<tile_t *> tiles;
  SgVariableSymbol * iterator;

  loop_t();
  loop_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_, SgVariableSymbol * iterator_);
};

struct section_t {
  SgExpression * offset;
  SgExpression * length;

  section_t(SgExpression * offset_, SgExpression * length_);
};

struct data_t {
  SgVariableSymbol * symbol;
  SgType * base_type;
  std::vector<section_t *> sections;
  mode_e mode;
  liveness_e liveness;

  data_t(SgVariableSymbol * symbol_, SgType * base_type_, mode_e mode_ = e_mode_rw, liveness_e liveness_ = e_live_inout);
};

struct kernel_t {
  size_t id;

  target_kind_e target;

  std::string kernel_name;

  std::vector<loop_t *> loops;
  std::vector<tile_t *> tiles;

  std::vector<SgVariableSymbol *> parameters;
  std::vector<data_t *> data;

  kernel_t(size_t id_, std::string kernel_name_);
};

} // namespace KLT::Descriptor

} // namespace KLT

#endif /* __KLT_DESCRIPTOR_HPP__ */

