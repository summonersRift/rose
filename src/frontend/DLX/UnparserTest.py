import UnparserBase


# This should be a little testing
# FIXME Move to tests directory
#
#class UnparserTestsRunner:
#    def testInclude(self):
#        unparser = DLXUnparser()
#        out = unparser.userInclude("myInclude.h")
#        print(out)


# This is main
print("DLX Unparser tests")
unparser = UnparserBase.DLXUnparser()

result = unparser.namespace("TestNS", "TestContentString")
assert result == "namespace TestNS {\nTestContentString\n}"

result = unparser.typedef("A", "B")
assert result == "typedef A B"

result = unparser.stmt("A")
assert result == "A;\n"

result = unparser.stmt(unparser.typedef("A", "B"))
assert result == "typedef A B;\n"

result = unparser.enum("test", ["one", "two"])
assert result == "enum test {\none,\ntwo\n}"

result = unparser.enum("check", ["a"])
assert result == "enum check {\na\n}"

result = unparser.struct("MyStruct", "TestContent", 0)
assert result == "struct MyStruct {\nTestContent\n}"

result = unparser.userInclude("my/test.h")
assert result == "#include \"my/test.h\""

result = unparser.sysInclude("iostream")
assert result == "#include <iostream>"

print(result)
