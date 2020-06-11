/*************************************************************************
 * Copyright (C) 2018-2019 Blue Brain Project
 *
 * This file is part of NMODL distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/

#include "catch/catch.hpp"

#include "ast/program.hpp"
#include "parser/nmodl_driver.hpp"
#include "test/utils/nmodl_constructs.hpp"
#include "visitors/ispc_rename_visitor.hpp"
#include "visitors/lookup_visitor.hpp"
#include "visitors/symtab_visitor.hpp"

using namespace nmodl;
using namespace visitor;
using namespace test_utils;

using ast::AstNodeType;
using nmodl::parser::NmodlDriver;
using symtab::syminfo::NmodlType;

//=============================================================================
// GlobalToRange visitor tests
//=============================================================================

std::shared_ptr<ast::Program> run_ispc_rename_visitor(const std::string& text) {
    NmodlDriver driver;
    auto ast = driver.parse_string(text);

    IspcRenameVisitor(ast).visit_program(*ast);
    SymtabVisitor().visit_program(*ast);
    return ast;
}

SCENARIO("Rename variables that ISPC parses as double constants", "[visitor][ispcrename]") {
    GIVEN("mod file with variables with names that resemble for ISPC double constants") {
        std::string input_nmodl = R"(
            NEURON {
                SUFFIX test
                RANGE d1, d2, var_d3
            }
            INITIAL {
                 d1 = 1
                 d2 = 2
                 var_d3 = 3
            }
        )";
        auto ast = run_ispc_rename_visitor(input_nmodl);
        auto symtab = ast->get_symbol_table();
        THEN(
            "Variables that match the constant double presentation in ISPC are renamed to "
            "var_<original_name>") {
            /// check if var_d1 and var_d2 exist
            auto var_d1 = symtab->lookup("var_d1");
            REQUIRE(var_d1 != nullptr);
            auto var_d2 = symtab->lookup("var_d2");
            REQUIRE(var_d2 != nullptr);
        }
        THEN("Variables that don't match the constant double presentation in ISPC stay the same") {
            /// check if var_d3
            auto var_d1 = symtab->lookup("var_d3");
            REQUIRE(var_d1 != nullptr);
        }
    }
}
