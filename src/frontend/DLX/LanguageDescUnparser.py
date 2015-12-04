from UnparserBase import DLXUnparser
import LanguageDesc

# The class unparses the (specialized) language.hpp file
class HeaderUnparser(DLXUnparser):
    def __init__(self, languageDesc):
        DLXUnparser.__init__(self, languageDesc)
        self.DLXIncludes_ = ["directives.hpp", "constructs.hpp", "clauses.hpp", "parser.hpp", "frontend.hpp", "compiler.hpp"]

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
        langTmplStr = self.langTmplSpecialization()
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
        genConsTmplParam = self.langTmplSpecialization()
        structName = self.templatize(self.genCons_t_, genConsTmplParam) + "::" + self.assocNodes_t_
        for co in self.lD_.constructs_:
            anTmplParam = self.langTmplSpecialization() + "::" + co
            anTmplStr = self.templatize(structName, anTmplParam)
            out += self.stmt(self.struct(anTmplStr, "", 2))
       
        return out

    def generateParametersTTemplates(self):
        out = ""
        genClauseTmplParam = self.langTmplSpecialization()
        structName = self.templatize(self.genClause_t_, genClauseTmplParam) + "::" + self.params_t_
        for cl in self.lD_.clauses_:
            ptTmplParam = self.langTmplSpecialization() + "::" + cl
            ptTmplStr = self.templatize(structName, ptTmplParam)
            out += self.stmt(self.struct(ptTmplStr, "", 2))

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

        directivesNSBody += self.generateParametersTTemplates()

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


class SourceUnparser(DLXUnparser):
    def __init__(self, languageDesc):
        DLXUnparser.__init__(self, languageDesc)
        self.DLXIncludes_ = ["directives.hpp", "constructs.hpp", "clauses.hpp", "parser.hpp", "frontend.hpp", "compiler.hpp"]

    def buildClauseFunctionBody(self):
        out = ""
        out += "switch(kind) {"
        for cl in self.lD_.clauses_:
            tStr = ""
            tStr += self.case(self.langTmplSpecialization() + "::" + cl) + "\n"
            templateStr = self.lD_.name_ +"::"+ self.lang_t_ +","+ self.lD_.name_ +"::"+ self.lang_t_ +"::"+ cl
            tStr += self.stmt("return new " + self.templatize(self.clause_t, templateStr) + "()")
            out += tStr
        out += "default: assert(false);"

        out += "}"

        return out

    def generateBuildClauseSource(self):
        out = ""
        langTmplStr = self.langTmplSpecialization()
        tStr = self.tmpSpcz_ + " " + self.templatize(self.genClause_t_, langTmplStr)
        tStr += " * " + self.templatize("buildClause", langTmplStr)
        tStr += "(" + langTmplStr + "::" + self.clauseKinds_e_ + " kind)"

        body = ""
        body += self.buildClauseFunctionBody()

        out += tStr + "{\n" + body + "}\n"
        return out

    def generateFrontendBody(self):
        out = ""
        out += "switch (" + "clause->kind" + ") {"
        for cl in self.lD_.clauses_:
            caseStr = self.case(self.langTmplSpecialization() + "::" + cl)
            retStr = "return " + self.templatize(self.frontend_, self.langTmplSpecialization()) +"::"
            retStr += self.templatize("parseClauseParameters", self.langTmplSpecialization() +"::"+ cl)
            retStr += "(" + self.directives_ +"::" 
            tmplArg = self.langTmplSpecialization() +","+ self.langTmplSpecialization() +"::"+cl 
            retStr += self.templatize(self.clause_t, tmplArg) + " * " + ")" + "clause" + ")"
            out += caseStr + self.stmt(retStr)
        out += self.stmt("default: assert(false)") + "\n}\n"
        return out

    def generateBuildFrontendSource(self):
        out = ""
        templStr = self.tmpSpcz_ + "bool " + self.templatize(self.frontend_, self.langTmplSpecialization())
        templStr += "::" + "parseClauseParameters_tpl" + "("
        templStr += self.templatize(self.directives_ +"::"+self.genClause_t_, self.langTmplSpecialization())
        out += templStr + " * " + "clause )"
        out += "{\n" + self.generateFrontendBody() + "\n}\n"

        return out

    def buildClauses(self):
        out = ""
        out += self.userInclude("sage3basic.h")
        for incl in self.DLXIncludes_:
            out += self.userInclude("DLX/Core/" + incl) + "\n"
        out += self.sysInclude("cassert")

        out += self.stmt("class SgLocatedNode")

        dlxNS = ""
        dlxNS += self.generateBuildClauseSource()

        directivesNS = self.namespace("DLX", dlxNS)
        out += self.namespace("Directives", directivesNS)
        out += "\n"
        out += self.generateBuildFrontendSource()

        return out


    def unparse(self):
        clausesStr = ""
        clausesStr += self.buildClauses()
        print(clausesStr)
        return clausesStr
