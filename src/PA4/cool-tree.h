#ifndef COOL_TREE_H
#define COOL_TREE_H
//////////////////////////////////////////////////////////
//
// file: cool-tree.h
//
// This file defines classes for each phylum and constructor
//
//////////////////////////////////////////////////////////


#include <map>
#include "tree.h"
#include "symtab.h"
#include "cool-tree.handcode.h"

using SymTab = SymbolTable<char *, int>;
using std::string, std::to_string;

extern int semant_debug;

extern int i_;
int new_id();

#define ERROR(MSG) errors++; cerr << filename << ":" << get_line_number() << ": " << (MSG) << endl;

extern std::map<Symbol, Symbol> depGraph;
extern std::map<int, Symbol> typetable;

// define the class for phylum
// define simple phylum - Program
typedef class Program_class *Program;

class Program_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Program(); }
   virtual Program copy_Program() = 0;

#ifdef Program_EXTRAS
   Program_EXTRAS
#endif
};


// define simple phylum - Class_
typedef class Class__class *Class_;

class Class__class : public tree_node {
public:
   tree_node *copy()		 { return copy_Class_(); }
   virtual Class_ copy_Class_() = 0;
   /**
    * @brief trav through a Class Node
    * 
    * @return int the error number
    */
   virtual int trav(char* filename, SymTab* symtab, int pad) = 0;

   /**
    * @brief Get the name object
    * 
    * @return char* the name
    */
   virtual char* get_name() const = 0;

   virtual int build_graph(int second = false) const = 0;
#ifdef Class__EXTRAS
   Class__EXTRAS
#endif
};


// define simple phylum - Feature
typedef class Feature_class *Feature;

class Feature_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Feature(); }
   virtual Feature copy_Feature() = 0;
   /**
    * @brief trav through a Feature Node
    * 
    * @return int the error number
    */
   virtual int trav(char* filename, SymTab* symtab, int pad) = 0;

#ifdef Feature_EXTRAS
   Feature_EXTRAS
#endif
};


// define simple phylum - Formal
typedef class Formal_class *Formal;

class Formal_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Formal(); }
   virtual Formal copy_Formal() = 0;
   /**
    * @brief trav through a Formal Node
    * 
    * @return int the error number
    */
   virtual int trav(char* filename, SymTab* symtab, int pad) = 0;

#ifdef Formal_EXTRAS
   Formal_EXTRAS
#endif
};


// define simple phylum - Expression
typedef class Expression_class *Expression;

class Expression_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Expression(); }
   virtual Expression copy_Expression() = 0;
   /**
    * @brief trav through a Expression Node
    * 
    * @return int the error number
    */
   virtual int trav(char* filename, SymTab* symtab, int pad) = 0;

#ifdef Expression_EXTRAS
   Expression_EXTRAS
#endif
};


// define simple phylum - Case
typedef class Case_class *Case;

class Case_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Case(); }
   virtual Case copy_Case() = 0;
   /**
    * @brief trav through a Case Node
    * 
    * @return int the error number
    */
   virtual int trav(char* filename, SymTab* symtab, int pad) = 0;

#ifdef Case_EXTRAS
   Case_EXTRAS
#endif
};


// define the class for phylum - LIST
// define list phlyum - Classes
typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;


// define list phlyum - Features
typedef list_node<Feature> Features_class;
typedef Features_class *Features;


// define list phlyum - Formals
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;


// define list phlyum - Expressions
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;


// define list phlyum - Cases
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;


// define the class for constructors
// define constructor - program
class program_class : public Program_class {
protected:
   Classes classes;
public:
   program_class(Classes a1) {
      classes = a1;
   }
   Program copy_Program();
   void dump(ostream& stream, int n);

#ifdef Program_SHARED_EXTRAS
   Program_SHARED_EXTRAS
#endif
#ifdef program_EXTRAS
   program_EXTRAS
#endif
};


// define constructor - class_
class class__class : public Class__class {
protected:
   Symbol name;
   Symbol parent;
   Features features;
   Symbol filename;
public:
   int build_graph(int second = false) const final;
   class__class(Symbol a1, Symbol a2, Features a3, Symbol a4) {
      name = a1;
      parent = a2;
      features = a3;
      filename = a4;
   }
   Class_ copy_Class_();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav class: " << name << endl;
      symtab->enterscope();
      for (auto it = features->first(); features->more(it); it = features->next(it)) {
         auto node = features->nth(it);
         errors += node->trav(filename, symtab, padding + 2);
      }
      symtab->exitscope();
      return errors;
   };

   char* get_name() const final {
      return (name->get_string());
   }

#ifdef Class__SHARED_EXTRAS
   Class__SHARED_EXTRAS
#endif
#ifdef class__EXTRAS
   class__EXTRAS
#endif
};


// define constructor - method
class method_class : public Feature_class {
protected:
   Symbol name;
   Formals formals;
   Symbol return_type;
   Expression expr;
public:
   method_class(Symbol a1, Formals a2, Symbol a3, Expression a4) {
      name = a1;
      formals = a2;
      return_type = a3;
      expr = a4;
   }
   Feature copy_Feature();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding = 0) final {
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav method: " << name << endl;

      // new method scope
      symtab->enterscope();

      for (auto it = formals->first(); formals->more(it); it = formals->next(it)) {
         auto node = formals->nth(it);
         errors += node->trav(filename, symtab, padding + 2);
      }

      if (semant_debug) {
         cout << pad(padding) << "entering method body, dumping symtab" << endl;
         symtab->dump();
      }

      // go into method body expr
      errors += expr->trav(filename, symtab, padding + 2);

      // exit this method scope
      symtab->exitscope();
      return errors;
   };

#ifdef Feature_SHARED_EXTRAS
   Feature_SHARED_EXTRAS
#endif
#ifdef method_EXTRAS
   method_EXTRAS
#endif
};


// define constructor - attr
class attr_class : public Feature_class {
protected:
   Symbol name;
   Symbol type_decl;
   Expression init;
public:
   attr_class(Symbol a1, Symbol a2, Expression a3) {
      name = a1;
      type_decl = a2;
      init = a3;
   }
   Feature copy_Feature();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding = 0) final {
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav attr: " << name << endl;

      if (strcmp(name->get_string(), "self") == 0) {
         ERROR("'self' cannot be the name of an attribute.")
      }

      if (symtab->probe(name->get_string()) != NULL) {
         ERROR("duplicated id: " + string(name->get_string()))
      }
      
      errors += init->trav(filename, symtab, padding + 2);

      const auto id = new int(new_id());
      symtab->addid(name->get_string(), id);
      typetable.emplace(*id, type_decl);
      return errors;
   };

#ifdef Feature_SHARED_EXTRAS
   Feature_SHARED_EXTRAS
#endif
#ifdef attr_EXTRAS
   attr_EXTRAS
#endif
};


// define constructor - formal
class formal_class : public Formal_class {
protected:
   Symbol name;
   Symbol type_decl;
public:
   formal_class(Symbol a1, Symbol a2) {
      name = a1;
      type_decl = a2;
   }
   Formal copy_Formal();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding = 0) final {
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav formal: " << name << endl;

      if (symtab->probe(name->get_string())) {
         ERROR("Formal parameter " + string(name->get_string()) + " is multiply defined.")
      }

      const auto id = new int(new_id());
      symtab->addid(name->get_string(), id);
      typetable.emplace(*id, type_decl);

      return errors;
   };

#ifdef Formal_SHARED_EXTRAS
   Formal_SHARED_EXTRAS
#endif
#ifdef formal_EXTRAS
   formal_EXTRAS
#endif
};


// define constructor - branch
class branch_class : public Case_class {
protected:
   Symbol name;
   Symbol type_decl;
   Expression expr;
public:
   branch_class(Symbol a1, Symbol a2, Expression a3) {
      name = a1;
      type_decl = a2;
      expr = a3;
   }
   Case copy_Case();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding = 0) final {
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

#ifdef Case_SHARED_EXTRAS
   Case_SHARED_EXTRAS
#endif
#ifdef branch_EXTRAS
   branch_EXTRAS
#endif
};


// define constructor - assign
class assign_class : public Expression_class {
protected:
   Symbol name;
   Expression expr;
public:
   assign_class(Symbol a1, Expression a2) {
      name = a1;
      expr = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav expr assign: " << name << endl;
      errors += expr->trav(filename, symtab, padding + 2);
      
      type = expr->type;

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef assign_EXTRAS
   assign_EXTRAS
#endif
};


// define constructor - static_dispatch
class static_dispatch_class : public Expression_class {
protected:
   Expression expr;
   Symbol type_name;
   Symbol name;
   Expressions actual;
public:
   static_dispatch_class(Expression a1, Symbol a2, Symbol a3, Expressions a4) {
      expr = a1;
      type_name = a2;
      name = a3;
      actual = a4;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // <expr>@<type>.id(<expr>, ... <expr>)
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav static dispatch: " << name << endl;

      errors += expr->trav(filename, symtab, padding + 2);

      for (auto it = actual->first(); actual->more(it); it = actual->next(it)) {
         auto node = actual->nth(it);
         errors += node->trav(filename, symtab, padding + 2);
      }
      // TODO: disptch type
      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef static_dispatch_EXTRAS
   static_dispatch_EXTRAS
#endif
};


// define constructor - dispatch
class dispatch_class : public Expression_class {
protected:
   Expression expr;
   Symbol name;
   Expressions actual;
public:
   dispatch_class(Expression a1, Symbol a2, Expressions a3) {
      expr = a1;
      name = a2;
      actual = a3;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // id(<expr>, ... <expr>)
      // short hand for self.<id>(...)
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav dispatch: " << name << endl;
      

      for (auto it = actual->first(); actual->more(it); it = actual->next(it)) {
         auto node = actual->nth(it);
         errors += node->trav(filename, symtab, padding + 2);
      }

      // TODO: disptch type
      return errors;
   };


#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef dispatch_EXTRAS
   dispatch_EXTRAS
#endif
};


// define constructor - cond
class cond_class : public Expression_class {
protected:
   Expression pred;
   Expression then_exp;
   Expression else_exp;
public:
   cond_class(Expression a1, Expression a2, Expression a3) {
      pred = a1;
      then_exp = a2;
      else_exp = a3;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // if <expr> then <expr> else <expr> fi
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav cond: " << endl;

      errors += pred->trav(filename, symtab, padding + 2);
      errors += then_exp->trav(filename, symtab, padding + 2);
      errors += else_exp->trav(filename, symtab, padding + 2);

      type = idtable.add_string("_no_type");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef cond_EXTRAS
   cond_EXTRAS
#endif
};


// define constructor - loop
class loop_class : public Expression_class {
protected:
   Expression pred;
   Expression body;
public:
   loop_class(Expression a1, Expression a2) {
      pred = a1;
      body = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // while <expr> loop <expr> pool
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav loop: " << endl;

      errors += pred->trav(filename, symtab, padding + 2);
      errors += body->trav(filename, symtab, padding + 2);

      type = idtable.add_string("_no_type");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef loop_EXTRAS
   loop_EXTRAS
#endif
};


// define constructor - typcase
class typcase_class : public Expression_class {
protected:
   Expression expr;
   Cases cases;
public:
   typcase_class(Expression a1, Cases a2) {
      expr = a1;
      cases = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // case expr of [[ID : TYPE => expr; ]] + esac
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav typecase: " << endl;

      errors += expr->trav(filename, symtab, padding + 2);

      for (auto it = cases->first(); cases->more(it); it = cases->next(it)) {
         auto node = cases->nth(it);
         node->trav(filename, symtab, padding + 2);
      }
      
      type = expr->type;

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef typcase_EXTRAS
   typcase_EXTRAS
#endif
};


// define constructor - block
class block_class : public Expression_class {
protected:
   Expressions body;
public:
   block_class(Expressions a1) {
      body = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // { <expr>; ... <expr>; }
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav block: " << endl;

      for (auto it = body->first(); body->more(it); it = body->next(it)) {
         auto node = body->nth(it);
         errors += node->trav(filename, symtab, padding + 2);
         type = node->type;
      }

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef block_EXTRAS
   block_EXTRAS
#endif
};


// define constructor - let
class let_class : public Expression_class {
protected:
   Symbol identifier;
   Symbol type_decl;
   Expression init;
   Expression body;
public:
   let_class(Symbol a1, Symbol a2, Expression a3, Expression a4) {
      identifier = a1;
      type_decl = a2;
      init = a3;
      body = a4;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // let <id1> : <type1> [ <- <expr1> ], ..., <idn> : <typen> [ <- <exprn> ] in <expr>
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav let: " << identifier << ':' << type_decl << endl;

      errors += init->trav(filename, symtab, padding + 2);
      if (init->type != idtable.add_string(type_decl->get_string())) {
         // TODO: deal with inherience
         // error msg
         ERROR("")
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
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef let_EXTRAS
   let_EXTRAS
#endif
};


// define constructor - plus
class plus_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   plus_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // e1 + e2
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav plus: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);
      errors += e2->trav(filename, symtab, padding + 2);
      // cout << e1->type->get_string() << ' ' << e2->type->get_string() << endl;
      if (e1->type != idtable.add_string("Int") || e2->type != idtable.add_string("Int")) {
         ERROR("non-Int arguments: " + string(e1->type->get_string()) + " + " + string(e2->type->get_string()))
      }

      type = idtable.add_string("Int");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef plus_EXTRAS
   plus_EXTRAS
#endif
};


// define constructor - sub
class sub_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   sub_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // e1 - e2
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav sub: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);
      errors += e2->trav(filename, symtab, padding + 2);
      if (e1->type != idtable.add_string("Int") || e2->type != idtable.add_string("Int")) {
         ERROR("non-Int expression " + string(e1->type->get_string()) + " - " + string(e2->type->get_string()) + "")
      }

      type = idtable.add_string("Int");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef sub_EXTRAS
   sub_EXTRAS
#endif
};


// define constructor - mul
class mul_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   mul_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // e1 * e2
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav mul: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);
      errors += e2->trav(filename, symtab, padding + 2);
      if (e1->type != idtable.add_string("Int") || e2->type != idtable.add_string("Int")) {
         ERROR("non-Int expression " + string(e1->type->get_string()) + " * " + string(e2->type->get_string()) + "")
      }

      type = idtable.add_string("Int");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef mul_EXTRAS
   mul_EXTRAS
#endif
};


// define constructor - divide
class divide_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   divide_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // e1 / e2
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav divide: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);
      errors += e2->trav(filename, symtab, padding + 2);
      if (e1->type != idtable.add_string("Int") || e2->type != idtable.add_string("Int")) {
         // TODO: error msg
         ERROR("non-Int expression " + string(e1->type->get_string()) + " / " + string(e2->type->get_string()) + "")
      }

      type = idtable.add_string("Int");
      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef divide_EXTRAS
   divide_EXTRAS
#endif
};


// define constructor - neg
class neg_class : public Expression_class {
protected:
   Expression e1;
public:
   neg_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // ~e1
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav neg: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);

      if (e1->type != idtable.add_string("Bool")) {
         ERROR("non-Bool expression: " + string(e1->type->get_string()))
      }
      type = idtable.add_string("Bool");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef neg_EXTRAS
   neg_EXTRAS
#endif
};


// define constructor - lt
class lt_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   lt_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // e1 < e2
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav lt: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);
      errors += e2->trav(filename, symtab, padding + 2);

      if (e1->type != e2->type) {
         ERROR("non-Bool expression: " + string(e1->type->get_string()) + " < " + string(e2->type->get_string()))
      }
      type = idtable.add_string("Bool");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef lt_EXTRAS
   lt_EXTRAS
#endif
};


// define constructor - eq
class eq_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   eq_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // e1 = e2
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav eq: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);
      errors += e2->trav(filename, symtab, padding + 2);

      if (e1->type != e2->type) {
         ERROR("non-Bool expression: " + string(e1->type->get_string()) + " = " + string(e2->type->get_string()))
      }
      type = idtable.add_string("Bool");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef eq_EXTRAS
   eq_EXTRAS
#endif
};


// define constructor - leq
class leq_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   leq_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // e1 <= e2
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav leq: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);
      errors += e2->trav(filename, symtab, padding + 2);

      if (e1->type != e2->type) {
         ERROR("non-Bool expression: " + string(e1->type->get_string()) + " <= " + string(e2->type->get_string()))
      }

      type = idtable.add_string("Bool");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef leq_EXTRAS
   leq_EXTRAS
#endif
};


// define constructor - comp
class comp_class : public Expression_class {
protected:
   Expression e1;
public:
   comp_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // (e1)
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav comp: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);

      type = e1->type;

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef comp_EXTRAS
   comp_EXTRAS
#endif
};


// define constructor - int_const
class int_const_class : public Expression_class {
protected:
   Symbol token;
public:
   int_const_class(Symbol a1) {
      token = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // 123
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav int_const: " << token << endl;

      type = idtable.add_string("Int");

      return errors;
   };


#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef int_const_EXTRAS
   int_const_EXTRAS
#endif
};


// define constructor - bool_const
class bool_const_class : public Expression_class {
protected:
   Boolean val;
public:
   bool_const_class(Boolean a1) {
      val = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // true
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav bool_const: " << val << endl;

      type = idtable.add_string("Bool");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef bool_const_EXTRAS
   bool_const_EXTRAS
#endif
};


// define constructor - string_const
class string_const_class : public Expression_class {
protected:
   Symbol token;
public:
   string_const_class(Symbol a1) {
      token = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // "abcd"
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav string_const: " << token << endl;
      
      type = idtable.add_string("String");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef string_const_EXTRAS
   string_const_EXTRAS
#endif
};


// define constructor - new_
class new__class : public Expression_class {
protected:
   Symbol type_name;
public:
   new__class(Symbol a1) {
      type_name = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // new A
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav new: " << type_name << endl;

      type = idtable.add_string(type_name->get_string());

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef new__EXTRAS
   new__EXTRAS
#endif
};


// define constructor - isvoid
class isvoid_class : public Expression_class {
protected:
   Expression e1;
public:
   isvoid_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // new A
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav isvoid: " << endl;

      errors += e1->trav(filename, symtab, padding + 2);

      type = idtable.add_string("Bool");

      return errors;
   };


#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef isvoid_EXTRAS
   isvoid_EXTRAS
#endif
};


// define constructor - no_expr
class no_expr_class : public Expression_class {
protected:
public:
   no_expr_class() {
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // ??
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav noexpr: " << endl;
      
      type = idtable.add_string("_no_type");

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef no_expr_EXTRAS
   no_expr_EXTRAS
#endif
};


// define constructor - object
class object_class : public Expression_class {
protected:
   Symbol name;
public:
   object_class(Symbol a1) {
      name = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   int trav(char* filename, SymTab* symtab, int padding) final {
      // ??
      int errors = 0;
      if (semant_debug) cout << pad(padding) << "trav object: " << name << endl;

      if (strcmp(name->get_string(), "self") == 0) {
         type = idtable.add_string("SELF_TYPE");
         return 0;
      }

      const auto id = symtab->lookup(name->get_string());

      if (!id) {
         ERROR("Undeclared identifier " + string(name->get_string()) + ".")
         type = idtable.add_string("Object");
         return errors;
      }
      
      type = idtable.add_string(typetable[*id]->get_string());

      return errors;
   };

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef object_EXTRAS
   object_EXTRAS
#endif
};


// define the prototypes of the interface
Classes nil_Classes();
Classes single_Classes(Class_);
Classes append_Classes(Classes, Classes);
Features nil_Features();
Features single_Features(Feature);
Features append_Features(Features, Features);
Formals nil_Formals();
Formals single_Formals(Formal);
Formals append_Formals(Formals, Formals);
Expressions nil_Expressions();
Expressions single_Expressions(Expression);
Expressions append_Expressions(Expressions, Expressions);
Cases nil_Cases();
Cases single_Cases(Case);
Cases append_Cases(Cases, Cases);
Program program(Classes);
Class_ class_(Symbol, Symbol, Features, Symbol);
Feature method(Symbol, Formals, Symbol, Expression);
Feature attr(Symbol, Symbol, Expression);
Formal formal(Symbol, Symbol);
Case branch(Symbol, Symbol, Expression);
Expression assign(Symbol, Expression);
Expression static_dispatch(Expression, Symbol, Symbol, Expressions);
Expression dispatch(Expression, Symbol, Expressions);
Expression cond(Expression, Expression, Expression);
Expression loop(Expression, Expression);
Expression typcase(Expression, Cases);
Expression block(Expressions);
Expression let(Symbol, Symbol, Expression, Expression);
Expression plus(Expression, Expression);
Expression sub(Expression, Expression);
Expression mul(Expression, Expression);
Expression divide(Expression, Expression);
Expression neg(Expression);
Expression lt(Expression, Expression);
Expression eq(Expression, Expression);
Expression leq(Expression, Expression);
Expression comp(Expression);
Expression int_const(Symbol);
Expression bool_const(Boolean);
Expression string_const(Symbol);
Expression new_(Symbol);
Expression isvoid(Expression);
Expression no_expr();
Expression object(Symbol);


#endif
