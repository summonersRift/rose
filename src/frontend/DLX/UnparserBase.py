


# The base class for DLX unparsing. Mainly used for some defines, which could come in handy
# when unparsing DLX
class DLXUnparser:
    def namespace(self, name, content):
        return "namespace " + name + " {\n" + content + "\n}"

    def typedef(self, a, b):
        return "typedef " + a + " " + b

    def stmt(self, content):
        return content + ";\n"

    def templatize(self, iden, tmplArg):
        return iden + "<" + tmplArg + ">"

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
