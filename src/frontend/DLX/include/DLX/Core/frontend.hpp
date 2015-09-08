
#ifndef __DLX_FRONTEND_HPP__
#define __DLX_FRONTEND_HPP__

#include <vector>
#include <string>

#include "DLX/Core/directives.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/constructs.hpp"

#include "DLX/Core/parser.hpp"

namespace DLX {

template <class language_tpl>
class Frontend {
  public:
    typedef language_tpl language_t;

    typedef Directives::directive_t<language_t> directive_t;
    typedef std::vector<directive_t *> directives_ptr_set_t;

    typedef Directives::generic_construct_t<language_t> generic_construct_t;
    typedef Directives::generic_clause_t<language_t> generic_clause_t;

  public:
    directives_ptr_set_t directives;
    directives_ptr_set_t graph_entry;
    directives_ptr_set_t graph_final;

    directive_t * getDirective(SgLocatedNode *) const;
    SgLocatedNode * getNode(directive_t * directive) const;

  protected:
    Parser parser;

    std::map<SgLocatedNode *, directive_t *> translation_map;
    std::map<directive_t *, SgLocatedNode *> rtranslation_map;

    bool findAssociatedNodes_tpl(SgLocatedNode * node, Directives::generic_construct_t<language_t> * construct);
    template <typename language_t::construct_kinds_e kind>
    bool findAssociatedNodes(SgLocatedNode * node, Directives::construct_t<language_t, kind> * construct);

    bool parseClauseParameters_tpl(Directives::generic_clause_t<language_t> * clause);
    template <typename language_t::clause_kinds_e kind>
    bool parseClauseParameters(Directives::clause_t<language_t, kind> * clause);

    generic_construct_t * parseConstruct();
    generic_clause_t    * parseClause();
    directive_t         * parse(std::string & directive_str, SgNode * directive_node);

    bool build_graph();

  public:
    bool parseDirectives(SgNode * node);

    void toGraphViz(std::ostream & out) const;
};

template <class language_tpl>
typename Frontend<language_tpl>::generic_construct_t * Frontend<language_tpl>::parseConstruct() {
  assert(language_t::s_construct_labels.size() > 0);

  typename language_t::construct_label_map_t::const_iterator it_construct;
  for (it_construct = language_t::s_construct_labels.begin(); it_construct != language_t::s_construct_labels.end(); it_construct++)
    if (Parser::consume(it_construct->second))
      return Directives::buildConstruct<language_tpl>(it_construct->first);
  return Directives::buildConstruct<language_tpl>(language_tpl::s_blank_construct);
}

template <class language_tpl>
typename Frontend<language_tpl>::generic_clause_t * Frontend<language_tpl>::parseClause() {
  assert(language_t::s_clause_labels.size() > 0);

  typename language_t::clause_labels_map_t::const_iterator it_clause;
  for (it_clause = language_t::s_clause_labels.begin(); it_clause != language_t::s_clause_labels.end(); it_clause++) {
    const typename language_t::label_set_t & labels = it_clause->second;
    typename language_t::label_set_t::const_iterator it_label;
    for (it_label = labels.begin(); it_label != labels.end(); it_label++)
      if (Parser::consume(*it_label))
        return Directives::buildClause<language_tpl>(it_clause->first);
  }
 
  return NULL;
}

template <class language_tpl>
typename Frontend<language_tpl>::directive_t * Frontend<language_tpl>::parse(std::string & directive_str, SgNode * directive_node) {
  parser.set(directive_str, directive_node);

  Parser::skip_whitespace();

  if (!Parser::consume(language_t::language_label)) {
    parser.get(directive_str);
    return NULL;
  }

  Parser::skip_whitespace();

  directive_t * directive = new directive_t();

  directive->construct = parseConstruct();
  assert(directive->construct != NULL);

  Parser::skip_whitespace();

  generic_clause_t * clause = NULL;
  while ((clause = parseClause()) != NULL) {

    bool res = parseClauseParameters_tpl(clause);
    if (!res) {
      std::string current_str;
      parser.get(current_str);
      std::cerr << "[Error] (DLX::Frontend<" << language_t::language_label << ">::directive_t) parseClauseParameters return false! Stopped with \"" << current_str << "\"." << std::endl;
    }
    assert(res);

    directive->clause_list.push_back(clause);

    Parser::skip_whitespace();
  }

  parser.get(directive_str);
  return directive;
}

template <class language_tpl>
typename Frontend<language_tpl>::directive_t * Frontend<language_tpl>::getDirective(SgLocatedNode * node) const {
  typename std::map<SgLocatedNode *, directive_t *>::const_iterator it_directive = translation_map.find(node);
  if (it_directive != translation_map.end()) return it_directive->second;
  else return NULL;
}

template <class language_tpl>
SgLocatedNode * Frontend<language_tpl>::getNode(directive_t * directive) const {
  typename std::map<directive_t *, SgLocatedNode *>::const_iterator it_rdirective = rtranslation_map.find(directive);
  if (it_rdirective != rtranslation_map.end()) return it_rdirective->second;
  else return NULL;
}

template <class language_tpl>
bool Frontend<language_tpl>::parseDirectives(SgNode * node) {
  // FIXME C/C++ only

  std::vector<SgPragmaDeclaration *> pragma_decls = SageInterface::querySubTree<SgPragmaDeclaration>(node);
  std::vector<SgPragmaDeclaration *>::iterator it_pragma_decl;
  for (it_pragma_decl = pragma_decls.begin(); it_pragma_decl != pragma_decls.end(); it_pragma_decl++) {
    SgPragmaDeclaration * pragma_decl = *it_pragma_decl;
    assert(pragma_decl != NULL);

//  TODO : SageInterface::replaceMacroCallsWithExpandedStrings(pragma_decl);

    typename std::map<SgLocatedNode *, directive_t *>::iterator it_directive = translation_map.find(pragma_decl);
    if (it_directive == translation_map.end()) {
      std::string directive_string = pragma_decl->get_pragma()->get_pragma();
      directive_t * directive = parse(directive_string, pragma_decl);
      if (directive != NULL) {
        translation_map.insert(std::pair<SgLocatedNode *, directive_t *>(pragma_decl, directive));
        rtranslation_map.insert(std::pair<directive_t *, SgLocatedNode *>(directive, pragma_decl));
      }
    }
  }

  bool res;
  typename std::map<SgLocatedNode *, directive_t *>::const_iterator it;
  for (it = translation_map.begin(); it != translation_map.end(); it++) {
    res = findAssociatedNodes_tpl(it->first, it->second->construct);
    assert(res);
    directives.push_back(it->second);
  }

  res = build_graph();
  assert(res);
 
  return true;
}

template <class language_tpl>
void Frontend<language_tpl>::toGraphViz(std::ostream & out) const {
  out << "digraph directives {" << std::endl;
  typename std::vector<directive_t *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    typename std::map<typename language_tpl::construct_kinds_e, std::string>::const_iterator it_construct_label = language_tpl::s_construct_labels.find((*it_directive)->construct->kind);
    assert(it_construct_label != language_tpl::s_construct_labels.end());
    out << "directive_" << *it_directive << " [label=\"" << it_construct_label->second << " ( " << *it_directive << " )\"];" << std::endl;

    typename std::vector<std::pair<typename language_tpl::directives_relation_e, directive_t *> >::const_iterator it_successor;
    for (it_successor = (*it_directive)->successor_list.begin(); it_successor != (*it_directive)->successor_list.end(); it_successor++) {
      typename std::map<typename language_tpl::directives_relation_e, std::string>::const_iterator it_rel_label = language_tpl::s_directives_relation_labels.find(it_successor->first);
      assert(it_rel_label != language_tpl::s_directives_relation_labels.end());
      out << "directive_" << *it_directive << " -> " << "directive_" << it_successor->second << " [label=\"" << it_rel_label->second << "\"];" << std::endl;
    }
  }
  out << "}" << std::endl;
}

}

#endif /* __DLX_FRONTEND_HPP__ */

