// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_PROPERTY_TREE_IPP
#define KTH_PROPERTY_TREE_IPP

#include <string>

#include <kth/domain/define.hpp>

namespace kth::domain::config {

void inline add_child(pt::ptree& out_list, std::string const& name, const pt::ptree& element, bool json) {
    if (json) {
        // HACK: work around invalid assertions in boost::property_tree.
        // It is sufficient to call push_back as long as the name is empty.
        out_list.push_back({"", element});
        ////out_list.add_child("", element);
    } else {
        out_list.add_child(name, element);
    }
}

/**
 * Create a property tree array of property tree elements.
 * The child elements of Value contain no arrays.
 * @param      <Values>  The array element type.
 * @param[in]  name      The name of the list elements.
 * @param[in]  values    The enumerable with elements of type Values.
 * @param[in]  json      Use json array formating.
 * @returns              A new property tree containing the list.
 */
template <typename Values>
pt::ptree property_tree_list(std::string const& name, const Values& values, bool json) {
    pt::ptree list;
    for (auto const& value : values) {
        add_child(list, name, property_list(value), json);
    }
    return list;
}

/**
 * Create a property tree array of property tree elements.
 * The child elements of Value contain arrays.
 * @param      <Values>  The array element type.
 * @param[in]  name      The name of the list elements.
 * @param[in]  values    The enumerable with elements of type Values.
 * @param[in]  json      Use json array formating.
 * @returns              A new property tree containing the list.
 */
template <typename Values>
pt::ptree property_tree_list_of_lists(std::string const& name, const Values& values, bool json) {
    pt::ptree list;
    for (auto const& value : values) {
        add_child(list, name, property_list(value, json), json);
    }
    return list;
}

/**
 * Create a property tree array of value elements.
 * @param      <Values>  The array element type.
 * @param[in]  name      The name of the list elements.
 * @param[in]  values    The enumerable with elements of type Values.
 * @param[in]  json      Use json array formating.
 * @returns              A new property tree containing the list.
 */
template <typename Values>
pt::ptree property_value_list(std::string const& name, const Values& values, bool json) {
    pt::ptree list;
    pt::ptree element;
    for (auto const& value : values) {
        element.put_value(value);
        add_child(list, name, element, json);
    }

    return list;
}

} // namespace kth::domain::config

#endif
