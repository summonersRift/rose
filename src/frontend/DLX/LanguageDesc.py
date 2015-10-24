# The class represents a description of the actual language.
# It has a name and lists of constructs, clauses, directives and relations between the directives
# In addition we find type declarations a user needs as well as additional inputs.
#
# TODO Tristan, I basically reverse-engineered that from the TileK header, is that correct?
class Language:
    def __init__(self, langName, constructs, clauses, directives, relations, typesAndIncludes):
        self.name_ = langName
        self.constructs_ = constructs
        self.clauses_ = clauses
        self.directives_ = directives
        self.relations_ = relations
        self.types_ = typesAndIncludes['types']
        self.includes_ = typesAndIncludes['includes']


