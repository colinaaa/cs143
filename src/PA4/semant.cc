

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <map>
#include <set>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;
std::map<Symbol, Symbol> depGraph;
std::map<int, Symbol> typetable;
std::map<int, std::vector<Symbol>> argstable;
std::set<Symbol> builtin_type;
Symbol current_class;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {
}

void ClassTable::install_basic_classes(SymTab* symtab) {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);
    Object_class->build_graph();
    symtab->addid(Object_class->get_name(), new int(new_id()));

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  
    IO_class->build_graph();
    symtab->addid(IO_class->get_name(), new int(new_id()));
    builtin_type.insert(IO);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);
    Int_class->build_graph();
    symtab->addid(Int_class->get_name(), new int(new_id()));
    builtin_type.insert(Int);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
    Bool_class->build_graph();
    symtab->addid(Bool_class->get_name(), new int(new_id()));
    builtin_type.insert(Bool);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    Str_class->build_graph();
    symtab->addid(Str_class->get_name(), new int(new_id()));
    builtin_type.insert(Str);
    symtab->addid("SELF_TYPE", new int(new_id()));
    Object_class->trav(filename->get_string(), symtab, 0);
    IO_class->trav(filename->get_string(), symtab, 0);
    Int_class->trav(filename->get_string(), symtab, 0);
    Bool_class->trav(filename->get_string(), symtab, 0);
    Str_class->trav(filename->get_string(), symtab, 0);

}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 


int class__class::build_graph(int second) const {
    if (second) {
        auto parent = depGraph[name];

        std::set<Symbol> visited{};
        visited.insert(name);

        while (strcmp(parent->get_string(), "Object") != 0) {
            if (visited.count(parent) != 0) {
                return -1;
            }
            visited.insert(parent);
            parent = depGraph[parent];
        }

        return 0;
    }
    depGraph[name] = parent;
    if (parent == SELF_TYPE || depGraph.count(parent) == 0) {
        return -1;
    }
    return 0;
}


string method_name(const Symbol class_name, const Symbol name) {
    return string(class_name->get_string()) + "@" + string(name->get_string());
}

int* lookup_identifier(SymTab* symtab, Symbol name) {
    int* id = symtab->lookup(name->get_string());
    auto t = current_class;
    if (semant_debug) {
        cout << "id: " << current_class->get_string() << "@" << name->get_string() << " " << endl;
    }

    while (t != No_class && !id) {
        if (semant_debug) {
            cout << "id: " << current_class->get_string() << "@" << name->get_string() << endl;
        }
        id = symtab->lookup(method_name(t, name));
        t = depGraph[t];
    }
    return id;
}


/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    /* some semantic analysis code may go here */

    /**
     * @brief first trav to build symbol table
     */
    auto *symtab = new SymbolTable<string, int>();
    symtab->enterscope();
    classtable->install_basic_classes(symtab);
    if (semant_debug) {
        cout << "finish install basic classes, start analysis classes" << endl;
    }
    bool hasMain = false;

    for (auto it = classes->first(); classes->more(it); it = classes->next(it)) {
        auto node = classes->nth(it);
        if (symtab->probe(node->get_name()) != NULL) {
            classtable->semant_error(node) << "dup define class: " << node->get_name() << endl;
        }
        symtab->addid(node->get_name(), new int(new_id()));
        auto res = node->build_graph();
        if (strcmp("Main", node->get_name()) == 0) {
            hasMain = true;
        }
        if (res == -1) {
            classtable->semant_error(node) << "unknown inherience class" << endl;
        }
    }

    if (!hasMain) {
        classtable->semant_error() << "Class Main is not defined." << endl;
    }
    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }

    for (auto it = classes->first(); classes->more(it); it = classes->next(it)) {
        auto node = classes->nth(it);
        auto res = node->build_graph(true);
        if (res == -1) {
            classtable->semant_error(node) << "circle inherience" << endl;
        }
    }

    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
    // cerr << "symtab... " << endl;
    for (auto it = classes->first(); classes->more(it); it = classes->next(it)) {
        auto node = classes->nth(it);
        auto errors = node->trav(node->get_filename()->get_string(), symtab, 0);

        if (errors) {
            classtable->semant_error();
        }
    }
    // cerr << "symtab end" << endl;
    symtab->exitscope();

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}



int class__class::trav(char* filename, SymTab* symtab, int padding) {
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav class: " << name << endl;
    current_class = name;
    for (auto it = features->first(); features->more(it); it = features->next(it)) {
        auto node = features->nth(it);
        errors += node->trav(filename, symtab, padding + 2);
    }
    symtab->enterscope();
    for (auto it = features->first(); features->more(it); it = features->next(it)) {
        auto node = features->nth(it);
        errors += node->trav(filename, symtab, padding + 2);
    }
    if (
        parent == Bool ||
        parent == Str ||
        parent == SELF_TYPE
    ) {
        ERROR("Class " + string(name->get_string()) + " cannot inherit class " + string(parent->get_string()) + ".")
    }
    symtab->exitscope();
    return errors;
}

char* class__class::get_name() const {
    return (name->get_string());
}

int method_class::trav(char* filename, SymTab* symtab, int padding) {
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav method: " << name << endl;

    if (!visited) {
        visited = true;
        const auto id = new int(new_id());

        symtab->addid(method_name(current_class, name), id);
        typetable.emplace(*id, return_type);
        argstable[*id] = std::vector<Symbol>{};

        // check args types
        for (auto it = formals->first(); formals->more(it); it = formals->next(it)) {
            auto node = formals->nth(it);
            auto t = node->get_type() == SELF_TYPE ? current_class : node->get_type();
            argstable[*id].push_back(t);
        }
        return errors;
    }
    // new method scope
    symtab->enterscope();

    for (auto it = formals->first(); formals->more(it); it = formals->next(it)) {
        auto node = formals->nth(it);
        errors += node->trav(filename, symtab, padding + 2);
    }

    if (symtab->lookup(return_type->get_string()) == NULL) {
        ERROR("Undefined return type " + string(return_type->get_string()))
    }

    if (semant_debug) {
        cout << pad(padding) << "entering method body, dumping symtab" << endl;
        symtab->dump();
    }

    // go into method body expr
    errors += expr->trav(filename, symtab, padding + 2);
    symtab->exitscope();

    // do not check return type of builtin classes
    if (builtin_type.count(current_class) != 0 || current_class == Object) {
        return errors;
    }

    if (return_type == SELF_TYPE && expr->type != SELF_TYPE) {
        ERROR("Inferred return type of method does not conform to declared return type SELF_TYPE.")
    }

    // if (expr->type == SELF_TYPE || return_type == SELF_TYPE) {
    //     if (expr->type != return_type) {
    //         ERROR("Method body type not match return type (SELF_TYPE)")
    //     }
    // } else if (!type_equal(expr->type, return_type)) {
    //     if (semant_debug) {
    //         cout << expr->type->get_string() << " vs " << return_type->get_string() << endl;
    //     }
    //     ERROR("Method body type not match return type")
    // }

    // exit this method scope
    return errors;
}

char* method_class::get_name() const {
    return name->get_string();
}

int attr_class::trav(char* filename, SymTab* symtab, int padding) {
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav attr: " << name << endl;

    if (!visited) {
        if (lookup_identifier(symtab, name) != NULL) {
        ERROR("duplicated attr id: " + string(name->get_string()))
        }
        const auto id = new int(new_id());
        symtab->addid(method_name(current_class, name), id);
        typetable.emplace(*id, type_decl);
        visited = true;
        return errors;
    }

    if (strcmp(name->get_string(), "self") == 0) {
        ERROR("'self' cannot be the name of an attribute.")
    }

    errors += init->trav(filename, symtab, padding + 2);

    return errors;
}

char* attr_class::get_name() const {
    return name->get_string();
}

int formal_class::trav(char* filename, SymTab* symtab, int padding) {
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav formal: " << name << endl;

    if (symtab->probe(name->get_string())) {
        ERROR("Formal parameter " + string(name->get_string()) + " is multiply defined.")
    }

    if (strcmp(name->get_string(), "self") == 0) {
        ERROR("self can't be formal parameter")
    }

    if (type_decl == SELF_TYPE) {
        ERROR("SELF_TYPE can't be formal type")
    }

    const auto id = new int(new_id());
    symtab->addid(name->get_string(), id);
    typetable.emplace(*id, type_decl);

    return errors;
}

int branch_class::trav(char* filename, SymTab* symtab, int padding) {
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav branch: " << name << ":" << type_decl << endl;

    // new case match scope
    symtab->enterscope();
    const auto id = new int(new_id());
    symtab->addid(name->get_string(), id);
    typetable.emplace(*id, type_decl);

    if (semant_debug) {
        cout << pad(padding) << "entering case match, dumping symtab:" << endl;
        symtab->dump();
    }

    expr->trav(filename, symtab, padding + 2);
    symtab->exitscope();
    return errors;
}

int assign_class::trav(char* filename, SymTab* symtab, int padding) {
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav expr assign: " << name << endl;
    errors += expr->trav(filename, symtab, padding + 2);

    const auto id = lookup_identifier(symtab, name);
    if (!id) {
        ERROR("Unknown assign identifier " + string(name->get_string()))
        return errors;
    }

    if (strcmp(expr->type->get_string(), typetable[*id]->get_string()) != 0) {
        ERROR("assign type unmatch")
    }
    
    type = expr->type;

    return errors;
}

int static_dispatch_class::trav(char* filename, SymTab* symtab, int padding) {
    // <expr>@<type>.id(<expr>, ... <expr>)
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav static dispatch: " << name << endl;

    errors += expr->trav(filename, symtab, padding + 2);

    for (auto it = actual->first(); actual->more(it); it = actual->next(it)) {
        auto node = actual->nth(it);
        errors += node->trav(filename, symtab, padding + 2);
    }

    const auto id = symtab->lookup(method_name(type_name, name));
    if (semant_debug) {
        cout << pad(padding) << expr->type->get_string() << "@" << type_name->get_string()
        << "." << name->get_string() << endl;
    }
    if (id == NULL) {
        ERROR("static dispatch not found")
        return errors;
    }

    const auto& args = argstable[*id];
    if (args.size() != static_cast<size_t>(actual->len())) {
        ERROR("args length not match")
        return errors;
    }

    auto i = actual->first();

    for (auto it = args.begin(); it != args.end(); it++) {
        auto arg = actual->nth(i);
        if (arg->type != *it) {
            ERROR("argument type not match")
        }
    }

    type = typetable[*id];

    if (type == SELF_TYPE) {
        type = expr->type;
    }
    return errors;
}

bool type_equal(const Symbol lhs, const Symbol rhs) {
    if (lhs == rhs) {
        return true;
    }
    // deal with Int, String, Bool, etc.
    if (builtin_type.count(lhs) != 0 || builtin_type.count(rhs) != 0) {
        return false;
    }

    auto t1 = lhs == SELF_TYPE ? current_class : lhs;
    auto t2 = rhs == SELF_TYPE ? current_class : rhs;

    while (t1 != t2) {
        t1 = t1 == No_class ? rhs : depGraph[t1];
        t2 = t2 == No_class ? lhs : depGraph[t2];
    }

    return !(t1 == No_class);
}

int dispatch_class::trav(char* filename, SymTab* symtab, int padding) {
    // id(<expr>, ... <expr>)
    // short hand for self.<id>(...)
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav dispatch: " << name << endl;

    // self
    errors += expr->trav(filename, symtab, padding + 2);

    for (auto it = actual->first(); actual->more(it); it = actual->next(it)) {
        auto node = actual->nth(it);
        errors += node->trav(filename, symtab, padding + 2);
    }

    auto t = expr->type;
    int* id;

    while (t != No_class) {
        if (t == SELF_TYPE) {
            t = current_class;
        }
        id = symtab->lookup(method_name(t, name));
        if (semant_debug) {
            cout << pad(padding) << t->get_string() << "." << name->get_string() << endl;
        }
        if (id != NULL) {
            break;
        }
        t = depGraph[t];
    }

    if (id == NULL) {
        ERROR(string(name->get_string()) + "() not found")
        return errors;
    }

    const auto& args = argstable[*id];
    if (args.size() != static_cast<size_t>(actual->len())) {
        if (semant_debug) {
            cout << "table: " << args.size() << " actual: " << actual->len() << endl;
        }
        ERROR("args length not match")
        return errors;
    }

    auto i = actual->first();

    for (auto it = args.begin(); it != args.end(); (i = actual->next(i)), it++) {
        auto arg = actual->nth(i);
        if (!type_equal(arg->type, *it)) {
            if (semant_debug) {
                cout << arg->type->get_string() << " vs " << (*it)->get_string() << endl;
            }
            ERROR("argument type not match")
        }
    }

    type = idtable.add_string(typetable[*id]->get_string());

    if (semant_debug) {
        cout << "dispatch type: " << type->get_string() << endl;
    }

    if (type == SELF_TYPE) {
        if (semant_debug) {
            cout << "SELF_TYPE to" << expr->type->get_string() << endl;
        }
        type = expr->type;
    }
    return errors;
}

int cond_class::trav(char* filename, SymTab* symtab, int padding) {
    // if <expr> then <expr> else <expr> fi
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav cond: " << endl;

    errors += pred->trav(filename, symtab, padding + 2);
    errors += then_exp->trav(filename, symtab, padding + 2);
    errors += else_exp->trav(filename, symtab, padding + 2);

    type = Object;

    return errors;
}

int loop_class::trav(char* filename, SymTab* symtab, int padding) {
    // while <expr> loop <expr> pool
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav loop: " << endl;

    errors += pred->trav(filename, symtab, padding + 2);
    if (pred->type != Bool) {
        ERROR("loop condition not bool, but: " + string(pred->type->get_string()))
    }
    errors += body->trav(filename, symtab, padding + 2);

    type = Object;

    return errors;
}

int typcase_class::trav(char* filename, SymTab* symtab, int padding) {
    // case expr of [[ID : TYPE => expr; ]] + esac
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav typecase: " << endl;

    errors += expr->trav(filename, symtab, padding + 2);
    std::set<Symbol> branch_type;

    for (auto it = cases->first(); cases->more(it); it = cases->next(it)) {
        auto node = cases->nth(it);
        node->trav(filename, symtab, padding + 2);
        if (branch_type.count(node->get_type()) != 0) {
            ERROR("Duplicate branch " + string(node->get_type()->get_string()) + " in case statement.")
        }
        branch_type.insert(node->get_type());
    }
    
    type = expr->type;

    return errors;
}

int block_class::trav(char* filename, SymTab* symtab, int padding) {
    // { <expr>; ... <expr>; }
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav block: " << endl;

    for (auto it = body->first(); body->more(it); it = body->next(it)) {
        auto node = body->nth(it);
        errors += node->trav(filename, symtab, padding + 2);
        type = node->type;
    }

    return errors;
}

int let_class::trav(char* filename, SymTab* symtab, int padding) {
    // let <id1> : <type1> [ <- <expr1> ], ..., <idn> : <typen> [ <- <exprn> ] in <expr>
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav let: " << identifier << ':' << type_decl << endl;

    if (strcmp(identifier->get_string(), "self") == 0) {
        ERROR("'self' cannot be bound in a 'let' expression.")
        return errors;
    }

    errors += init->trav(filename, symtab, padding + 2);
    if (!(init->type == No_type)) {
        bool match = false;
        auto t = init->type;
        while (strcmp(t->get_string(), "Object") != 0) {
            if (string(type_decl->get_string()) == string(t->get_string())) {
            match = true;
            break;
            }
            t = depGraph[t];
        }

        if (!match) {
            ERROR("init type not match, expect " + string(type_decl->get_string()) + " got: " + string(init->type->get_string()))
        }
    }
    
    // enter let binding scope
    symtab->enterscope();
    const auto id = new int(new_id());
    symtab->addid(identifier->get_string(), id);
    typetable.emplace(*id, type_decl);

    if (semant_debug) {
        cout << pad(padding) << "entering let body, dump symtab: " << endl;
        symtab->dump();
    }
    errors += body->trav(filename, symtab, padding + 2);
    symtab->exitscope();

    type = body->type;

    return errors;
}

int plus_class::trav(char* filename, SymTab* symtab, int padding) {
    // e1 + e2
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav plus: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);
    errors += e2->trav(filename, symtab, padding + 2);
    // cout << e1->type->get_string() << ' ' << e2->type->get_string() << endl;
    if (e1->type != Int || e2->type != Int) {
        ERROR("non-Int arguments: " + string(e1->type->get_string()) + " + " + string(e2->type->get_string()))
    }

    type = Int;

    return errors;
}

int sub_class::trav(char* filename, SymTab* symtab, int padding) {
    // e1 - e2
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav sub: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);
    errors += e2->trav(filename, symtab, padding + 2);
    if (e1->type != Int || e2->type != Int) {
        ERROR("non-Int expression " + string(e1->type->get_string()) + " - " + string(e2->type->get_string()) + "")
    }

    type = Int;

    return errors;
}

int mul_class::trav(char* filename, SymTab* symtab, int padding) {
    // e1 * e2
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav mul: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);
    errors += e2->trav(filename, symtab, padding + 2);
    if (e1->type != Int || e2->type != Int) {
        ERROR("non-Int expression " + string(e1->type->get_string()) + " * " + string(e2->type->get_string()) + "")
    }

    type = Int;

    return errors;
}

int divide_class::trav(char* filename, SymTab* symtab, int padding) {
    // e1 / e2
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav divide: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);
    errors += e2->trav(filename, symtab, padding + 2);
    if (e1->type != Int || e2->type != Int) {
        ERROR("non-Int expression " + string(e1->type->get_string()) + " / " + string(e2->type->get_string()) + "")
    }

    type = Int;
    return errors;
}

int neg_class::trav(char* filename, SymTab* symtab, int padding) {
    // ~e1
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav neg: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);

    if (e1->type != Int) {
        ERROR("non-Bool expression: " + string(e1->type->get_string()))
    }
    type = Int;

    return errors;
}

int lt_class::trav(char* filename, SymTab* symtab, int padding) {
    // e1 < e2
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav lt: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);
    errors += e2->trav(filename, symtab, padding + 2);

    if (e1->type != e2->type) {
        ERROR("non-Bool expression: " + string(e1->type->get_string()) + " < " + string(e2->type->get_string()))
    }
    type = Bool;

    return errors;
}

int eq_class::trav(char* filename, SymTab* symtab, int padding) {
    // e1 = e2
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav eq: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);
    errors += e2->trav(filename, symtab, padding + 2);

    if (e1->type != e2->type) {
        if (builtin_type.count(e1->type) || builtin_type.count(e2->type)) {
            ERROR("non-eq expression: " + string(e1->type->get_string()) + " = " + string(e2->type->get_string()))
        }
    }
    type = Bool;

    return errors;
}

int leq_class::trav(char* filename, SymTab* symtab, int padding) {
    // e1 <= e2
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav leq: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);
    errors += e2->trav(filename, symtab, padding + 2);

    if (e1->type != e2->type) {
        ERROR("non-Bool expression: " + string(e1->type->get_string()) + " <= " + string(e2->type->get_string()))
    }

    type = Bool;

    return errors;
}

int comp_class::trav(char* filename, SymTab* symtab, int padding) {
    // (e1)
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav comp: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);

    type = e1->type;

    return errors;
}

int int_const_class::trav(char* filename, SymTab* symtab, int padding) {
    // 123
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav int_const: " << token << endl;

    type = Int;

    return errors;
}

int bool_const_class::trav(char* filename, SymTab* symtab, int padding) {
    // true
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav bool_const: " << val << endl;

    type = Bool;

    return errors;
}

int string_const_class::trav(char* filename, SymTab* symtab, int padding) {
    // "abcd"
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav string_const: " << token << endl;
    
    type = Str;

    return errors;
}

int new__class::trav(char* filename, SymTab* symtab, int padding) {
    // new A
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav new: " << type_name << endl;

    if (symtab->lookup(type_name->get_string()) == NULL) {
        ERROR("'new' used with undefined class " + string(type_name->get_string()))
    }
    type = idtable.add_string(type_name->get_string());

    return errors;
}

int isvoid_class::trav(char* filename, SymTab* symtab, int padding) {
    // new A
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav isvoid: " << endl;

    errors += e1->trav(filename, symtab, padding + 2);

    type = Bool;

    return errors;
}

int no_expr_class::trav(char* filename, SymTab* symtab, int padding) {
    // ??
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav noexpr: " << endl;
    
    type = No_type;

    return errors;
}

int object_class::trav(char* filename, SymTab* symtab, int padding) {
    // ??
    int errors = 0;
    if (semant_debug) cout << pad(padding) << "trav object: " << name << endl;

    if (strcmp(name->get_string(), "self") == 0) {
        type = SELF_TYPE;
        return 0;
    }

    auto id = lookup_identifier(symtab, name);

    if (!id) {
        ERROR("Undeclared identifier " + string(name->get_string()) + ".")
        type = Object;
        return errors;
    }

    type = idtable.add_string(typetable[*id]->get_string());

    return errors;
}