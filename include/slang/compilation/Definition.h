//------------------------------------------------------------------------------
//! @file Definition.h
//! @brief Module / interface / program definitions
//
// File is under the MIT license; see LICENSE for details
//------------------------------------------------------------------------------
#pragma once

#include <flat_hash_map.hpp>

#include "slang/binding/Lookup.h"
#include "slang/symbols/SemanticFacts.h"
#include "slang/util/SmallVector.h"
#include "slang/util/Util.h"

namespace slang {

class AttributeSymbol;
class Compilation;
class Expression;
class NetType;
class Scope;
class SyntaxTree;
class Type;

struct DeclaratorSyntax;
struct ModuleDeclarationSyntax;
struct ParameterDeclarationSyntax;
struct TypeAssignmentSyntax;
struct TypeParameterDeclarationSyntax;

class Definition {
public:
    struct ParameterDecl {
        union {
            const ParameterDeclarationSyntax* valueSyntax;
            const TypeParameterDeclarationSyntax* typeSyntax;
            const Type* givenType;
        };

        union {
            const DeclaratorSyntax* valueDecl;
            const TypeAssignmentSyntax* typeDecl;
            const Expression* givenInitializer;
        };

        string_view name;
        SourceLocation location;
        bool isTypeParam;
        bool isLocalParam;
        bool isPortParam;
        bool hasSyntax;

        ParameterDecl(const Scope& scope, const ParameterDeclarationSyntax& syntax,
                      const DeclaratorSyntax& decl, bool isLocal, bool isPort);
        ParameterDecl(const Scope& scope, const TypeParameterDeclarationSyntax& syntax,
                      const TypeAssignmentSyntax& decl, bool isLocal, bool isPort);
        ParameterDecl(string_view name, SourceLocation location, const Type& givenType,
                      bool isLocal, bool isPort, const Expression* givenInitializer);
        ParameterDecl(string_view name, SourceLocation location, bool isLocal, bool isPort,
                      const Type* defaultType);

        bool hasDefault() const;
    };

    struct PortDecl {
        string_view name;
        uint32_t index;
        bool likelyInterface;

        PortDecl(string_view name, uint32_t index, bool likelyInterface) :
            name(name), index(index), likelyInterface(likelyInterface) {}
    };

    string_view name;
    SourceLocation location;
    const ModuleDeclarationSyntax& syntax;
    const NetType& defaultNetType;
    const Scope& scope;
    SymbolIndex indexInScope;
    DefinitionKind definitionKind;
    VariableLifetime defaultLifetime;
    UnconnectedDrive unconnectedDrive;
    TimeScale timeScale;
    SmallVectorSized<ParameterDecl, 8> parameters;
    flat_hash_set<string_view> modports;
    span<const AttributeSymbol* const> attributes;
    const SyntaxTree* syntaxTree;

    Definition(const Scope& scope, LookupLocation lookupLocation,
               const ModuleDeclarationSyntax& syntax, const NetType& defaultNetType,
               UnconnectedDrive unconnectedDrive, optional<TimeScale> directiveTimeScale,
               const SyntaxTree* syntaxTree);

    span<const PortDecl> getPorts() const {
        if (!resolvedPorts)
            resolvePorts();
        return ports;
    }

private:
    void resolvePorts() const;

    mutable SmallVectorSized<PortDecl, 8> ports;
    mutable bool resolvedPorts = false;
};

} // namespace slang
