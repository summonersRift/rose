from UnparserBase import DLXUnparser
import LanguageDesc

# The class unparses the (specialized) language.hpp file
class HeaderUnparser(DLXUnparser):
    def __init__(self, languageDesc):
        self.DLXIncludes_ = ["directives.hpp", "constructs.hpp", "clauses.hpp", "parser.hpp", "frontend.hpp", "compiler.hpp"]
        self.lD_ = languageDesc
        self.lang_t = "language_t"
        self.tmpSpcz = "template <>"
        self.genCons_t = "generic_construct_t"
        self.genClause_t = "generic_clause_t"
        self.consKinds_e = "construct_kinds_e"
        self.clauseKinds_e = "clause_kinds_e"


    def includeGuard(self):
        return "__DLX_INSTANCE_GENERATED_" + self.lD_.name_.upper() + "_H\n"

    def generateLanguageStructDefinition(self):
        out = ""
        out += self.stmt(self.enum(self.consKinds_e, self.lD_.constructs_))
        out += "\n"
        out += self.stmt(self.enum("clause_kinds_e", self.lD_.clauses_))
        out += "\n"
        out += self.stmt(self.enum("relation_kinds_e", self.lD_.relations_))
        out += "\n"
        # TODO Do we need to generate the static data member at this point?
        return out

    def generateBuildTemplates(self):
        out = ""
        langTmplStr = self.lD_.name_ + "::" + self.lang_t
        tStr = self.tmpSpcz + " " + self.templatize(self.genCons_t, langTmplStr)
        tStr += " * " + self.templatize("buildConstruct", langTmplStr)
        tStr += "(" + langTmplStr + "::" + self.consKinds_e + " kind)"
        out += self.stmt(tStr) + "\n"
        tStr = self.tmpSpcz + " " + self.templatize(self.genClause_t, langTmplStr)
        tStr += " * " + self.templatize("buildClause", langTmplStr)
        tStr += "(" + langTmplStr + "::" + self.clauseKinds_e + " kind)"
        out += self.stmt(tStr) + "\n"
        return out

    def generateAssocNodesTemplates(self):
        out = ""
        
        return out


    def unparseDefinitions(self):
        out = ""
# we have: namespace lName { struct language_t { enum constructs, enum clauses, enum directives }
        # body gets embedded into the namespace
        langNSBody = ""
        structBody = self.generateLanguageStructDefinition()
        langNSBody += self.struct(self.lang_t, structBody, 0)
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
