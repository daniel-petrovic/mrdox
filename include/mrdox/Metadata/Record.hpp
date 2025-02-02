//
// This is a derivative work. originally part of the LLVM Project.
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Official repository: https://github.com/cppalliance/mrdox
//

#ifndef MRDOX_METADATA_RECORD_HPP
#define MRDOX_METADATA_RECORD_HPP

#include <mrdox/Platform.hpp>
#include <mrdox/Metadata/Access.hpp>
#include <mrdox/ADT/BitField.hpp>
#include <mrdox/Metadata/Field.hpp>
#include <mrdox/Metadata/Reference.hpp>
#include <mrdox/Metadata/Scope.hpp>
#include <mrdox/Metadata/Symbol.hpp>
#include <mrdox/Metadata/Template.hpp>
#include <mrdox/Metadata/Symbols.hpp>
#include <clang/AST/Type.h>
#include <llvm/ADT/SmallVector.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace clang {
namespace mrdox {

/** Bit constants used with Record metadata
*/
union RecFlags0
{
    BitFieldFullValue raw{.value=0u};

    BitFlag<0> isFinal;
    BitFlag<1> isFinalDestructor;
};

/** Metadata for a direct base.
*/
struct BaseInfo
{
    SymbolID id;
    std::string Name;
    Access access;
    bool IsVirtual;

    BaseInfo(
        SymbolID const& id_ = SymbolID::zero,
        std::string_view Name_ = "",
        Access access_ = Access::Public,
        bool IsVirtual_ = false)
        : id(id_)
        , Name(Name_)
        , access(access_)
        , IsVirtual(IsVirtual_)
    {
    }
};

/** Members of a class, struct, or union.
*/
struct RecordScope
{
    std::vector<MemberRef> Records;
    std::vector<MemberRef> Functions;
    std::vector<MemberRef> Enums;
    std::vector<MemberRef> Types;
    std::vector<MemberRef> Fields;
    std::vector<MemberRef> Vars;
};

/** Metadata for struct, class, or union.
*/
struct RecordInfo
    : SymbolInfo
{
    // VFALCO Use our own enumeration for this
    // Type of this record (struct, class, union, interface).
    TagTypeKind TagType = TagTypeKind::TTK_Struct;

    // When present, this record is a template or specialization.
    std::optional<TemplateInfo> Template;

    // Indicates if the record was declared using a typedef. Things like anonymous
    // structs in a typedef:
    //   typedef struct { ... } foo_t;
    // are converted into records with the typedef as the Name + this flag set.
    // KRYSTIAN FIXME: this does not account for alias-declarations
    bool IsTypeDef = false;

    RecFlags0 specs;

    /** List of immediate bases.
    */
    std::vector<BaseInfo> Bases;

    /** List of friend functions.
    */
    llvm::SmallVector<SymbolID, 4> Friends;

    /** Record members
    */
    RecordScope Members;

    //--------------------------------------------

    static constexpr InfoType type_id = InfoType::IT_record;

    explicit
    RecordInfo(
        SymbolID id = SymbolID::zero,
        llvm::StringRef Name = llvm::StringRef())
        : SymbolInfo(InfoType::IT_record, id, Name)
    {
    }
};

} // mrdox
} // clang

#endif
