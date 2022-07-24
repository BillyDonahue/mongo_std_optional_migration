/**
 *    Copyright (C) 2022-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

/**
 * Clang-tidy plugin to help convert uses of boost::optional to std::optional
 */

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <clang-tidy/ClangTidyCheck.h>
#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>
#include <clang-tidy/utils/TransformerClangTidyCheck.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/OperationKinds.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Rewrite/Frontend/FixItRewriter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Transformer/RangeSelector.h>
#include <clang/Tooling/Transformer/RewriteRule.h>
#include <clang/Tooling/Transformer/Stencil.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>

namespace {
namespace mongo_std_optional_migration {

class ApiSubsetCheck : public clang::tidy::utils::TransformerClangTidyCheck {
public:
    ApiSubsetCheck(clang::StringRef name, clang::tidy::ClangTidyContext* context)
        : clang::tidy::utils::TransformerClangTidyCheck(rule(), name, context) {}

private:
    static clang::transformer::RewriteRule rule(const std::string& target,
                                                const std::string& replacement) {
        using namespace clang::ast_matchers;
        using namespace clang::transformer;
        std::string memberBind = "member";
        auto boostOptionalDecl =
            cxxRecordDecl(hasName("::boost::optional"), isTemplateInstantiation());
        auto isBoostOptional =
            qualType(anyOf(hasDeclaration(boostOptionalDecl), pointsTo(boostOptionalDecl)));
        return makeRule(cxxMemberCallExpr(isExpansionInMainFile(),
                                          on(hasType(isBoostOptional)),
                                          callee(cxxMethodDecl(hasName(target))),
                                          callee(memberExpr().bind(memberBind))),
                        changeTo(member(memberBind), cat(replacement)),
                        cat("For boost::optional, prefer `", replacement, "` to `", target, "`"));
    }

    static clang::transformer::RewriteRule rule() {
        std::vector<clang::transformer::RewriteRule> rules;
        for (auto&& r : std::vector<std::pair<std::string, std::string>>{
                 {"get", "value"},
                 {"is_initialized", "has_value"},
             }) {
            rules.push_back(rule(r.first, r.second));
        }
        return applyFirst(rules);
    }
};

class Module : public clang::tidy::ClangTidyModule {
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories& factories) override {
        factories.registerCheck<ApiSubsetCheck>("mongo-std-optional-migration-api-subset");
    }
};

clang::tidy::ClangTidyModuleRegistry::Add<Module> X(
    "mongo-std-optional-migration-module", "Converts uses of boost::optional to std::optional.");

// Trick from clang-tidy to force module link?
volatile int mongoStdOptionalMigrationModuleAnchorSource = 0;

}  // namespace mongo_std_optional_migration
}  // namespace
