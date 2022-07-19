
//==============================================================================
// FILE:
//   StdOptionalMigration.cpp
//
// DESCRIPTION:
//   Clang-tidy plugin to help convert uses of boost::optional to std::optional
//
// USAGE:
//   TODO
//
// License: The Unlicense
//==============================================================================

#include <memory>

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
  static clang::transformer::RewriteRule rule() {
    using namespace clang::ast_matchers;
    using namespace clang::transformer;
    std::string exprBind = "expr";
    std::string memberBind = "member";
    auto onBoostOptional = on(
      expr(
        hasType(cxxRecordDecl(hasName("::boost::optional"), isTemplateInstantiation()))
      ).bind(exprBind));
    std::vector<clang::transformer::RewriteRule> rules;
    for (auto&& r: std::vector<std::pair<std::string, std::string>>{
          {"is_initialized", "has_value"},
          {"get", "value"},
        }) {
      auto&& target = r.first;
      auto&& replacement = r.second;
      rules.push_back(
          makeRule(
              cxxMemberCallExpr(
                isExpansionInMainFile(),
                onBoostOptional,
                callee(cxxMethodDecl(hasName(target)).bind(memberBind))),
              changeTo(cat(access(exprBind, replacement), "()")),
              cat("For boost::optional, prefer `", replacement,  "` to `", target, "`")));
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

clang::tidy::ClangTidyModuleRegistry::Add<Module>
    X("mongo-std-optional-migration-module",
      "Converts uses of boost::optional to std::optional.");

}  // namespace mongo_std_optional_migration
}  // namespace

// Trick from clang-tidy to force module link?
volatile int mongoStdOptionalMigrationModuleAnchorSource = 0;
