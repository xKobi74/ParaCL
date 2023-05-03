%{
    #include <iostream>
    #include <list>
    #include <string>
    #include <vector>
    #include <typeinfo>
    #include <type_traits>
    #include <chrono>

    #include "pcl_bison.hpp"
    #include "../paracl/memory_manager.hpp"

    #include <boost/program_options.hpp>
    namespace po = boost::program_options;
    #include <iterator>
    #include <fstream>

    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
   
    extern int yylineno;
    extern int yylex();
    extern FILE * yyin;

    std::vector<ptree::PTree*> blocks;
    ptree::Block* tmp;
    unsigned long offset = 0;
    int blk_num = 1;


    

%}

%token IF ELSE WHILE PRINT INPUT MOD
%token EQ LE GE NE AND OR NOT GREAT LESS
%token NUM ID  P_PLUS P_MINUS SEQUENCE
%token LPAR RPAR LBR RBR LCB RCB
%token ASSIGN PLUS MINUS MUL DIV

%type<str> NUM ID
%type<oper>  OP1 OP2 OP
%type<oper> EXPR EXPR1 EXPR2 EXPR3 TERM VAL 
%type<lval> VAR
%type<blk> BLOCK SCOPE OPS
%type<cnd> COND


%%

PROGRAM: BLOCK                            // обработка дерева программы
;
                                                                                                    
BLOCK: OPS                              { tmp = new ptree::Block(std::move(*$1)); delete $1; tmp->update_blk_info(offset++, blk_num++); blocks.push_back(tmp); $$ = tmp;}//unique pointer wiil fit good
;

OPS:    OP                              {tmp = new ptree::Block(); tmp->push_expression($1); $$ = tmp;}
|       OPS OP                          {tmp = new ptree::Block(std::move(*$1)); delete $1; tmp->push_expression($2); $$ = tmp;} //here just a version of block, which provides compilation
;

SCOPE:   LCB RCB                          { $$ = new ptree::Block();}
|        LCB BLOCK RCB                    { $$ = $2; }

OP1:    SCOPE                             {$$ = $1;}
|       EXPR SEQUENCE                     { $$ = new ptree::Expression(nullptr, $1);}
|       IF LPAR COND RPAR OP1 ELSE OP1    { $$ = new ptree::IfBlk($3, nullptr, wrap_block($7), wrap_block($5));}
|       WHILE LPAR COND RPAR OP1          { $$ = new ptree::WhileBlk($3, nullptr, wrap_block($5));}
;

OP2:    IF LPAR COND RPAR OP              { $$ = new ptree::IfBlk($3, nullptr, nullptr, wrap_block($5)); }
|       IF LPAR COND RPAR OP1 ELSE OP2    { $$ = new ptree::IfBlk($3, nullptr, wrap_block($7), wrap_block($5)); }
|       WHILE LPAR COND RPAR OP2          { $$ = new ptree::WhileBlk($3, nullptr, wrap_block($5)); }
;

COND:  EXPR                               {$$ = new ptree::Condition(nullptr, $1);}

OP:     OP1 | OP2 ;                     // inherit to solve C problem with if block

EXPR:   EXPR1                           // inherit
|       VAR ASSIGN EXPR                  { $$ = new ptree::Assign(nullptr, $1, $3); }
|       PRINT EXPR                       { $$ = new ptree::Output(nullptr, $2);}

EXPR1: EXPR2                           //inherit
|      EXPR1 AND EXPR2                 { $$ = new ptree::BinOp(ptree::BinOpType::LOG_AND, nullptr, $1, $3); }
|      EXPR1 OR EXPR2                  { $$ = new ptree::BinOp(ptree::BinOpType::LOG_OR, nullptr, $1, $3); }
;

EXPR2:  EXPR3                           // inherit
|       EXPR2 EQ EXPR3                  { $$ = new ptree::BinOp(ptree::BinOpType::EQUAL, nullptr, $1, $3);}
|       EXPR2 LE EXPR3                  { $$ = new ptree::BinOp(ptree::BinOpType::LESS_EQUAL, nullptr, $1, $3); }
|       EXPR2 GE EXPR3                  { $$ = new ptree::BinOp(ptree::BinOpType::MORE_EQUAL, nullptr, $1, $3);}
|       EXPR2 NE EXPR3                  { $$ = new ptree::BinOp(ptree::BinOpType::NON_EQUAL, nullptr, $1, $3);}
|       EXPR2 GREAT EXPR3               { $$ = new ptree::BinOp(ptree::BinOpType::MORE, nullptr, $1, $3); }
|       EXPR2 LESS EXPR3                { $$ = new ptree::BinOp(ptree::BinOpType::LESS, nullptr, $1, $3); }
;

EXPR3:  TERM                            // inherit
|       EXPR3 PLUS TERM                  { $$ = new ptree::BinOp(ptree::BinOpType::ADDITION, nullptr, $1, $3); }
|       EXPR3 MINUS TERM                  { $$ = new ptree::BinOp(ptree::BinOpType::SUBTRACTION, nullptr, $1, $3); }
;

TERM:   VAL                             // inherit
|       TERM MUL VAL                    { $$ = new ptree::BinOp(ptree::BinOpType::MULTIPLICATION, nullptr, $1, $3); }
|       TERM DIV VAL                    { $$ = new ptree::BinOp(ptree::BinOpType::DIVISION, nullptr, $1, $3); }
|       TERM MOD VAL                    { $$ = new ptree::BinOp(ptree::BinOpType::REMAINDER, nullptr, $1, $3);}
;

VAR:    ID                              { $$ = new ptree::NameInt(nullptr, 0, $1);}

VAL:    NUM                             { $$ = new ptree::Imidiate<int>(nullptr, std::stoi($1));}
|       INPUT                           { $$ = new ptree::Reserved(nullptr, ptree::Reserved::Types::Input);}
|       MINUS VAR                       { $$ = new ptree::UnOp(ptree::UnOpType::MINUS, nullptr, $2);}
|       MINUS NUM                       { $$ = new ptree::Imidiate<int>(nullptr, -std::stoi($2));}
|       NOT VAL                         { $$ = new ptree::UnOp(ptree::UnOpType::NOT, nullptr, $2); }
|       VAR P_PLUS                      { $$ = new ptree::UnOp(ptree::UnOpType::POST_ADDITION, nullptr, $1); }
|       VAR P_MINUS                     { $$ = new ptree::UnOp(ptree::UnOpType::POST_SUBTRACTION, nullptr, $1); }
|       LPAR EXPR RPAR                  { $$ = $2; }
|       VAR                             { $$ = $1;}

;




%%



int main(int ac, char* av[]) { 
    bool opt_time = false;
    try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help, h", "shows help option")
        ("version, v", "shows program version")
        ("build, b", "only builds program without execute")
        ("dump-tree, d", "dumps built tree to file default: out.dot")
        ("dump-out", po::value<std::string>(), "sets output file name")
        ("time-stamp", "makes time measurements on building and execution")
        ("input-file", po::value<std::string>(), "input file")
    ;
    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;        
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }
    if (vm.count("version")) {
        std::cout << "ParaCL interpreter 1.0.2"<< std::endl;
        std::cout << "Authors: Ilya Gavrilin and Eugene Bogdanov" << std::endl;
        std::cout <<  std::endl;
        std::cout << "This is free software;There is NO warranty;" << std::endl;
        std::cout << "not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl;
        return 0;
    }

    if (vm.count("time-stamp")) {
        opt_time = true;
    }
    

    if (!vm.count("input-file")) {
        std::cout << "No input file provided" << std::endl;
        return -1;
    }
    FILE* fh;
    if ((fh = fopen(vm["input-file"].as<std::string>().c_str(), "r"))) yyin = fh;
    auto tstart = high_resolution_clock::now();
    int res = yyparse();
    ptree::MemManager  memfunc = ptree::manage_tree_mem(blocks.back());
    auto tfin = high_resolution_clock::now();
    
    if (opt_time) {
        std::cout << "Build finished, elapsed time: " << duration_cast<milliseconds>(tfin - tstart).count()
           << " ms" << std::endl;
    }

    if (vm.count("dump-tree")) {
        std::string dump = "digraph G {\n";
        dump += (blocks.back())->dump();
        dump += "}\n";
        
        std::string out = "out.dot";
        if (vm.count("dump-out")) out = vm["dump-out"].as<std::string>();
        
        std::ofstream f_out;
        f_out.open(out, std::ios::out);
        f_out << dump;
        f_out.close();

    }
    
    if (vm.count("build")) {
        std::cout << "Build finished, no error catched" << std::endl;
        return 0;
    } 

    tstart = high_resolution_clock::now();
    ptree::Stack* stack = new ptree::Stack{memfunc.getmaxstacksize()};
    (blocks.back())->execute(stack);
    tfin = high_resolution_clock::now();

    if (opt_time) {
        std::cout << "Execute finished, elapsed time: " << duration_cast<milliseconds>(tfin - tstart).count()
            << " ms" << std::endl;
    }

    delete stack;
    
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!" << std::endl;
    }

    return 0;
}