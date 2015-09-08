
#ifndef __DLX_FRONTEND_PARSER_HPP__
#define __DLX_FRONTEND_PARSER_HPP__

#include "DLX/Core/directives.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/constructs.hpp"

#include <boost/filesystem.hpp>

#include <vector>
#include <utility>
#include <string>

namespace DLX {

// Builtin types for clause's arguments

struct section_t {
  SgExpression * lower_bound;
  SgExpression * size;
  SgExpression * stride;
};

typedef std::vector<section_t> section_list_t;

typedef std::pair<SgVariableSymbol *, section_list_t> data_sections_t;

/// Parsing using AstFromString (TODO get/set should lock/unlock the Parser)
class Parser {
  private:
    char * cstr;

  public:
    Parser();
    virtual ~Parser();

    void set(std::string & str, SgNode * node);
    void get(std::string & str);

  public:
    static void skip_whitespace();
    static void skip_parenthesis();

    static bool consume(const char);
    static bool consume(const std::string &);

    template <typename A>
    static bool parse(A &);

    template <typename A>
    static bool one(A & singleton, char start, char stop) {
      skip_whitespace();
      if (!consume(start)) return false;
      skip_whitespace();
      if (!parse<A>(singleton)) return false;
      skip_whitespace();
      return consume(stop);
    }

    template <typename A, typename B>
    static bool pair(std::pair<A, B> & pair, char start, char stop, char sep) {
      skip_whitespace();
      if (!consume(start)) return false;
      skip_whitespace();
      if (!parse<A>(pair.first)) return false;
      skip_whitespace();
      if (!consume(sep)) return false;
      skip_whitespace();
      if (!parse<B>(pair.second)) return false;
      skip_whitespace();
      return consume(stop);
    }

    template <typename A>
    static bool list(std::vector<A> & vect, char start, char stop, char sep) {
      skip_whitespace();

      if (!consume(start)) return false;

      skip_whitespace();
      do {
        A a;
        if (!parse<A>(a)) {
          if (sep == '\0')
            break;
          else
            return false;
        }
        vect.push_back(a);
        skip_whitespace();
      } while (consume(sep));

      return consume(stop);
    }
};

template <> bool Parser::parse<size_t>(size_t &);

template <> bool Parser::parse<SgExpression *>(SgExpression * & expr);

template <> bool Parser::parse<SgValueExp *>(SgValueExp * & expr);

template <> bool Parser::parse<std::string>(std::string & str);

template <> bool Parser::parse<std::pair<SgExpression *, SgExpression *> >(std::pair<SgExpression *, SgExpression *> &);

template <> bool Parser::parse<SgVariableSymbol *>(SgVariableSymbol * &);

template <> bool Parser::parse<data_sections_t>(data_sections_t &);

template <> bool Parser::parse<section_list_t>(section_list_t &);

template <> bool Parser::parse<section_t>(section_t &);

template <> bool Parser::parse<boost::filesystem::path>(boost::filesystem::path &);

}

#endif /* __DLX_FRONTEND_PARSER_HPP__ */

