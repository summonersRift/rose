/*!
 * 
 * \file lib/core/parser.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "sage3basic.h"

#include "DLX/Core/parser.hpp"
#include <cstring>

namespace DLX {

inline bool whitespace(const char & c) {
  return (c == ' ');
}

Parser::Parser() : cstr(NULL) {}

Parser::~Parser() {
  if (cstr != NULL) {
    AstFromString::c_char = NULL;
    delete [] cstr;
    cstr = NULL;
  }
}

void Parser::get(std::string & str) {
  assert(cstr != NULL);
  assert(AstFromString::c_char != NULL);

  str = std::string(AstFromString::c_char);

  AstFromString::c_char = NULL;
  AstFromString::c_sgnode = NULL;
  delete [] cstr;
  cstr = NULL;
}

void Parser::set(std::string & str, SgNode * node) {
  if (cstr != NULL) {
    delete [] cstr;
    cstr = NULL;
    AstFromString::c_char = NULL;
    AstFromString::c_sgnode = NULL;
  }
  assert(AstFromString::c_char == NULL);

  cstr = new char [str.length()+1];
  std::strcpy(cstr, str.c_str());

  AstFromString::c_char = cstr;
  AstFromString::c_sgnode = node;
}

/// Parsing

void Parser::skip_whitespace() {
  AstFromString::afs_skip_whitespace();
}

void Parser::skip_parenthesis() {
  std::string str(AstFromString::c_char);

  assert(str[0] == '(');
  size_t count = 1;
  size_t depth = 1;
  while (depth > 0) {
    if (str[count] == '(') depth++;
    else if (str[count] == ')') depth--;
    count++;
    assert(count <= str.size());
  }
  AstFromString::c_char += count;
}

bool Parser::consume(const char c) {
  if (c == '\0') return true;
  if (AstFromString::c_char[0] == c) {
    AstFromString::c_char++;
    return true;
  }
  return false;
}

bool Parser::consume(const std::string & label) {
  std::string str(AstFromString::c_char);
  if (str.find(label) == 0) {
    AstFromString::c_char += label.length();
    return true;
  }
  else return false;
}

template <>
bool Parser::parse<size_t>(size_t & val) {
  skip_whitespace();
  int val_;
  if (!AstFromString::afs_match_integer_const(&val_)) 
    return false;
  skip_whitespace();
  val = val_;

  return true;
}

template <>
bool Parser::parse<SgExpression *>(SgExpression * & expr) {
  skip_whitespace();
  if (!AstFromString::afs_match_assignment_expression()) // AstFromString::afs_match_expression would match expression list too
     return false;
  skip_whitespace();

  expr = isSgExpression(AstFromString::c_parsed_node);
  return expr != NULL;
}

template <>
bool Parser::parse<SgValueExp *>(SgValueExp * & expr) {
  skip_whitespace();
  if (!AstFromString::afs_match_constant())
     return false;
  skip_whitespace();

  expr = isSgValueExp(AstFromString::c_parsed_node);
  return expr != NULL;
}

template <>
bool Parser::parse<std::string>(std::string & str) {
  if (!consume('"')) return false;

  const char * old_c_char = AstFromString::c_char;

  while (AstFromString::c_char[0] != '\0' && !consume('"')) {
    str += AstFromString::c_char[0];
    AstFromString::c_char++;
  }

  if (AstFromString::c_char[0] == '\0') {
    AstFromString::c_char = old_c_char;
    return false;
  }

  return true;
}

template <>
bool Parser::parse<boost::filesystem::path>(boost::filesystem::path & file) {

  if (!consume('"')) return false;

  const char * old_c_char = AstFromString::c_char;
  while (AstFromString::c_char[0] != '\0' && !consume('"')) {
    AstFromString::c_char++;
  }

  if (AstFromString::c_char[0] == '\0') {
    AstFromString::c_char = old_c_char;
    return false;
  }

  file = boost::filesystem::path(old_c_char, AstFromString::c_char);

  return true;
}

template <>
bool Parser::parse<std::pair<SgExpression *, SgExpression *> >(std::pair<SgExpression *, SgExpression *> & pair) {
  skip_whitespace();
  if (!parse<SgExpression *>(pair.first)) return false;
  skip_whitespace();
  if (!consume(',')) return false;
  skip_whitespace();
  if (!parse<SgExpression *>(pair.second)) return false;
  return true;
}
 
template <>
bool Parser::parse<SgVariableSymbol *>(SgVariableSymbol * & var_sym) {
  skip_whitespace();
  if (!AstFromString::afs_match_identifier())
    return false;
  skip_whitespace();
 
  SgVarRefExp * var_ref = isSgVarRefExp(AstFromString::c_parsed_node);
  if (var_ref == NULL)
    return false;
  var_sym = var_ref->get_symbol();

  return var_sym != NULL;
}

template <>
bool Parser::parse<int>(int & val) {
  skip_whitespace();
  if (!AstFromString::afs_match_integer_const(&val))
    return false;
  skip_whitespace();
 
  return true;
}

template <>
bool Parser::parse<data_sections_t>(data_sections_t & data_sections) {
  return pair<SgVariableSymbol *, section_list_t>(data_sections, '\0', '\0', '\0');
}
 
template <>
bool Parser::parse<section_list_t>(section_list_t & section_list) {
  return list<section_t>(section_list, '\0', '\0', '\0');
}
 
template <>
bool Parser::parse<section_t>(section_t & section) {
  SgExpression * expr_1 = NULL;
  SgExpression * expr_2 = NULL;
  SgExpression * expr_3 = NULL;

  if (!consume('[')) return false;

  if (!parse<SgExpression *>(expr_1)) return false;

  if (!consume(':')) {
    if (!consume(']')) return false;

    section.lower_bound = NULL;
    section.size = expr_1;
    section.stride = NULL;

    return true;
  }

  if (!parse<SgExpression *>(expr_2)) return false;

  if (!consume(':')) {
    if (!consume(']')) return false;

    section.lower_bound = expr_1;
    section.size = expr_2;
    section.stride = NULL;

    return true;
  }

  if (!parse<SgExpression *>(expr_3)) return false;

  if (!consume(']')) return false;

  section.lower_bound = expr_1;
  section.size = expr_2;
  section.stride = expr_3;

  return true;
}

}

