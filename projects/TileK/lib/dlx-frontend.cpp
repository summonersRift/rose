
#include "sage3basic.h"

#include "DLX/Core/frontend.hpp"
#include "DLX/Core/directives.hpp"
#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/TileK/language.hpp"

#include <queue>
#include <iostream>

#include <cassert>

namespace DLX {

template <> 
template <> 
bool Frontend<TileK::language_t>::findAssociatedNodes<TileK::language_t::e_construct_dataenv>(
  SgLocatedNode * directive_node,
  Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_dataenv> * construct
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.dataenv_region = isSgStatement(SageInterface::getNextStatement(pragma_decl));
  assert(construct->assoc_nodes.dataenv_region != NULL);

  return true;
}

template <> 
template <> 
bool Frontend<TileK::language_t>::findAssociatedNodes<TileK::language_t::e_construct_kernel>(
  SgLocatedNode * directive_node,
  Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_kernel> * construct
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.kernel_region = isSgStatement(SageInterface::getNextStatement(pragma_decl));
  assert(construct->assoc_nodes.kernel_region != NULL);

  return true;
}

template <> 
template <> 
bool Frontend<TileK::language_t>::findAssociatedNodes<TileK::language_t::e_construct_loop>(
  SgLocatedNode * directive_node,
  Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_loop> * construct
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.for_loop = isSgForStatement(SageInterface::getNextStatement(pragma_decl));
  assert(construct->assoc_nodes.for_loop != NULL);

  return true;
}

bool parse_enum_mode(enum mode_e & mode) {
  std::cerr << "[Info] (parse_enum_mode) " << AstFromString::c_char << std::endl;
  if (!Parser::consume("mode")) return false;
  Parser::skip_whitespace();
  if (!Parser::consume(':')) return false;
  Parser::skip_whitespace();
  if (Parser::consume("rw"))
    mode = e_mode_read_write;
  else if (Parser::consume("r"))
    mode = e_mode_read;
  else if (Parser::consume("w"))
    mode = e_mode_write;
  else {
    std::cerr << "[Error] (parse_enum_mode) Unknown read/write mode!" << std::endl;
    assert(false);
  }
  return true;
}

bool parse_enum_liveness(enum liveness_e & live) {
  std::cerr << "[Info] (parse_enum_liveness) " << AstFromString::c_char << std::endl;
  if (!Parser::consume("live")) return false;
  Parser::skip_whitespace();
  if (!Parser::consume(':')) return false;
  Parser::skip_whitespace();
  if (Parser::consume("not"))
    live = e_live_not;
  else if (Parser::consume("inout"))
    live = e_live_inout;
  else if (Parser::consume("in"))
    live = e_live_in;
  else if (Parser::consume("out"))
    live = e_live_out;
  else {
    std::cerr << "[Error] (parse_enum_liveness) Unknown liveness!" << std::endl;
    assert(false);
  }
  return true;
}

bool parse_device_id(SgExpression * & id) {
  std::cerr << "[Info] (parse_device_id) " << AstFromString::c_char << std::endl;
  if (!Parser::consume("device")) return false;
  Parser::skip_whitespace();
  if (!Parser::consume(':')) return false;
  Parser::skip_whitespace();
  if (!Parser::parse<SgExpression *>(id)) {
    std::cerr << "[Error] (parse_device_id) Expect a constant positive integer!" << std::endl;
    assert(false);
  }
  return true;
}

template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_alloc>(
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_alloc> * clause
) {
  std::cerr << "[Info] (Frontend<TileK>::parseClauseParameters<e_clause_alloc>) " << AstFromString::c_char << std::endl;
  if (!Parser::consume('(')) return false;
  Parser::skip_whitespace();
  if (!Parser::parse(clause->parameters.data_section)) return false;
  Parser::skip_whitespace();
  clause->parameters.mode = e_mode_unknown;
  clause->parameters.liveness = e_live_unknown;
  clause->parameters.device_id = NULL;
  while (Parser::consume(',')) {
    Parser::skip_whitespace();
    if (!parse_enum_mode    (clause->parameters.mode)      &&
        !parse_enum_liveness(clause->parameters.liveness)  &&
        !parse_device_id    (clause->parameters.device_id)
    ) return false;
    Parser::skip_whitespace();
  }
  if (!Parser::consume(')')) return false;
  
  return true;
}

template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_data>(
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_data> * clause
) {
  std::cerr << "[Info] (Frontend<TileK>::parseClauseParameters<e_clause_data>) " << AstFromString::c_char << std::endl;
  if (!Parser::consume('(')) return false;
  Parser::skip_whitespace();
  if (!Parser::parse(clause->parameters.data_section)) return false;
  Parser::skip_whitespace();
  clause->parameters.mode = e_mode_unknown;
  clause->parameters.liveness = e_live_unknown;
  while (Parser::consume(',')) {
    Parser::skip_whitespace();
    if (!parse_enum_mode    (clause->parameters.mode)     &&
        !parse_enum_liveness(clause->parameters.liveness)
    ) return false;
    Parser::skip_whitespace();
  }
  if (!Parser::consume(')')) return false;
  
  return true;
}

template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_tile>(
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_tile> * clause
) {
  if (!Parser::one<size_t>(clause->parameters.order, '[', ']'))
    clause->parameters.order = 0;

  if (!Parser::consume('(')) return false;
  Parser::skip_whitespace();
  if (Parser::consume("dynamic")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_dynamic_tile;
    clause->parameters.param = NULL;
  }
  else if (Parser::consume("static")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_static_tile;
    Parser::skip_whitespace();
    if (!Parser::consume(',')) return false;
    Parser::skip_whitespace();
    if (!Parser::parse<SgExpression *>(clause->parameters.param)) return false;
  }
#ifdef TILEK_THREADS
  else if (Parser::consume("thread")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_thread_tile;
    clause->parameters.param = NULL;
  }
#endif
#ifdef TILEK_ACCELERATOR
  else if (Parser::consume("gang")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_gang_tile;
    Parser::skip_whitespace();
    if (!Parser::consume(',')) return false;
    Parser::skip_whitespace();
    if (!Parser::parse<SgExpression *>(clause->parameters.param)) return false;
  }
  else if (Parser::consume("worker")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_worker_tile;
    Parser::skip_whitespace();
    if (!Parser::consume(',')) return false;
    Parser::skip_whitespace();
    if (!Parser::parse<SgExpression *>(clause->parameters.param)) return false;
  }
#endif
  else return false;

  Parser::skip_whitespace();
  if (!Parser::consume(')')) return false;

  return true;
}

#ifdef TILEK_THREADS
template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_num_threads>(
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_num_threads> * clause
) {
  return Parser::one<SgExpression *>(clause->parameters.num_threads, '(', ')');
}
#endif

#ifdef TILEK_ACCELERATOR
template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_num_gangs>(
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_num_gangs> * clause
) {
  if (!Parser::one<size_t>(clause->parameters.gang_id, '[', ']'))
    clause->parameters.gang_id = 0;
  return Parser::one<SgExpression *>(clause->parameters.num_gangs, '(', ')');
}

template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_num_workers>(
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_num_workers> * clause
) {
  if (!Parser::one<size_t>(clause->parameters.worker_id, '[', ']'))
    clause->parameters.worker_id = 0;
  return Parser::one<SgExpression *>(clause->parameters.num_workers, '(', ')');
}
#endif

void lookup_region_successors(
  SgStatement * region,
  Directives::directive_t<TileK::language_t> * directive,
  const std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *> & translation_map
) {
  if (isSgPragmaDeclaration(region)) {
    std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *>::const_iterator it_region = translation_map.find(region);
    assert(it_region != translation_map.end());
    directive->add_successor(TileK::language_t::e_child_scope, it_region->second);
    it_region->second->add_predecessor(TileK::language_t::e_parent_scope, directive);
  }
  else {
    std::queue<SgStatement *> nodes;
    nodes.push(region);
    while (!nodes.empty()) {
      SgStatement * node = nodes.front();
      nodes.pop();

      if (node == NULL) continue;

      switch (node->variantT()) {
        case V_SgDoWhileStmt:
          /// \todo do-while-loop condition: Can it contain directives? It could call functions known to use TileK.
          nodes.push(((SgDoWhileStmt *)node)->get_body());
          break;
        case V_SgForStatement:
          /// \todo for-loop initialization, condition, increment: Can they contain directives? They could call functions known to use TileK.
          nodes.push(((SgForStatement *)node)->get_loop_body());
          break;
        case V_SgIfStmt:
          /// \todo if-stmt condition: Can it contain directives? It could call functions known to use TileK.
          nodes.push(((SgIfStmt *)node)->get_true_body());
          nodes.push(((SgIfStmt *)node)->get_false_body());
          break;
        case V_SgSwitchStatement:
          /// \todo switch-stmt item selector: Can it contain directives? It could call functions known to use TileK.
          nodes.push(((SgSwitchStatement *)node)->get_body());
          break;
        case V_SgWhileStmt:
          /// \todo while-loop condition: Can it contain directives? It could call functions known to use TileK.
          nodes.push(((SgWhileStmt *)node)->get_body());
          break;
        case V_SgBasicBlock:
        {
          SgBasicBlock * bb_stmt = (SgBasicBlock *)node;
          std::vector<SgStatement *>::const_iterator it_stmt;
          for (it_stmt = bb_stmt->get_statements().begin();  it_stmt != bb_stmt->get_statements().end(); it_stmt++) {
            std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *>::const_iterator it_stmt_directive = translation_map.find(*it_stmt);
            if (it_stmt_directive != translation_map.end()) {
              directive->add_successor(TileK::language_t::e_child_scope, it_stmt_directive->second);
              it_stmt_directive->second->add_predecessor(TileK::language_t::e_parent_scope, directive);

              // Exclude statements scoped under data/parallel/kernel/loop constructs
              //   do-loop handles succession of directives scoped one under the other, for example:
              //             sg_scope->{acc_data, acc_parallel, acc_loop, sg_for->{...}, sg_stmt, ... }
              //     -> find acc_data
              //     -> ignore acc_parallel, acc_loop and sg_for
              //     -> restart with sg_stmt
              do {
                if (
                  it_stmt_directive->second->construct->kind == TileK::language_t::e_construct_kernel   ||
                  it_stmt_directive->second->construct->kind == TileK::language_t::e_construct_loop
                ) {
                  it_stmt++;

                  if (it_stmt == bb_stmt->get_statements().end())
                    break;

                  it_stmt_directive = translation_map.find(*it_stmt);
                }
                else break;
              } while (it_stmt != bb_stmt->get_statements().end() && it_stmt_directive != translation_map.end());
            }
            else nodes.push(*it_stmt);
          }
          break;
        }
        case V_SgPragmaDeclaration:
          assert(false); /// \todo Non TileK pragma would not be in 'translation_map' and trigger this assertion
        case V_SgExprStatement:
          break; /// \todo might be a call-site for a function known to use TileK
        case V_SgVariableDeclaration:
          break; /// \todo might be a call-site for a function known to use TileK
        default:
          assert(false);
      }
    }
  }
}

void lookup_loop_successors(
  SgForStatement * for_loop,
  Directives::directive_t<TileK::language_t> * directive,
  const std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *> & translation_map
) {
  assert(for_loop != NULL);

  std::queue<SgStatement *> nodes;
  nodes.push(for_loop->get_loop_body());
  while (!nodes.empty()) {
    SgStatement * node = nodes.front();
    nodes.pop();

    if (node == NULL) continue;

    switch (node->variantT()) {
      case V_SgForStatement:
        /// \todo for-loop initialization, condition, increment: Can they contain directives? They could call functions known to use TileK.
        nodes.push(((SgForStatement *)node)->get_loop_body());
        break;
      case V_SgIfStmt:
        /// \todo if-stmt condition: Can it contain directives? It could call functions known to use TileK.
        nodes.push(((SgIfStmt *)node)->get_true_body());
        nodes.push(((SgIfStmt *)node)->get_false_body());
        break;
      case V_SgBasicBlock:
      {
        SgBasicBlock * bb_stmt = (SgBasicBlock *)node;
        std::vector<SgStatement *>::const_iterator it_stmt;
        for (it_stmt = bb_stmt->get_statements().begin();  it_stmt != bb_stmt->get_statements().end(); it_stmt++) {
          std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *>::const_iterator it_stmt_directive = translation_map.find(*it_stmt);
          if (it_stmt_directive != translation_map.end()) {
            assert(it_stmt_directive->second->construct->kind == TileK::language_t::e_construct_loop);
            directive->add_successor(TileK::language_t::e_child_scope, it_stmt_directive->second);
            it_stmt_directive->second->add_predecessor(TileK::language_t::e_parent_scope, directive);
            it_stmt++;
          }
          else nodes.push(*it_stmt);
        }
        break;
      }
      case V_SgExprStatement:
        break; /// \todo might be a call-site for a user-defined function that has to be inlined 
      case V_SgVariableDeclaration:
        break; /// \todo might be a call-site for a user-defined function that has to be inlined 
      case V_SgPragmaDeclaration:
        assert(false); /// \todo Non TileK pragma would not be in 'translation_map' and trigger this assertion
      case V_SgDoWhileStmt:
      case V_SgSwitchStatement:
      case V_SgWhileStmt:
        assert(false); /// \todo not supported inside parallel/kernel region => cannot be scoped under loop construct.
      default:
        assert(false);
    }
  }
}

template <>
bool Frontend<TileK::language_t>::build_graph() {
  std::vector<Directives::directive_t<TileK::language_t> *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    switch ((*it_directive)->construct->kind) {
      case TileK::language_t::e_construct_dataenv:
      {
        Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_dataenv> * construct =
                 (Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_dataenv> *)((*it_directive)->construct);
        lookup_region_successors(construct->assoc_nodes.dataenv_region, *it_directive, translation_map);
        break;
      }
      case TileK::language_t::e_construct_kernel:
      {
        Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_kernel> * construct =
                 (Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_kernel> *)((*it_directive)->construct);
        lookup_region_successors(construct->assoc_nodes.kernel_region, *it_directive, translation_map);
        break;
      }
      case TileK::language_t::e_construct_loop:
      {
        Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_loop> * construct =
                 (Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_loop> *)((*it_directive)->construct);
        lookup_loop_successors(construct->assoc_nodes.for_loop, *it_directive, translation_map);
        break;
      }
      default:
        assert(false);
    }
  }

  graph_entry.clear();
  graph_final.clear();

  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    if ((*it_directive)->predecessor_list.empty())
      graph_entry.push_back(*it_directive);
    if ((*it_directive)->successor_list.empty())
      graph_final.push_back(*it_directive);
  }

  return true;
}

}

