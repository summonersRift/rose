import LanguageDescUnparser
import LanguageDesc


print("Testing the DLXUnparser")

# A language consists of a name, a list of constructs, clauses and directives
lName = "MyTest"
lConstructs = ["ConstructOne", "ConstructTwo"]
lClauses = ["ClauseOne", "ClauseTwo"]
lDirectives = ["DirectiveOne", "DirectiveTwo"]
# Further we have a list of relations and additional includes and types lists
aIncludes = ["inlcude/one.h", "include/two.h"]
aTypes = ["class MyFunkyClass"]
lRelations = ["RelationOne", "RelationTwo"]

lInclAndTypes = {'includes':aIncludes, 'types':aTypes}

testLang = LanguageDesc.Language(lName, lConstructs, lClauses, lDirectives, lRelations, lInclAndTypes)

unparser = LanguageDescUnparser.HeaderUnparser(testLang)

unparser.unparse()
