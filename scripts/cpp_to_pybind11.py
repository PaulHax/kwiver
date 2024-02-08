# This file is part of KWIVER, and is distributed under the
# OSI-approved BSD 3-Clause License. See top-level LICENSE file or
# https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

""" cpp_to_pybind11.py:

Reads a C++ header file and creates C++ files with pybind11 bindings for
the objects defined in the input file. cpp_to_pybind11.py uses pygccxml to parse
the input C++, and the generated C++ file contains functions used for generating
bindings.

To install the required dependencies:

python -v venv env
source env/bin/activate
pip install pygccxml castxml

This script was derived from https://gitlab.kitware.com/cmb/smtk/-/blob/master/utilities/python/cpp_to_pybind11.py
"""

import os
import sys
from pathlib import Path

# Find out the file location within the sources tree
this_module_dir_path = os.path.abspath(os.path.dirname(sys.modules[__name__].__file__))
# Add pygccxml package to Python path
sys.path.append(os.path.join(this_module_dir_path, "..", ".."))

from pygccxml import parser  # nopep8
from pygccxml import declarations  # nopep8
from pygccxml import utils  # nopep8

LICENSE_STRING = """// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.
"""

COMMAND_STRING = """// Generated using: """


def get_scope(namespace):
    """
    Returns a list of namespaces from global to the input namespace
    """
    scope = []
    current_namespace = namespace.parent
    while current_namespace != namespace.top_parent:
        scope.append(current_namespace.name)
        current_namespace = current_namespace.parent

    scope = scope[::-1]
    return scope


def full_class_name(class_):
    """
    Returns the absolute name of a class, with all nesting namespaces included
    """
    return "::".join(get_scope(class_) + [class_.name])


def mangled_name(obj):
    """
    Returns a string that is unique to the object and can be used as a variable
    name
    """
    return (
        "_".join(get_scope(obj) + [obj.name])
        .replace("<", "_")
        .replace(">", "_")
        .replace("::", "_")
    )


def get_path_relative_to_directory(path, directory):
    return os.path.relpath(
        os.path.abspath(path),
        os.path.commonprefix([os.path.abspath(directory), os.path.abspath(path)]),
    )


def create_trampoline(filename, project_source_directory, decls, stream):
    # grab global namespace
    try:
        global_ns = declarations.get_global_namespace(decls)
    except RuntimeError:
        print(f"create_trampoline: No classes found in {filename}")
        exit(1)

    fileguard = Path(filename).stem.replace(".", "_").upper() + "_TRAMPOLINE_TXX"

    # output file preamble
    stream(LICENSE_STRING)
    stream(COMMAND_STRING)
    stream(f"#ifndef {fileguard}")
    stream(f"#define {fileguard}")
    stream("")

    # includes
    stream("#include <pybind11/pybind11.h>")
    stream("#include <python/kwiver/vital/algo/trampoline/algorithm_trampoline.txx>")
    stream(
        "#include <%s>"
        % os.path.relpath(
            os.path.abspath(filename),
            os.path.commonprefix(
                [os.path.abspath(project_source_directory), os.path.abspath(filename)]
            ),
        )
    )
    stream("")

    # namespace
    stream("namespace kwiver::vital::python {")

    # generate trampoline class
    class_ = None
    for class_ in global_ns.classes(allow_empty=True):
        if class_.location.file_name != os.path.abspath(filename):
            continue
        if class_.parent and type(class_.parent).__name__.find("class_t") != -1:
            continue
        break

    stream(
        f"""
template< class {class_.name}_base = {full_class_name(class_)} >
class {class_.name}_trampoline
    : public algorithm_trampoline< {class_.name}_base >
{{
  public:
    using algorithm_trampoline< {class_.name}_base >::algorithm_trampoline;
"""
    )

    ## generate only the methods required for a trampoline class ie pure virtual
    for member in [
        *class_.private_members,
        *class_.protected_members,
        *class_.public_members,
    ]:
        if member.__class__.__name__ != "member_function_t":
            continue
        if member.virtuality != declarations.VIRTUALITY_TYPES.NOT_VIRTUAL:
            pybind11_macro = "PYBIND11_OVERLOAD"
            if member.virtuality == declarations.VIRTUALITY_TYPES.PURE_VIRTUAL:
                pybind11_macro = "PYBIND11_OVERLOAD_PURE"
            elif member.access_type == "private":
                pybind11_macro = "PYBIND11_OVERLOAD_PURE"  # This looks like a kwiver pattern: private non pure virtual methods become accessible by making overlading them as pure
            argument_string = ", ".join(
                [arg.decl_type.decl_string + " " + arg.name for arg in member.arguments]
            )
            argument_name_list = ", ".join([arg.name for arg in member.arguments])
            const_specifier = "const " if member.has_const else ""
            stream(
                f"""
  {member.return_type}
  {member.name}({argument_string}) {const_specifier}override
  {{
    {pybind11_macro}(
      {member.return_type},
      {full_class_name(class_)},
      {member.name},
      {argument_name_list}
      );
  }}"""
            )

    stream("}; // class")
    stream("} // namespace")
    stream("#endif")

    return {class_: f"{class_.name}_trampoline"}


def create_class_header(filename, project_source_directory, stream):
    """
    Generate a header for the pybind11 bindings of the class described in filename
    """
    header_path_relative_to_project = get_path_relative_to_directory(
        filename, project_source_directory
    )
    fileguard = (
        "KWIVER_PYTHON_"
        + header_path_relative_to_project.replace(".", "_")
        .replace("/", "_")
        .replace("-", "_")
        .upper()
    )

    stream(LICENSE_STRING)
    stream(COMMAND_STRING)
    stream(f"#ifndef {fileguard}")
    stream(f"#define {fileguard}")
    stream("")
    stream("#include <pybind11/pybind11.h>")
    stream("")
    stream("namespace kwiver::vital::python {")
    stream("namespace py = pybind11;")
    stream("")

    stream(f"void {Path(filename).stem}(py::module& m);")
    stream("}")
    stream("#endif")


def create_class_implementation(
    filename,
    class_name,
    extra_includes,
    project_source_directory,
    declaration_names,
    decls,
    stream,
):
    # grab global namespace
    try:
        global_ns = declarations.get_global_namespace(decls)
    except RuntimeError:
        print(f"create_class_implementation: No classes found in {filename}")
        exit(1)
    # output file preamble
    header_path_relative_to_project = os.path.relpath(
        os.path.abspath(filename),
        os.path.commonprefix(
            [os.path.abspath(project_source_directory), os.path.abspath(filename)]
        ),
    )

    trampoline_path = (
        Path("python/kwiver")
        / Path(*Path(header_path_relative_to_project).parts[:-1])
        / "trampoline"
        / Path(Path(filename).stem + "_trampoline.txx")
    )

    stream(LICENSE_STRING)
    stream(COMMAND_STRING)
    stream("")
    stream("#include <pybind11/pybind11.h>")
    for line in extra_includes:
        stream(f"#include {line}")

    stream("")
    stream(f"#include <{header_path_relative_to_project}>")
    stream(f"#include <python/kwiver/vital/algo/algorithm.txx>")
    stream(f"#include <{trampoline_path}>")
    stream("")
    # namespace
    stream("namespace kwiver::vital::python {")
    stream("namespace py = pybind11;")
    stream("")

    stream(f"void {Path(filename).stem}(py::module& m)")
    stream("{")

    for class_ in global_ns.classes(allow_empty=True):
        if class_.location.file_name != os.path.abspath(filename):
            continue
        if class_.parent and type(class_.parent).__name__.find("class_t") != -1:
            continue
        parse_class(class_, class_name, stream)
        break

    stream("")

    stream("}")


def parse_file(
    filename,
    project_source_directory,
    include_directories,
    declaration_names,
    class_name,
    extra_includes,
    stream_header,
    stream_impl,
    stream_tramp,
):
    """
    Entry point for parsing a file
    """
    # Find out the xml generator (gccxml or castxml)
    generator_path, generator_name = utils.find_xml_generator()

    # Configure the xml generator
    config = parser.xml_generator_configuration_t(
        start_with_declarations=declaration_names.split(" "),
        include_paths=include_directories.split(" "),
        xml_generator_path=generator_path,
        xml_generator=generator_name,
        cflags="-std=c++17 -DKWIVER_PYBIND11_WRAPPING",
        castxml_epic_version=1,  # required to be able to parse comments
    )

    # Parse source file
    declarations = parser.parse(
        [os.path.abspath(filename)],
        config,
        compilation_mode=parser.COMPILATION_MODE.ALL_AT_ONCE,
    )

    if stream_tramp:
        create_trampoline(
            filename, project_source_directory, declarations, stream_tramp
        )

    create_class_header(filename, project_source_directory, stream_header)
    create_class_implementation(
        filename,
        class_name,
        extra_includes,
        project_source_directory,
        declaration_names,
        declarations,
        stream_impl,
    )


def parse_free_enumeration(enum, stream):
    """
    Write bindings for a free enumeration
    """
    init_function_name = "pybind11_init_" + mangled_name(enum)
    stream("inline void %s(py::module &m)" % init_function_name)
    stream("{")
    full_enum_name = full_class_name(enum)
    stream('  py::enum_<%s>(m, "%s")' % (full_enum_name, enum.name))
    for name, num in enum.values:
        stream('    .value("%s", %s::%s)' % (name, full_enum_name, name))
    stream("    .export_values();")
    stream("}")
    return init_function_name


def parse_free_function(func, overloaded, stream):
    """
    Write bindings for a free function
    """
    init_function_name = "pybind11_init_" + (
        func.mangled if overloaded else mangled_name(func)
    )
    stream("inline void %s(py::module &m)" % init_function_name)
    stream("{")
    if overloaded:
        stream(
            '  m.def("%s", (%s (*)(%s)) &%s, "", %s);'
            % (
                func.name,
                func.return_type,
                ", ".join([arg.decl_type.decl_string for arg in func.arguments]),
                "::".join(get_scope(func) + [func.name]),
                ", ".join(
                    [
                        (
                            'py::arg("%s") = %s' % (arg.name, arg.default_value)
                            if arg.default_value is not None
                            else 'py::arg("%s")' % (arg.name)
                        )
                        for arg in func.arguments
                    ]
                ),
            )
        )
    else:
        stream(
            '  m.def("%s", &%s, "", %s);'
            % (
                func.name,
                "::".join(get_scope(func) + [func.name]),
                ", ".join(
                    [
                        (
                            'py::arg("%s") = %s' % (arg.name, arg.default_value)
                            if arg.default_value is not None
                            else 'py::arg("%s")' % (arg.name)
                        )
                        for arg in func.arguments
                    ]
                ),
            )
        )
    stream("}")
    return init_function_name


def has_static(class_):
    for member in class_.public_members:
        if member.parent.name != class_.name:
            continue
        if member.__class__.__name__ != "member_function_t":
            continue
        if member.has_static:
            return True

    for variable in class_.variables(allow_empty=True):
        if variable.parent.name != class_.name:
            continue
        if variable.access_type == "public" and variable.type_qualifiers.has_static:
            return True

    return False


def parse_class(class_, class_name, stream, top_level=True):
    """
    Write bindings for a class
    """
    full_class_name_ = full_class_name(class_)

    python_name = (
        class_name
        if class_name is not None
        else "".join(part.title() for part in class_.name.split("_"))
    )
    stream(
        f"""
    py::class_<{full_class_name_},
               std::shared_ptr<{full_class_name_}>,
               kwiver::vital::algorithm,
               {class_.name}_trampoline<> > instance(m,  "{python_name}");
    """
    )

    stream("    instance")

    if class_.is_abstract:
        stream("    .def(py::init<>())")
    else:
        for constructor in class_.constructors():
            print(dir(constructor))
            if constructor.parent.name != class_.name:
                continue
            if constructor.access_type != "public":
                continue
            types = ", ".join([str(arg.decl_type) for arg in constructor.arguments])
            names = ", ".join(
                [
                    f'py::arg("{arg.name}") = {arg.default_value}'
                    for arg in constructor.arguments
                ]
            )
            print(names)
            stream(f"    .def(py::init<{types}>(), {names})")

    all_methods = set()
    overloaded_methods = set()
    virtual_methods = set()
    for member in class_.public_members:
        if member.parent.name != class_.name:
            continue
        if member.__class__.__name__ != "member_function_t":
            continue
        if member.virtuality == declarations.VIRTUALITY_TYPES.VIRTUAL:
            virtual_methods.add(member.name)
        if member.name in all_methods:
            overloaded_methods.add(member.name)
        else:
            all_methods.add(member.name)

    # this code block separates method pairs that look like "get_XXX()" and
    # "setXXXX()" and flags them to be used as set-s and get-s to define a
    # property.
    use_properties = False
    property_set_methods = set()
    property_get_methods = set()
    if use_properties:

        def uncapitalize(s):
            return s[:1].lower() + s[1:] if s else ""

        property_set_methods = set(
            [
                m
                for m in class_.public_members
                if m.name[:3] == "set" and m.name not in virtual_methods
            ]
        )
        property_get_methods = set(
            [
                m
                for m in class_.public_members
                if m.name[:3] == "get" and m.name not in virtual_methods
            ]
        )

        for getter, setter in zip(property_get_methods, property_set_methods):
            name = getter.name[4:]
            stream(
                f'    .def_property("{name}", &%{full_class_name_}::{getter.name}, &{full_class_name_}::{setter.name})'
            )

    for member in class_.public_members:
        if member.parent.name != class_.name:
            continue
        if member.__class__.__name__ != "member_function_t":
            continue
        static = "_static" if member.has_static else ""
        const = " const" if member.has_const else ""
        doc = (
            (
                ", "
                + 'R"('
                + "\n".join(line for line in member.comment.text).replace("///", "")
                + ')"'
            )
            if len(member.comment.text) != 0
            else ""
        )
        args_ = (
            (
                ", "
                + ", ".join(
                    [
                        (
                            f'py::arg("{arg.name}") = {arg.default_value}'
                            if arg.default_value is not None
                            else f'py::arg("{arg.name}")'
                        )
                        for arg in member.arguments
                    ]
                )
            )
            if len(member.arguments) != 0
            else ""
        )
        if member in property_set_methods or member in property_get_methods:
            continue
        if member.name in overloaded_methods:
            args_declaration = ", ".join(
                [arg.decl_type.decl_string for arg in member.arguments]
            )
            method_specifier = "" if member.has_static else full_class_name_ + "::"
            stream(
                f'    .def{static}("{member.name}", ({member.return_type} ({method_specifier}*)({args_declaration}){const}) &{full_class_name_}::{member.name}{doc}{args_})'
            )
        else:
            stream(
                f'    .def{static}("{member.name}", &{full_class_name_}::{member.name}{doc}{args_})'
            )

    for variable in class_.variables(allow_empty=True):
        if variable.parent.name != class_.name:
            continue

        if variable.access_type == "public":
            static = "_static" if variable.type_qualifiers.has_static else ""
            if declarations.type_traits.is_const(variable.decl_type):
                stream(
                    '    .def_readonly%s("%s", &%s::%s)'
                    % (static, variable.name, full_class_name_, variable.name)
                )

        elif variable.access_type == "private" and variable.name[:2] == "c_":
            static = "_static" if variable.type_qualifiers.has_static else ""
            name = variable.name[2:]  # drop c_
            print(name)
            if declarations.is_const(variable):
                stream(
                    '    .def_readonly%s("%s", &%s::%s)'
                    % (static, variable.name, full_class_name_, variable.name)
                )
            else:
                stream(
                    f'    .def_property("{name}", &{full_class_name_}::get_{name}, &{full_class_name_}::set_{name})'
                )

    stream("    ;")

    for enum in class_.enumerations(allow_empty=True):
        stream(
            '  py::enum_<%s::%s>(%s, "%s")'
            % (full_class_name_, enum.name, "instance", enum.name)
        )
        for name, num in enum.values:
            stream(
                '    .value("%s", %s::%s::%s)'
                % (name, full_class_name_, enum.name, name)
            )
        stream("    .export_values();")

    for decl in class_.declarations:
        if type(decl).__name__.find("class_t") != -1:
            parse_class(decl, stream, False)

    stream(f"  register_algorithm< {full_class_name_} > (instance);")
    stream("}")


if __name__ == "__main__":
    import argparse

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument(
        "-I",
        "--include-dirs",
        help="Add an include directory to the parser",
        default="",
    )

    arg_parser.add_argument(
        "-d",
        "--declaration-name",
        help="names of C++ classes and functions",
        default="",
    )

    arg_parser.add_argument(
        "-i", "--input", help="<Required> Input C++ header file", required=True
    )
    arg_parser.add_argument(
        "-e",
        "--extra-includes",
        nargs="+",
        help="List of extra includes files for the implementation class. Usually, <pybind11/stl.h>",
        default=[],
    )

    arg_parser.add_argument(
        "-o", "--output", help="Basename of output C++ file(s)", required=True
    )

    arg_parser.add_argument(
        "-s", "--project-source-dir", help="Project source directory", default="."
    )

    arg_parser.add_argument(
        "-t",
        "--trampoline",
        help="Generate also the corresponding trampoline class",
        action="store_true",
    )
    arg_parser.add_argument(
        "-n",
        "--class-name",
        help="Python name for the wrapped class. By default the C++ name is converted to camelcase.",
        default=None,
    )

    arg_parser.add_argument(
        "-v", "--verbose", help="Print out generated wrapping code", action="store_true"
    )

    args = arg_parser.parse_args()

    command_args = " ".join(sys.argv)
    COMMAND_STRING += command_args

    if not args.include_dirs:
        args.include_dirs = args.input_directory

    def stream_with_line_breaks(stream):
        def write(string):
            stream.write(string.replace(">>", "> >"))
            stream.write("\n")

        def write_verbose(string):
            write(string)
            print(string.replace(">>", "> >"))

        if args.verbose:
            return write_verbose
        else:
            return write

    class_header = args.output + ".h"
    class_impl = args.output + ".cxx"
    class_trampoline = args.output + "_trampoline.txx"
    with open(class_header, "w") as h, open(class_impl, "w") as cxx, open(
        class_trampoline, "w"
    ) as tramp:
        stream_header = stream_with_line_breaks(h)
        stream_impl = stream_with_line_breaks(cxx)
        stream_tramp = stream_with_line_breaks(tramp) if args.trampoline else None
        parse_file(
            args.input,
            args.project_source_dir,
            args.include_dirs,
            args.declaration_name,
            args.class_name,
            args.extra_includes,
            stream_header,
            stream_impl,
            stream_tramp,
        )
