/*************************************************************************
 * Copyright (C) 2018-2019 Blue Brain Project
 *
 * This file is part of NMODL distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/

#pragma once

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "pybind11/embed.h"

namespace nmodl {
namespace pybind_wrappers {


    struct py_executor {
        struct input { };
        struct output { };

        virtual ~py_executor() { }

        virtual void operator()() = 0;
    };


    struct solve_linear_system_executor : public py_executor {
        // input
        std::vector<std::string> eq_system;
        std::vector<std::string> state_vars;
        std::set<std::string> vars;
        bool small_system;
        bool elimination;
        std::set<std::string> function_calls;
        // output
        // returns a vector of solutions, i.e. new statements to add to block:
        std::vector<std::string> solutions;
        // and a vector of new local variables that need to be declared in the block:
        std::vector<std::string> new_local_vars;
        // may also return a python exception message:
        std::string exception_message;

        // executor function
        virtual void operator()() override;
    };

    struct solve_non_linear_system_executor : public py_executor {
        // input
        std::vector<std::string> eq_system;
        std::vector<std::string> state_vars;
        std::set<std::string> vars;
        std::set<std::string> function_calls;
        // output
        // returns a vector of solutions, i.e. new statements to add to block:
        std::vector<std::string> solutions;
        // may also return a python exception message:
        std::string exception_message;

        // executor function
        virtual void operator()() override;
    };

    struct diffeq_solver_executor : public py_executor {
        // input
        std::string node_as_nmodl;
        std::string dt_var;
        std::set<std::string> vars;
        bool use_pade_approx;
        std::set<std::string> function_calls;
        std::string method;
        // output
        // returns  solution, i.e. new statement to add to block:
        std::string solution;
        // may also return a python exception message:
        std::string exception_message;

        // executor function
        virtual void operator()() override;

    };

    struct analytic_diff_executor : public py_executor {
        // input

        std::vector<std::string> expressions;
        std::set<std::string> used_names_in_block;
        // output
        // returns  solution, i.e. new statement to add to block:
        std::string solution;
        // may also return a python exception message:
        std::string exception_message;

        // executor function
        virtual void operator()() override;

    };


    solve_linear_system_executor* create_sls_executor();
    solve_non_linear_system_executor* create_nsls_executor();
    diffeq_solver_executor* create_des_executor();
    analytic_diff_executor* create_ads_executor();
    void destroy_sls_executor(solve_linear_system_executor* exec);
    void destroy_nsls_executor(solve_non_linear_system_executor* exec);
    void destroy_des_executor(diffeq_solver_executor* exec);
    void destroy_ads_executor(analytic_diff_executor* exec);

    void initialize_interpreter();
    void finalize_interpreter();

        struct pybind_wrap_api {
        decltype(&initialize_interpreter) initialize_interpreter;
        decltype(&finalize_interpreter) finalize_interpreter;
        decltype(&create_sls_executor) create_sls_executor;
        decltype(&create_nsls_executor) create_nsls_executor;
        decltype(&create_des_executor) create_des_executor;
        decltype(&create_ads_executor) create_ads_executor;
        decltype(&destroy_sls_executor) destroy_sls_executor;
        decltype(&destroy_nsls_executor) destroy_nsls_executor;
        decltype(&destroy_des_executor) destroy_des_executor;
        decltype(&destroy_ads_executor) destroy_ads_executor;
    };

    /**
     * A singleton class handling access to the pybind_wrap_api struct
     *
     * This class manages the runtime loading of the libpython so/dylib file and the python binding wrapper
     * library and provides access to the API wrapper struct that can be used to access the pybind11
     * embedded python functionality.
     */
    class EmbeddedPythonLoader {

    public:
        /**
         * Construct (if not already done) and get the only instance of this class
         *
         * @return the EmbeddedPythonLoader singleton instance
         */
        static EmbeddedPythonLoader& get_instance() {
            static EmbeddedPythonLoader instance;

            return instance;
        }

        EmbeddedPythonLoader(const EmbeddedPythonLoader&) = delete;
        void operator=(const EmbeddedPythonLoader&) = delete;


        /**
         * Get a pointer to the pybind_wrap_api struct
         *
         * Get access to the container struct for the pointers to the functions in the wrapper library.
         * @return a pybind_wrap_api pointer
         */
        const pybind_wrap_api* api();

        ~EmbeddedPythonLoader() {
            unload();
        }
    private:
        pybind_wrap_api* wrappers = nullptr;

        void* pylib_handle = nullptr;
        void* pybind_wrapper_handle = nullptr;

        bool have_wrappers();
        void load_libraries();
        void populate_symbols();
        void unload();

        EmbeddedPythonLoader() {
            if (!have_wrappers()) {
                load_libraries();
                populate_symbols();
            }
        }

    };
}
}