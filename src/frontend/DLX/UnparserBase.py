


# The base class for DLX unparsing. Mainly used for some defines, which could come in handy
# when unparsing DLX
class DLXUnparser:

    def __init__(self, languageDesc):
        self.lang_t_ = "language_t"
        self.tmpSpcz_ = "template <>\n"
        self.genCons_t_ = "generic_construct_t"
        self.genClause_t_ = "generic_clause_t"
        self.consKinds_e_ = "construct_kinds_e"
        self.clauseKinds_e_ = "clause_kinds_e"
        self.relKinds_e_= "relation_kinds_e"
        self.assocNodes_t_ = "assoc_nodes_t"
        self.params_t_ = "parameters_t"
        self.clause_t = "clause_t"
        self.lD_ = languageDesc
        self.frontend_ = "Frontend"
        self.directives_ = "Directives"

    def namespace(self, name, content):
        return "namespace " + name + " {\n" + content + "\n}"

    def typedef(self, a, b):
        return "typedef " + a + " " + b

    def stmt(self, content):
        return content + ";\n"

    def templatize(self, iden, tmplArg):
        return iden + "<" + tmplArg + ">"

    def langTmplSpecialization(self):
        return self.lD_.name_ + "::" + self.lang_t_

    def case(self, label):
        return "case " + label + ": \n"
    
    # XXX does it make sense to always return this as statement?
    def enum(self, name, values):
        s = "enum " + name + " {\n"
        for v in values:
            s += v + ",\n"
        s = s[:-2]
        s += "\n}"
        return s

    def struct(self, name, content, tmplSpecialization):
        s = ""
        for i in xrange(tmplSpecialization):
            s += "template<>\n"
        s += "struct " + name + " {\n" + content + "\n}"
        return s

    def userInclude(self, fileWithPath):
        return "#include \"" + fileWithPath + "\""

    def sysInclude(self, header):
        return "#include <" + header + ">"
