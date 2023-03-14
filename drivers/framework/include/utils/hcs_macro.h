/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup DriverConfig
 * @{
 *
 * @brief Defines APIs for HDF driver developers to read driver configuration information.
 *
 * During version compilation of the device resource source file defined by developers, the compilation tool
 * (for example, the compilation tool of the HCS file is hc-gen) generates bytecodes. When the HDF starts,
 * it transfers the bytecode memory to the <b>DriverConfig</b> module. The <b>DriverConfig</b> module converts
 * the bytecodes into a configuration tree and provides APIs for developers to query the tree.
 *
 * @since 1.0
 */

/**
 * @file hcs_macro.h
 *
 * @brief Defines the macro-based APIs used for querying the configuration tree.
 *
 * During version compilation, the hc-gen tool can be used to parse the configuration tree into macro definitions.
 * The following types of macro definitions are available:
 * 1. Macro definition for determining whether a node exists in the configuration tree: The macro name is a string
 * consisting of the node name and <b>_exists</b>. The value of the macro definition is <b>1</b>.
 * 2. Macro definition for obtaining the node attributes in the configuration tree. The macro name is a string
 * consisting of the node name and attribute name. The macro value is the attribute of the node.
 * 3. Macro definition for determining whether a node attribute exists. The macro name is a string consisting of
 * the node attribute name and <b>_exists</b>. The value of the macro definition is <b>1</b>.
 * 4. Macro definition for traversing the nodes in the configuration tree. The macro name is a string consisting of
 * the node name and <b>_foreach_child</b>. The value of the macro definition is <b>func (child node)</b>.
 * 5. Macro definition for obtaining the array type attribute of the configuration tree. The macro name is a string
 * consisting of the node attribute name and <b>_data</b>. The value of the macro definition is the array type
 * attribute of the node.
 * 6. Macro definition for obtaining the array size of a node in the configuration tree. The macro name is a string
 * consisting of the node attribute name and <b>_array_size</b>. The value of the macro definition is the array
 * size of the node.
 *
 * The macro definition of the configuration tree node name is a string consisting of the parent node name and
 * the node name. The macro definition of the configuration tree node attribute name is a string consists of
 * the node name and attribute name.
 *
 * The driver uses the macro-based APIs to concatenate name strings to macro definition names and obtain the macro
 * definition values, that is, the attribute values of the configuration tree.
 *
 * Macro definition example: #define root_device_info_match_attr "hdf_manager"
 * Example of using macro-based APIs to obtain the configuration:
 * printf("%s", HCS_PROP(HCS_NODE(HCS_ROOT, device_info), match_attr)); -- The output is <b>hdf_manager</b>.
 * @since 1.0
 */

#ifndef HCS_MACRO_H
#define HCS_MACRO_H

/**
 * @brief Concatenates parameter 1 and parameter 2 into a string.
 *
 * @param part1 Indicates parameter 1.
 * @param part2 Indicates parameter 2.
 * @since 1.0
 */
#define HCS_CAT(part1, part2) part1 ## part2

/**
 * @brief Defines the root node of the configuration tree.
 *
 * @since 1.0
 */
#define HCS_ROOT root

/**
 * @brief Checks whether a node exists in the configuration tree.
 *
 * @param node Indicates the node to check.
 * @since 1.0
 */
#define HCS_NODE_EXISTS(node) HCS_CAT(node, _exists)

/**
 * @brief Obtains the macro definition of a node in the configuration tree.
 *
 * @param parent Indicates the parent node of the target node.
 * @param node Indicates the target node.
 * @since 1.0
 */
#define HCS_NODE(parent, node) HCS_CAT(parent, _##node)

/**
 * @brief Checks whether a configuration node attribute exists.
 *
 * @param node Indicates the target node.
 * @param prop Indicates the node attribute to check.
 * @since 1.0
 */
#define HCS_NODE_HAS_PROP(node, prop) HCS_CAT(node, _##prop##_exists)

/**
 * @brief Obtains the macro definition of a node attribute in the configuration tree.
 *
 * @param node Indicates the target node.
 * @param prop Indicates the attribute name.
 * @since 1.0
 */
#define HCS_PROP(node, prop) HCS_CAT(node, _##prop)

/**
 * @brief Calls a function to traverse a node in the configuration tree.
 *
 * @param node Indicates the target node.
 * @param func Indicates the function to call.
 * @since 1.0
 */
#define HCS_FOREACH_CHILD(node, func) \
    HCS_CAT(node, _foreach_child)(func)

/**
 * @brief Calls a function with passed variables to traverse a node in the configuration tree.
 *
 * @param node Indicates the target node.
 * @param func Indicates the function to call. Variables can be passed to the function to be called.
 * @since 1.0
 */
#define HCS_FOREACH_CHILD_VARGS(node, func, ...) \
    HCS_CAT(node, _foreach_child_vargs)(func, __VA_ARGS__)

/**
 * @brief Obtains the attribute value of the node array type in the configuration tree.
 *
 * @param arrays_node Indicates the attribute name of the node array type.
 * @since 1.0
 */
#define HCS_ARRAYS(arrays_node) HCS_CAT(arrays_node, _data)

/**
 * @brief Obtain the size of the attribute value of the node array type in the configuration tree.
 *
 * @param arrays_node Indicates the attribute name of the node array type.
 * @since 1.0
 */
#define HCS_ARRAYS_SIZE(arrays_node) HCS_CAT(arrays_node, _array_size)

#endif // HCS_MACRO_H
