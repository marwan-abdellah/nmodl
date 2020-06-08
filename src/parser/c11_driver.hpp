/*************************************************************************
 * Copyright (C) 2018-2019 Blue Brain Project
 *
 * This file is part of NMODL distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/

#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>


namespace nmodl {
namespace parser {

/// flex generated scanner class (extends base lexer class of flex)
class CLexer;

/// parser class generated by bison
class CParser;

class location;

/**
 * @addtogroup parser
 * @{
 */

/**
 * \class CDriver
 * \brief Class that binds all pieces together for parsing C verbatim blocks
 */
class CDriver {
  private:
    /// all typedefs
    std::map<std::string, std::string> typedefs;

    /// constants defined in enum
    std::vector<std::string> enum_constants;

    /// all tokens encountered
    std::vector<std::string> tokens;

    /// enable debug output in the flex scanner
    bool trace_scanner = false;

    /// enable debug output in the bison parser
    bool trace_parser = false;

    /// pointer to the lexer instance being used
    CLexer* lexer = nullptr;

    /// pointer to the parser instance being used
    CParser* parser = nullptr;

    /// print messages from lexer/parser
    bool verbose = false;

  public:
    /// file or input stream name (used by scanner for position), see todo
    std::string streamname;

    CDriver() = default;
    CDriver(bool strace, bool ptrace);

    void error(const std::string& m);

    bool parse_stream(std::istream& in);
    bool parse_string(const std::string& input);
    bool parse_file(const std::string& filename);
    void scan_string(std::string& text);
    void add_token(const std::string&);

    void error(const std::string& m, const location& l);

    void set_verbose(bool b) {
        verbose = b;
    }

    bool is_verbose() const {
        return verbose;
    }

    bool is_typedef(std::string type) const {
        return typedefs.find(type) != typedefs.end();
    }

    bool is_enum_constant(std::string constant) const {
        return std::find(enum_constants.begin(), enum_constants.end(), constant) !=
               enum_constants.end();
    }

    std::vector<std::string> all_tokens() const {
        return tokens;
    }

    bool has_token(std::string token) {
        if (std::find(tokens.begin(), tokens.end(), token) != tokens.end()) {
            return true;
        }
        return false;
    }
};

/** @} */  // end of parser

}  // namespace parser
}  // namespace nmodl