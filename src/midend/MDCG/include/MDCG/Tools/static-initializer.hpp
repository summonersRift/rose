
#ifndef __MDCG_TOOLS_STATIC_INITIALIZER_HPP__
#define __MDCG_TOOLS_STATIC_INITIALIZER_HPP__

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include "MDCG/Model/model.hpp"

#include <string>
#include <sstream>
#include <iterator>

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace MDCG {

namespace Tools {

class StaticInitializer {
  public:
    static inline SgVariableSymbol * instantiateDeclaration(
      MFB::Driver<MFB::Sage> & driver,
      std::string decl_name,
      SgScopeStatement * scope,
      size_t file_id,
      SgType * type,
      SgInitializer * init,
      bool prepend
    ) {
      MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name, type, init, scope, NULL, file_id, false, false, prepend);
      MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = driver.build<SgVariableDeclaration>(var_decl_desc);

      return var_decl_res.symbol;
    }

    template <class ModelTraversal>
    static SgInitializer * createInitializer(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      const typename ModelTraversal::input_t & input,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend
    );

    template <class ModelTraversal>
    static SgVariableSymbol * addDeclaration(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      const typename ModelTraversal::input_t & input,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend,
      std::string decl_name
    );

    template <class ModelTraversal>
    static SgExpression * createPointer(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      const typename ModelTraversal::input_t & input,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend,
      const std::string & decl_name
    );

    template <class ModelTraversal, class Iterator>
    static SgAggregateInitializer * createArray(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      Iterator input_begin, Iterator input_end,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend
    );

    template <class ModelTraversal, class Iterator>
    static SgVariableSymbol * addArrayDeclaration(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      size_t num_element,
      Iterator input_begin,
      Iterator input_end,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend,
      std::string decl_name
    );

    template <class ModelTraversal, class Iterator>
    static SgExpression * createArrayPointer(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      size_t num_element, Iterator input_begin, Iterator input_end,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend,
      const std::string & decl_name
    );



    template <class ModelTraversal, class Iterator>
    static SgAggregateInitializer * createPointerArray(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      Iterator input_begin, Iterator input_end,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend,
      const std::string & decl_prefix
    );

    template <class ModelTraversal, class Iterator>
    static SgVariableSymbol * addPointerArrayDeclaration(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      size_t num_element,
      Iterator input_begin,
      Iterator input_end,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend,
      std::string decl_name,
      const std::string & sub_decl_prefix
    );

    template <class ModelTraversal, class Iterator>
    static SgExpression * createPointerArrayPointer(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      size_t num_element, Iterator input_begin, Iterator input_end,
      SgScopeStatement * scope,
      MFB::file_id_t file_id,
      bool prepend,
      const std::string & decl_name,
      const std::string & sub_decl_prefix
    );
};

template <class ModelTraversal>
SgInitializer * StaticInitializer::createInitializer(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  const typename ModelTraversal::input_t & input,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

#if VERBOSE
  std::cerr << "[Info] (MDCG::Tools::StaticInitializer::createInitializer) Calls driver.useSymbol<SgClassDeclaration> for symbol: " << element->node->symbol->get_name().getString() << std::endl;
#endif

  driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  if (element->scope->field_children.size() > 0) {
    std::vector<Model::field_t>::const_iterator it_field = element->scope->field_children.begin();
    SgExpression * expr = ModelTraversal::createFieldInitializer(driver, *it_field, 0, input, scope, file_id, prepend);
    if (expr == NULL) return NULL;
    expr_list->append_expression(expr);
    it_field++;
    size_t field_id = 1;
    for (; it_field != element->scope->field_children.end(); it_field++) {
      expr = ModelTraversal::createFieldInitializer(driver, *it_field, field_id++, input, scope, file_id, prepend);
      assert(expr != NULL);
      expr_list->append_expression(expr);
    }
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal>
SgVariableSymbol * StaticInitializer::addDeclaration(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  const typename ModelTraversal::input_t & input,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend,
  std::string decl_name
) {
  SgInitializer * initializer = createInitializer<ModelTraversal>(driver, element, input, scope, file_id, prepend);
  assert(initializer != NULL);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(driver, decl_name, scope, file_id, type, initializer, prepend);
  assert(symbol != NULL);
      
  return symbol;
}

template <class ModelTraversal>
SgExpression * StaticInitializer::createPointer(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  const typename ModelTraversal::input_t & input,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend,
  const std::string & decl_name
) {
  return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(addDeclaration<ModelTraversal>(driver, element, input, scope, file_id, prepend, decl_name)));
}

template <class ModelTraversal, class Iterator>
SgAggregateInitializer * StaticInitializer::createArray(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  Iterator input_begin,
  Iterator input_end,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

#if VERBOSE
  std::cerr << "[Info] (MDCG::Tools::StaticInitializer::createArray) Calls driver.useSymbol<SgClassDeclaration> for symbol: " << element->node->symbol->get_name().getString() << std::endl;
#endif

  driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  Iterator it;
  for (it = input_begin; it != input_end; it++) {
    SgExpression * expr = createInitializer<ModelTraversal>(driver, element, *it, scope, file_id, prepend);
    if (expr != NULL)
      expr_list->append_expression(expr);
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal, class Iterator>
SgVariableSymbol * StaticInitializer::addArrayDeclaration(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  size_t num_element,
  Iterator input_begin,
  Iterator input_end,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend,
  std::string decl_name
) {
  SgInitializer * initializer = createArray<ModelTraversal, Iterator>(driver, element, input_begin, input_end, scope, file_id, prepend);
  assert(initializer != NULL);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);
  type = SageBuilder::buildArrayType(type, SageBuilder::buildUnsignedLongVal(num_element));
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(driver, decl_name, scope, file_id, type, initializer, prepend);
  assert(symbol != NULL);
      
  return symbol;
}

template <class ModelTraversal, class Iterator>
SgExpression * StaticInitializer::createArrayPointer(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  size_t num_element,
  Iterator input_begin,
  Iterator input_end,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend,
  const std::string & decl_name
) {
  return SageBuilder::buildVarRefExp(addArrayDeclaration<ModelTraversal, Iterator>(driver, element, num_element, input_begin, input_end, scope, file_id, prepend, decl_name));
}

template <class ModelTraversal, class Iterator>
SgAggregateInitializer * StaticInitializer::createPointerArray(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  Iterator input_begin,
  Iterator input_end,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend,
  const std::string & decl_prefix
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

#if VERBOSE
  std::cerr << "[Info] (MDCG::Tools::StaticInitializer::createPointerArray) Calls driver.useSymbol<SgClassDeclaration> for symbol: " << element->node->symbol->get_name().getString() << std::endl;
#endif

  driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  size_t cnt = 0;

  Iterator it;
  for (it = input_begin; it != input_end; it++) {
    std::ostringstream decl_name;
      decl_name << decl_prefix << "_" << cnt++;
    SgExpression * expr = createPointer<ModelTraversal>(driver, element, *it, scope, scope, file_id, prepend, decl_name.str());
    if (expr != NULL)
      expr_list->append_expression(expr);
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal, class Iterator>
SgVariableSymbol * StaticInitializer::addPointerArrayDeclaration(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  size_t num_element,
  Iterator input_begin,
  Iterator input_end,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend,
  std::string decl_name,
  const std::string & sub_decl_prefix
) {
  SgInitializer * initializer = createPointerArray<ModelTraversal, Iterator>(driver, element, input_begin, input_end, scope, file_id, prepend, sub_decl_prefix);
  assert(initializer != NULL);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);
  type = SageBuilder::buildArrayType(type, SageBuilder::buildUnsignedLongVal(num_element));
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(driver, decl_name, scope, file_id, type, initializer, prepend);
  assert(symbol != NULL);
      
  return symbol;
}

template <class ModelTraversal, class Iterator>
SgExpression * StaticInitializer::createPointerArrayPointer(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  size_t num_element,
  Iterator input_begin,
  Iterator input_end,
  SgScopeStatement * scope,
  MFB::file_id_t file_id,
  bool prepend,
  const std::string & decl_name,
  const std::string & sub_decl_prefix
) {
  return SageBuilder::buildVarRefExp(addPointerArrayDeclaration<ModelTraversal, Iterator>(driver, element, num_element, input_begin, input_end, scope, file_id, prepend, decl_name, sub_decl_prefix));
}

}

}

#endif /* __MDCG_TOOLS_STATIC_INITIALIZER_HPP__ */

