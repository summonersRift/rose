
#include "sage3basic.h"

#include "KLT/Core/looptree.hpp"
#include "KLT/Core/descriptor.hpp"

#include <iostream>

#ifndef VERBOSE
# define VERBOSE 0
#endif

#ifndef USE_SAGE_NODE_CLASS_NAME
# define USE_SAGE_NODE_CLASS_NAME 0
#endif

namespace KLT {

namespace LoopTree {

extraction_context_t::extraction_context_t(loop_map_t & loop_map_, const vsym_set_t & data_) :
  loop_map(loop_map_), data(data_), loop_cnt(0), parameters(), iterators(), locals() {}

void extraction_context_t::addParameter(vsym_t * vsym) {
  if (iterators.find(vsym) == iterators.end() && locals.find(vsym) == locals.end() && data.find(vsym) == data.end())
    parameters.insert(vsym);
}

void extraction_context_t::addIterator(vsym_t * vsym) {
  iterators.insert(vsym);
}

void extraction_context_t::addLocal(vsym_t * vsym) {
  locals.insert(vsym);
}

void extraction_context_t::processVarRefs(SgNode * node) {
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(node);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
    assert(sym->get_declaration() != NULL);
    assert(sym->get_declaration()->get_parent() != NULL);
    assert(sym->get_declaration()->get_parent()->get_parent() != NULL);

    if (isSgClassDefinition(sym->get_declaration()->get_parent()->get_parent())) continue; // reference to a field ('x' in 'a.x')

    addParameter(sym);
  }
}

size_t extraction_context_t::nextLoopID() {
  return loop_cnt++;
}

void extraction_context_t::registerLoop(SgForStatement * for_stmt, loop_t * loop) {
  loop_map.insert(std::pair<SgForStatement *, size_t>(for_stmt, loop->id));
}

const extraction_context_t::vsym_set_t & extraction_context_t::getParameters() const {
  return parameters;
}

////////////////////////////////////////////////

kind_e kindOf(SgStatement * stmt) {
       if ( isSgBasicBlock          (stmt)    ) return e_block;
  else if ( isSgForStatement        (stmt)    ) return e_loop;
  else if ( isSgIfStmt              (stmt)    ) return e_cond;
  else if ( isSgExprStatement       (stmt) ||
            isSgVariableDeclaration (stmt)    ) return e_stmt;
  else if ( isSgPragmaDeclaration   (stmt)    ) return e_ignored;
  else                                         return e_unknown;
}

// Constructors

node_t::node_t(kind_e kind_) :
  kind(kind_), parent(NULL) {}

node_t::~node_t() {}

block_t::block_t() :
  node_t(e_block), children() {}

block_t::~block_t() {}

cond_t::cond_t(SgExpression * cond) :
  node_t(e_cond), condition(cond), branch_true(NULL), branch_false(NULL) {}

cond_t::~cond_t() {}

loop_t::loop_t(size_t id_, SgVariableSymbol * it, SgExpression * lb, SgExpression * ub, SgExpression * stride_) :
  node_t(e_loop), id(id_), iterator(it), lower_bound(lb), upper_bound(ub), stride(stride_), body(NULL) {}

loop_t::~loop_t() {}

stmt_t::stmt_t(SgStatement * stmt) :
  node_t(e_stmt), statement(stmt) {}

stmt_t::~stmt_t() {}

tile_t::tile_t(size_t id_, unsigned long kind_, size_t order_, SgExpression * param_, loop_t * loop_, size_t tile_id_) :
  node_t(e_tile), id(id_), kind(kind_), order(order_), param(param_), loop(loop_), tile_id(tile_id_), next_tile(NULL), next_node(NULL) {}

tile_t::~tile_t() {}

// Extract Methods

node_t * node_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  node_t * res = NULL;

  switch (kindOf(stmt)) {
    case e_block: res = block_t::extract(stmt, ctx); break;
    case e_cond:  res = cond_t::extract (stmt, ctx); break;
    case e_loop:  res = loop_t::extract (stmt, ctx); break;
    case e_stmt:  res = stmt_t::extract (stmt, ctx); break;

    case e_ignored:
#if VERBOSE
      std::cerr << "[Warning] (KLT::LoopTrees::node_t::extract) Ignored statement " << stmt << " (type: " << stmt->class_name() << ")." << std::endl;
#endif
      break;

    case e_unknown:
    default: 
      std::cerr << "[Error] (KLT::LoopTrees::node_t::extract) Uknown statement " << stmt << " (type: " << stmt->class_name() << ")." << std::endl;
      assert(false);
  }

  if (res == NULL) {
#if VERBOSE
    std::cerr << "[Warning] (KLT::LoopTrees::node_t::extract) Statement " << stmt << " (type: " << stmt->class_name() << ") cannot be translated." << std::endl;
#endif
  }

  return res;
}

block_t * block_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgBasicBlock * bb = isSgBasicBlock(stmt);
  assert(bb != NULL);

  block_t * block = new block_t();

  std::vector<SgStatement *>::const_iterator it_stmt;
  for (it_stmt = bb->get_statements().begin(); it_stmt != bb->get_statements().end(); it_stmt++) {
    node_t * node = node_t::extract(*it_stmt, ctx);
    if (node != NULL)
      block->children.push_back(node);
  }

  return block;
}

cond_t * cond_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgIfStmt * if_stmt = isSgIfStmt(stmt);
  assert(if_stmt != NULL);

  SgExprStatement * cond_stmt = isSgExprStatement(if_stmt->get_conditional());
  assert(cond_stmt != NULL);
  SgExpression * cond_expr = cond_stmt->get_expression();
  assert(cond_expr != NULL);

  ctx.processVarRefs(cond_expr);

  cond_t * cond = new cond_t(cond_expr);
      
  cond->branch_true = node_t::extract(if_stmt->get_true_body(), ctx);
  cond->branch_false = node_t::extract(if_stmt->get_false_body(), ctx);
      
  return cond;
}

loop_t * loop_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgForStatement * for_stmt = isSgForStatement(stmt);
  assert(for_stmt != NULL);

  SgVariableSymbol * iterator = NULL;
  SgExpression * lower_bound = NULL;
  SgExpression * upper_bound = NULL;
  SgExpression * stride = NULL;

  bool canon_loop = SageInterface::getForLoopInformations(for_stmt, iterator, lower_bound, upper_bound, stride);
  assert(canon_loop == true);

  ctx.addIterator(iterator);

  ctx.processVarRefs(for_stmt->get_for_init_stmt());
  ctx.processVarRefs(for_stmt->get_test());
  ctx.processVarRefs(for_stmt->get_increment());

  loop_t * loop = new loop_t(ctx.nextLoopID(), iterator, lower_bound, upper_bound, stride);

  ctx.registerLoop(for_stmt, loop);

  loop->body = node_t::extract(for_stmt->get_loop_body(), ctx);

  return loop;
}

stmt_t * stmt_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  if (isSgExprStatement(stmt)) {
    SgExprStatement * expr_stmt = (SgExprStatement *)stmt;
    SgExpression * expr = expr_stmt->get_expression();
    assert(expr != NULL);

    ctx.processVarRefs(expr);

    return new stmt_t(stmt);
  }
  else if (isSgVariableDeclaration(stmt)) {
    SgVariableDeclaration * var_decl = isSgVariableDeclaration(stmt);
    assert(var_decl != NULL);
    SgScopeStatement * scope = var_decl->get_scope();
    assert(scope != NULL);

    const std::vector<SgInitializedName *> & decls = var_decl->get_variables();
    std::vector<SgInitializedName *>::const_iterator it_decl;
    for (it_decl = decls.begin(); it_decl != decls.end(); it_decl++) {
      SgVariableSymbol * var_sym = scope->lookup_variable_symbol((*it_decl)->get_name());
      assert(var_sym != NULL);
      ctx.addLocal(var_sym);
    }

    ctx.processVarRefs(stmt);

    return new stmt_t(stmt);
  }
  else assert(false);
}

std::string node_t::getGraphVizLabel() const {
  std::ostringstream oss; oss << "node_" << this;
  return oss.str();
}

void block_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"block\"]" << std::endl;
  std::vector<node_t *>::const_iterator it;
  for (it = children.begin(); it != children.end(); it++) {
    (*it)->toGraphViz(out, indent + "  ");
    out << indent + "  " << getGraphVizLabel() << " -> " << (*it)->getGraphVizLabel() << std::endl;
  }
}

void block_t::toJSON(std::ostream & out, std::string indent) const {
  out << "[";
  std::vector<node_t *>::const_iterator it = children.begin();
  (*it)->toJSON(out, indent + "  ");
  it++;
  for (; it != children.end(); it++) {
    out << ",";
    (*it)->toJSON(out, indent + "  ");
  }
  out << std::endl;
  out << indent << "]";
}

void cond_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"If (" << condition->unparseToString() << ")\"]" << std::endl;
  branch_true->toGraphViz(out, indent + "  ");
  out << indent + "  " << getGraphVizLabel() << " -> " << branch_true->getGraphVizLabel() << " [label=\"T\"]" << std::endl;
  branch_false->toGraphViz(out, indent + "  ");
  out << indent + "  " << getGraphVizLabel() << " -> " << branch_false->getGraphVizLabel() << " [label=\"F\"]" << std::endl;
}

void cond_t::toJSON(std::ostream & out, std::string indent) const {
  out << "{" << std::endl;
  out << indent << "  \"type\":\"if\"," << std::endl;
  out << indent << "  \"true\":";
  branch_true->toJSON(out, indent + "  ");
  out << "," << std::endl;
  out << indent << "  \"false\":";
  branch_false->toJSON(out, indent + "  ");
  out << std::endl;
  out << indent << "}";
}

std::string loop_t::getGraphVizLabel() const {
  std::ostringstream oss; oss << "loop_" << id;
  return oss.str();
}

void loop_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"Loop#" << id << "\\niterator=" << iterator->get_name().getString() << "\\nlower=" << lower_bound->unparseToString() << "\\nupper=" << upper_bound->unparseToString() << "\\nstride=" << stride->unparseToString() << "\\n\"]" << std::endl;
  body->toGraphViz(out, indent + "  ");
  out << indent + "  " << getGraphVizLabel() << " -> " << body->getGraphVizLabel() << std::endl;
}

void loop_t::toJSON(std::ostream & out, std::string indent) const {
  out << "{" << std::endl;
  out << indent << "  \"type\":\"loop\"," << std::endl;
  out << indent << "  \"iterator\":\"" << iterator->get_name().getString() << "\"," << std::endl;
  out << indent << "  \"lower_bound\":\"" << lower_bound->unparseToString() << "\"," << std::endl;
  out << indent << "  \"upper_bound\":\"" << upper_bound->unparseToString() << "\"," << std::endl;
  out << indent << "  \"stride\":\"" << stride->unparseToString() << "\"," << std::endl;
  out << indent << "  \"body\":";
  body->toJSON(out, indent + "  ");
  out << indent << "}";
}

void tile_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"Tile #" << id << "\\nkind=" << kind << "\\norder=" << order << "\\nloop_id=" << loop->id << "\\ntile_id=" << tile_id << "\\n";
  if (param != NULL) out << "param=" << param->unparseToString() << "\\n";
  out << "\"]" << std::endl;

  if (next_tile != NULL) {
    next_tile->toGraphViz(out, indent + "  ");
    out << indent + "  " << getGraphVizLabel() << " -> " << next_tile->getGraphVizLabel() << std::endl;
  }
  if (next_node != NULL) {
    next_node->toGraphViz(out, indent + "  ");
    out << indent + "  " << getGraphVizLabel() << " -> " << next_node->getGraphVizLabel() << std::endl;
  }
}

void tile_t::toJSON(std::ostream & out, std::string indent) const {
  out << "{" << std::endl;
  out << indent << "  \"type\":\"tile\"," << std::endl;
  out << indent << "  \"id\":" << id << "," << std::endl;
  out << indent << "  \"kind\":" << kind << "," << std::endl;
  out << indent << "  \"order\":" << order << "," << std::endl;
  out << indent << "  \"loop->id\":" << loop->id << "," << std::endl;
  out << indent << "  \"tile_id\":" << tile_id << "," << std::endl;
  if (param != NULL)
    out << indent << "  \"param\":\"" << param->unparseToString() << "\"," << std::endl;
  if (next_tile != NULL) {
    out << indent << "  \"next\":";
    next_tile->toJSON(out, indent + "  ");
    out << std::endl;
  }
  if (next_node != NULL) {
    out << indent << "  \"next\":";
    next_node->toJSON(out, indent + "  ");
    out << std::endl;
  }
  out << indent << "}";
}

struct feature_vector_SgNode_t {
  std::map<VariantT, size_t> node_histogram;

  std::map<size_t, size_t> read_by_dimension; // scalar dim == 0
  std::map<size_t, size_t> write_by_dimension;

  std::map<VariantT, size_t> operation_histogram;
  std::map<VariantT, size_t> index_histogram;

#if USE_SAGE_NODE_CLASS_NAME
  std::map<VariantT, std::string> variant_label;
#endif

  void collect(
    SgNode * node, std::string info_indent = std::string(),
    bool is_lvalue = false, bool is_index = false, size_t lvl_avalue = 0
  );

  void toGraphViz(std::ostream & out, std::string indent, SgStatement * stmt);

  void mapVariantToJSON(const std::map<VariantT, size_t> & map, std::string name, std::ostream & out, const std::string & indent);
  void mapSizeToJSON(const std::map<size_t, size_t> & map, std::string name, std::ostream & out, const std::string & indent);

  void toJSON(std::ostream & out, std::string indent, SgStatement * stmt);
};

void feature_vector_SgNode_t::toGraphViz(std::ostream & out, std::string indent, SgStatement * stmt) {
  std::map<VariantT, size_t>::const_iterator varit;
  std::map<size_t, size_t>::const_iterator dimit;
  out << "code: \\\"" << stmt->unparseToString() << "\\\"\\n";
  out << "node_histogram: ";
  for (varit = node_histogram.begin(); varit != node_histogram.end(); varit++) {
#if USE_SAGE_NODE_CLASS_NAME
    out << variant_label[varit->first] << ":" << varit->second << ",";
#else
    out << varit->first << ":" << varit->second << ",";
#endif
  }
  out << "\\n";
  out << "read_by_dimension: ";
  for (dimit = read_by_dimension.begin(); dimit != read_by_dimension.end(); dimit++) {
    out << dimit->first << ":" << dimit->second << ",";
  }
  out << "\\n";
  out << "write_by_dimension: ";
  for (dimit = write_by_dimension.begin(); dimit != write_by_dimension.end(); dimit++) {
    out << dimit->first << ":" << dimit->second << ",";
  }
  out << "\\n";
  out << "operation_histogram: ";
  for (varit = operation_histogram.begin(); varit != operation_histogram.end(); varit++) {
#if USE_SAGE_NODE_CLASS_NAME
    out << variant_label[varit->first] << ":" << varit->second << ",";
#else
    out << varit->first << ":" << varit->second << ",";
#endif
  }
  out << "\\n";
  out << "index_histogram: ";
  for (varit = index_histogram.begin(); varit != index_histogram.end(); varit++) {
#if USE_SAGE_NODE_CLASS_NAME
    out << variant_label[varit->first] << ":" << varit->second << ",";
#else
    out << varit->first << ":" << varit->second << ",";
#endif
  }
  out << "\\n";
}

void feature_vector_SgNode_t::mapVariantToJSON(const std::map<VariantT, size_t> & map, std::string name, std::ostream & out, const std::string & indent) {
  out << indent << "\"" << name << "\": {";
  std::map<VariantT, size_t>::const_iterator it = map.begin();
  if (it != map.end()) {
#if USE_SAGE_NODE_CLASS_NAME
    out << "\"" << variant_label[it->first] << "\":" << it->second;
#else
    out << "\"" << it->first << "\":" << it->second;
#endif
    it++;
    for (; it != map.end(); it++) {
#if USE_SAGE_NODE_CLASS_NAME
      out << ", \"" << variant_label[it->first] << "\":" << it->second;
#else
      out << ", \"" << it->first << "\":" << it->second;
#endif
    }
  }
  out << "}";
}

void feature_vector_SgNode_t::mapSizeToJSON(const std::map<size_t, size_t> & map, std::string name, std::ostream & out, const std::string & indent) {
  out << indent << "\"" << name << "\": {";
  std::map<size_t, size_t>::const_iterator it = map.begin();
  if (it != map.end()) {
    out << "\"" << it->first << "\":" << it->second;
    it++;
    for (; it != map.end(); it++) {
      out << ", \"" << it->first << "\":" << it->second;
    }
  }
  out << "}";
}

void feature_vector_SgNode_t::toJSON(std::ostream & out, std::string indent, SgStatement * stmt) {
  std::map<VariantT, size_t>::const_iterator varit;
  std::map<size_t, size_t>::const_iterator dimit;
  out << "{" << std::endl;
  out << indent << "  \"type\":\"stmt\"," << std::endl;
  out << indent << "  \"code\":\"" << stmt->unparseToString() << "\"," << std::endl;
  mapVariantToJSON(node_histogram, "node_histogram", out, indent + "  ");
  out << "," << std::endl;
  mapSizeToJSON(read_by_dimension, "read_by_dimension", out, indent + "  ");
  out << "," << std::endl;
  mapSizeToJSON(write_by_dimension, "write_by_dimension", out, indent + "  ");
  out << "," << std::endl;
  mapVariantToJSON(operation_histogram, "operation_histogram", out, indent + "  ");
  out << "," << std::endl;
  mapVariantToJSON(index_histogram, "index_histogram", out, indent + "  ");
  out << std::endl;
  out << indent << "}";
}

void feature_vector_SgNode_t::collect(
  SgNode * node, std::string info_indent,
  bool is_lvalue, bool is_index, size_t lvl_avalue
) {
#if VERBOSE
  std::cerr << info_indent << "|+ " << node->unparseToString() << std::endl;
  std::cerr << info_indent << "|+ " << node->class_name() << " (" << node << ")" << std::endl;
  std::cerr << info_indent << "|+ is_lvalue  = " << is_lvalue << " , is_index   = " << is_index << " , lvl_avalue  = " << lvl_avalue << std::endl;
#endif

  SgStatement * stmt = isSgStatement(node);
  SgExpression * expr = isSgExpression(node);

  if (stmt != NULL) {
    assert(is_lvalue == false);
    assert(is_index  == false);

    SgExprStatement * expr_stmt = isSgExprStatement(stmt);

    if (expr_stmt != NULL) {
      collect(expr_stmt->get_expression(), info_indent + "    ", is_lvalue, is_index, lvl_avalue);
      return;
    }
  }
  else if (expr != NULL) {
    VariantT variant = node->variantT();
    node_histogram[variant] += 1;
#if USE_SAGE_NODE_CLASS_NAME
    variant_label[variant] = node->class_name();
#endif

    SgUnaryOp * una_op = isSgUnaryOp(expr);
    SgBinaryOp * bin_op = isSgBinaryOp(expr);
    SgFunctionCallExp * func_call_expr = isSgFunctionCallExp(expr);
    SgConditionalExp * cond_expr = isSgConditionalExp(expr);
    SgValueExp * value_expr = isSgValueExp(expr);
    SgVarRefExp * varref_expr = isSgVarRefExp(expr);
    SgInitializer * init_expr = isSgInitializer(expr);

    if (una_op != NULL) {
      SgExpression * operand_expr = una_op->get_operand_i();

#if VERBOSE
      std::cerr << info_indent << "|| operand = " << operand_expr->class_name() << " (" << operand_expr << ")" << std::endl;
#endif

      collect(operand_expr, info_indent + "    ", is_lvalue, is_index, lvl_avalue);
      return;
    }
    else if (bin_op != NULL) {
      SgPntrArrRefExp * pntr_arrref_expr = isSgPntrArrRefExp(bin_op);
      SgAssignOp * assign_op = isSgAssignOp(bin_op);
      SgCompoundAssignOp * compound_assign_op = isSgCompoundAssignOp(bin_op);

      SgExpression * lhs_expr = bin_op->get_lhs_operand_i();
      SgExpression * rhs_expr = bin_op->get_rhs_operand_i();

#if VERBOSE
      std::cerr << info_indent << "|| lhs_expr = " << lhs_expr->class_name() << " (" << lhs_expr << ")" << std::endl;
      std::cerr << info_indent << "|| rhs_expr = " << rhs_expr->class_name() << " (" << rhs_expr << ")" << std::endl;
#endif

      if (pntr_arrref_expr != NULL) {
        assert(lvl_avalue == 0); // (a[0])[0]
        while (pntr_arrref_expr != NULL) {
#if VERBOSE
          std::cerr << info_indent << "|| index[" << lvl_avalue << "] = " << rhs_expr->class_name() << " (" << rhs_expr << ")" << std::endl;
#endif
          collect(rhs_expr, info_indent + "    ", is_lvalue, true, 0);
          lvl_avalue++;

          pntr_arrref_expr = isSgPntrArrRefExp(lhs_expr);
          if (pntr_arrref_expr != NULL) {
            lhs_expr = pntr_arrref_expr->get_lhs_operand_i();
            rhs_expr = pntr_arrref_expr->get_rhs_operand_i();
          }
        }
        assert(lvl_avalue > 0);
        collect(lhs_expr, info_indent + "    ", is_lvalue, is_index, lvl_avalue);
      }
      else if (assign_op != NULL) {
        assert(!is_index);       // a[i=0]
        assert(lvl_avalue == 0); // (a=b)[0]
        collect(lhs_expr, info_indent + "    ",      true,    false, lvl_avalue);
        collect(rhs_expr, info_indent + "    ", is_lvalue,    false, lvl_avalue);
      }
      else if (compound_assign_op != NULL) {
        assert(!is_index);       // a[i+=1]
        assert(lvl_avalue == 0); // (a+=b)[0]
        collect(lhs_expr, info_indent + "    ",      true,    false, lvl_avalue);
        collect(lhs_expr, info_indent + "    ", is_lvalue,    false, lvl_avalue);
        collect(rhs_expr, info_indent + "    ", is_lvalue,    false, lvl_avalue);

        operation_histogram[variant] += 1;
      }
      else {
        collect(lhs_expr, info_indent + "    ", is_lvalue, is_index, lvl_avalue);
        collect(rhs_expr, info_indent + "    ", is_lvalue, is_index, lvl_avalue);

        if (is_index)
          index_histogram[variant] += 1;
        else
          operation_histogram[variant] += 1;
      }

      return;
    }
    else if (func_call_expr != NULL) {
      // fail
    }
    else if (cond_expr != NULL) {
      SgExpression * conditional_exp = cond_expr->get_conditional_exp();
      SgExpression * true_exp = cond_expr->get_true_exp();
      SgExpression * false_exp = cond_expr->get_false_exp();

#if VERBOSE
      std::cerr << info_indent << "|| conditional_exp = " << conditional_exp->class_name() << " (" << conditional_exp << ")" << std::endl;
      std::cerr << info_indent << "|| true_exp = " << true_exp->class_name() << " (" << true_exp << ")" << std::endl;
      std::cerr << info_indent << "|| false_exp = " << false_exp->class_name() << " (" << false_exp << ")" << std::endl;
#endif

      collect(conditional_exp, info_indent + "    ", is_lvalue, is_index, lvl_avalue);
      collect(       true_exp, info_indent + "    ", is_lvalue, is_index, lvl_avalue);
      collect(      false_exp, info_indent + "    ", is_lvalue, is_index, lvl_avalue);
    }
    else if (value_expr != NULL) {
      if (is_index)
        index_histogram[variant] += 1;
      else
        operation_histogram[variant] += 1;
      return;
    }
    else if (varref_expr != NULL) {
      if (is_lvalue)
        write_by_dimension[lvl_avalue] += 1;
      else
        read_by_dimension[lvl_avalue] += 1;
      return;
    }
    else if (init_expr != NULL) {
      // fail
    }
//  return;
  }

  std::cerr << "[Error] (collect) Unknown type of SgNode: " << node->class_name() << std::endl;
  assert(false);
}

void stmt_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"";
  {
    feature_vector_SgNode_t fv_node;
    fv_node.collect(statement, "[Info] (feature_vector_SgNode_t::collect) ");
    fv_node.toGraphViz(out, indent, statement);
  }
  out << "\"]" << std::endl;
}

void stmt_t::toJSON(std::ostream & out, std::string indent) const {
  feature_vector_SgNode_t fv_node;
  fv_node.collect(statement, "[Info] (feature_vector_SgNode_t::collect) ");
  fv_node.toJSON(out, indent, statement);
}

void block_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  std::vector<node_t *>::const_iterator it;
  for (it = children.begin(); it != children.end(); it++)
    (*it)->collectLoops(loops, loop_translation_map);
}

void block_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  std::vector<node_t *>::const_iterator it;
  for (it = children.begin(); it != children.end(); it++)
    (*it)->collectTiles(tiles, loop_translation_map);
}

void cond_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  branch_true->collectLoops(loops, loop_translation_map);
  branch_false->collectLoops(loops, loop_translation_map);
}

void cond_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  branch_true->collectTiles(tiles, loop_translation_map);
  branch_false->collectTiles(tiles, loop_translation_map);
}

template <class T>
struct CompID {
  bool operator() (T * i   , size_t j) { return (i->id < j    );}
  bool operator() (size_t i, T * j   ) { return (    i < j->id);}
  bool operator() (T * i   , T * j   ) { return (i->id < j->id);}
};

CompID<Descriptor::tile_t> cmp_id_tile;
CompID<Descriptor::loop_t> cmp_id_loop;

void loop_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  Descriptor::loop_t * loop_desc = new Descriptor::loop_t(id, lower_bound, upper_bound, stride, iterator);

  loop_translation_map.insert(std::pair<const loop_t *, Descriptor::loop_t *>(this, loop_desc));

#if VERBOSE
  std::cerr << "[Info] (KLT::LoopTree::loop_t::collectLoops) Loop #" << id << std::endl;
#endif

  assert(!std::binary_search(loops.begin(), loops.end(), id, cmp_id_loop));

  loops.insert(
    std::upper_bound(loops.begin(), loops.end(), id, cmp_id_loop),
    loop_desc
  );

  body->collectLoops(loops, loop_translation_map);
}

void loop_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  body->collectTiles(tiles, loop_translation_map);
}

void tile_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  Descriptor::loop_t * loop_desc = new Descriptor::loop_t(loop->id, loop->lower_bound, loop->upper_bound, loop->stride, loop->iterator);

  loop_translation_map.insert(std::pair<const loop_t *, Descriptor::loop_t *>(loop, loop_desc));

#if VERBOSE
  std::cerr << "[Info] (KLT::LoopTree::tile_t::collectLoops) Loop #" << loop->id << std::endl;
#endif

  if (!std::binary_search(loops.begin(), loops.end(), loop->id, cmp_id_loop))
    loops.insert(
      std::upper_bound(loops.begin(), loops.end(), loop->id, cmp_id_loop),
      loop_desc
    );

  if (next_tile != NULL) next_tile->collectLoops(loops, loop_translation_map);
  if (next_node != NULL) next_node->collectLoops(loops, loop_translation_map);
}

void tile_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {

  Descriptor::tile_t * tile_desc = new Descriptor::tile_t(id, (Descriptor::tile_kind_e)kind, order, param);
  {
    std::vector<Descriptor::tile_t *>::iterator pos = std::upper_bound(tiles.begin(), tiles.end(), id, cmp_id_tile);
    tiles.insert(pos, tile_desc);
  }
  std::map<const loop_t *, Descriptor::loop_t *>::const_iterator it = loop_translation_map.find(loop);
  assert(it != loop_translation_map.end());
  {
    std::vector<Descriptor::tile_t *>::iterator pos = std::upper_bound(it->second->tiles.begin(), it->second->tiles.end(), id, cmp_id_tile);
    it->second->tiles.insert(pos, tile_desc);
  }

  if (next_tile != NULL) next_tile->collectTiles(tiles, loop_translation_map);
  if (next_node != NULL) next_node->collectTiles(tiles, loop_translation_map);
}

void stmt_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {}
void stmt_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {}

node_t * block_t::finalize() {
#if VERBOSE
  std::cerr << "[Info] (KLT::LoopTree::block_t::finalize)" << std::endl;
#endif

  if (children.size() == 1) {
    node_t * res = children[0]->finalize();
    res->parent = parent;
    delete this;
    return res;
  }
  else {
    std::vector<node_t *>::iterator it;
    for (it = children.begin(); it != children.end(); it++)
      *it = (*it)->finalize();
    return this;
  }
}

node_t * cond_t::finalize() {
#if VERBOSE
  std::cerr << "[Info] (KLT::LoopTree::cond_t::finalize)" << std::endl;
#endif

  branch_true  = branch_true->finalize();
  branch_false = branch_false->finalize();
  return this;
}

node_t * loop_t::finalize() {
#if VERBOSE
  std::cerr << "[Info] (KLT::LoopTree::loop_t::finalize) Loop #" << id << std::endl;
#endif

  body = body->finalize();
  return this;
}

node_t * tile_t::finalize() {
#if VERBOSE
  std::cerr << "[Info] (KLT::LoopTree::tile_t::finalize) Loop #" << loop->id << ", Tile #" << tile_id << std::endl;
#endif

  if (next_node != NULL) { // it is the last tile in a chain
    assert(next_tile == NULL);

    node_t * node = next_node->finalize();  // recursive call on the next node

    if (node->kind == e_tile) { // 'next_node' was a block with only one children which is a tile
      next_tile = (tile_t *)node;
      next_node = NULL;
    }
    else {
      next_tile = NULL;
      next_node = node;
    }
  }
  else if (next_tile != NULL) { // in a chain of tiles
    assert(next_node == NULL);
    node_t * node = next_tile->finalize();
    assert(node->kind == e_tile);
    next_tile = (tile_t *)node;
  }
  else assert(false);

  if (next_tile != NULL) { // in a chain of tiles and 'next_tile' should be the first of a **ordered-linked-list** of tile_t
    tile_t * curr = this;
    tile_t * prev = NULL;
    do {
      prev = curr; curr = prev->next_tile;
      if (curr == NULL) break;

#if VERBOSE
      if (curr->order == order) {
        std::cerr << "[Warning] (KLT::LoopTree::tile_t::finalize) Tile #" << tile_id << " of loop #" << loop->id << " and tile #" << curr->tile_id << " of loop #" << curr->loop->id << " have the same ordering index = " << order << "." << std::endl;
        std::cerr << "[Warning] (KLT::LoopTree::tile_t::finalize) Resulting tile order is undefined." << std::endl; // should be increasing order of the pair loop ID and tile ID. See sort
      }
#endif
    } while (order > curr->order);
    if (curr == next_tile) {
      // insert first (nothing to do)
    }
    else if (curr != NULL) {
      // insert between 'prev' and 'curr'

      tile_t * res = next_tile;
        res->parent = parent;

      prev->next_tile = this;
      parent = prev;
      next_tile = curr;
      curr->parent = this;

      return res;
    }
    else {
      // insert last (after 'prev')

      tile_t * res = next_tile;
        res->parent = parent;

      next_tile = NULL;
      next_node = prev->next_node;
      parent = prev;
      
      prev->next_tile = this;
      prev->next_node = NULL;

      next_node->parent = this;
      
      return res;
    }
    
  }

  return this;
}

node_t * stmt_t::finalize() { return this; }

}

}

