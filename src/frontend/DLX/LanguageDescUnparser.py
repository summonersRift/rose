from UnparserBase import DLXUnparser
import LanguageDesc

# The class unparses the (specialized) language.hpp file
class HeaderUnparser(DLXUnparser):
    def __init__(self, languageDesc):
        self.DLXIncludes_ = ["directives.hpp", "constructs.hpp", "clauses.hpp", "parser.hpp", "frontend.hpp", "compiler.hpp"]
        self.lD_ = languageDesc
        self.lang_t_ = "language_t"
        self.tmpSpcz_ = "template <>"
        self.genCons_t_ = "generic_construct_t"
        self.genClause_t_ = "generic_clause_t"
        self.consKinds_e_ = "construct_kinds_e"
        self.clauseKinds_e_ = "clause_kinds_e"
        self.relKinds_e_= "relation_kinds_e"
        self.assocNodes_t_ = "assoc_nodes_t"


    def includeGuard(self):
        return "__DLX_INSTANCE_GENERATED_" + self.lD_.name_.upper() + "_H\n"

    def generateLanguageStructDefinition(self):
        out = ""
        out += self.stmt(self.enum(self.consKinds_e_, self.lD_.constructs_))
        out += "\n"
        out += self.stmt(self.enum(self.clauseKinds_e_, self.lD_.clauses_))
        out += "\n"
        out += self.stmt(self.enum(self.relKinds_e_, self.lD_.relations_))
        out += "\n"
        # TODO Do we need to generate the static data member at this point?
        return out

    def generateBuildTemplates(self):
        out = ""
        langTmplStr = self.lD_.name_ + "::" + self.lang_t_
        tStr = self.tmpSpcz_ + " " + self.templatize(self.genCons_t_, langTmplStr)
        tStr += " * " + self.templatize("buildConstruct", langTmplStr)
        tStr += "(" + langTmplStr + "::" + self.consKinds_e_ + " kind)"
        out += self.stmt(tStr) + "\n"
        tStr = self.tmpSpcz_ + " " + self.templatize(self.genClause_t_, langTmplStr)
        tStr += " * " + self.templatize("buildClause", langTmplStr)
        tStr += "(" + langTmplStr + "::" + self.clauseKinds_e_ + " kind)"
        out += self.stmt(tStr) + "\n\n"
        return out

    def generateAssocNodesTemplates(self):
        out = ""
        genConsTmplParam = self.lD_.name_ + "::" + self.lang_t_
        structName = self.templatize(self.genCons_t_, genConsTmplParam) + "::" + self.assocNodes_t_
        for co in self.lD_.constructs_:
            anTmplParam = self.lD_.name_ + "::" + self.lang_t_ + "::" + co
            anTmplStr = self.templatize(structName, anTmplParam)
            out += self.stmt(self.struct(anTmplStr, "", 2))
       
        return out


    def unparseDefinitions(self):
        out = ""
# we have: namespace lName { struct language_t { enum constructs, enum clauses, enum directives }
        # body gets embedded into the namespace
        langNSBody = ""
        structBody = self.generateLanguageStructDefinition()
        langNSBody += self.struct(self.lang_t_, structBody, 0)
        out += self.namespace(self.lD_.name_, langNSBody)

        directivesNSBody = ""
        directivesNSBody += self.generateBuildTemplates()

        directivesNSBody += self.generateAssocNodesTemplates()

        out += self.namespace("Directives", directivesNSBody)


        return "\n" + out + "\n"


    def unparse(self):
        out = ""
        out += "#ifndef " + self.includeGuard() + "#define " + self.includeGuard()
        out += "\n"
        for i in self.DLXIncludes_:
            out += self.userInclude(i) + "\n"

        for i in self.lD_.includes_:
            out += self.userInclude(i) + "\n"

        for t in self.lD_.types_:
            out += self.stmt(t);


        out += self.unparseDefinitions()

        out += "#endif\n"
        print(out)
