//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Official repository: https://github.com/cppalliance/mrdox
//

#include <mrdox/Corpus.hpp>
#include <mrdox/Metadata/Function.hpp>
#include <mrdox/Metadata/Namespace.hpp>
#include <mrdox/Metadata/Overloads.hpp>
#include <mrdox/Metadata/Scope.hpp>
#include <llvm/ADT/STLExtras.h>

namespace clang {
namespace mrdox {

NamespaceOverloads::
NamespaceOverloads(
    NamespaceInfo const& I,
    std::vector<FunctionInfo const*> data)
    : data_(std::move(data))
{
    // Sort to group the overloads, preserving order
    llvm::stable_sort(data_,
        []( FunctionInfo const* f0,
            FunctionInfo const* f1)
        {
            return compareSymbolNames(f0->Name, f1->Name) < 0;
        });

    // Find the end of the range of each overload set
    auto it0 = data_.begin();
    while(it0 != data_.end())
    {
        auto it = std::find_if_not(
            it0 + 1, data_.end(),
            [it0](auto I)
            {
                return llvm::StringRef((*it0)->Name).compare_insensitive(
                    llvm::StringRef(I->Name)) == 0;
            });
        list.emplace_back(OverloadInfo{
            &I,
            { (*it0)->Name.data(), (*it0)->Name.size() },
            { it0, it } });
        it0 = it;
    }
}

NamespaceOverloads
makeNamespaceOverloads(
    NamespaceInfo const& I,
    Corpus const& corpus)
{
    std::vector<FunctionInfo const*> data;
    data.reserve(I.Children.Functions.size());
    for(auto const& ref : I.Children.Functions)
    {
        auto const& I = corpus.get<FunctionInfo>(ref.id);
        data.push_back(&I);
    }

    return NamespaceOverloads(I, std::move(data));
}

} // mrdox
} // clang
