#!/usr/bin/env python
# ckwg +28
# Copyright 2011-2020 by Kitware, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
#  * Neither name of Kitware, Inc. nor the names of any contributors may be used
#    to endorse or promote products derived from this software without specific
#    prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from kwiver.sprokit.util.test import find_tests, run_test, test_error


def test_import():
    try:
        import kwiver.sprokit.pipeline.edge
    except:
        test_error("Failed to import the edge module")


def test_create():
    from kwiver.vital.config import config
    from kwiver.sprokit.pipeline import edge

    c = config.empty_config()

    edge.Edge()
    edge.Edge(c)
    edge.Edges()


def test_datum_create():
    from kwiver.sprokit.pipeline import datum
    from kwiver.sprokit.pipeline import edge
    from kwiver.sprokit.pipeline import stamp

    d = datum.complete()
    s = stamp.new_stamp(1)

    edge.EdgeDatum()
    edge.EdgeDatum(d, s)
    edge.EdgeData()


def test_api_calls():
    from kwiver.vital.config import config
    from kwiver.sprokit.pipeline import datum
    from kwiver.sprokit.pipeline import edge
    from kwiver.vital.modules import modules
    from kwiver.sprokit.pipeline import process
    from kwiver.sprokit.pipeline import process_factory
    from kwiver.sprokit.pipeline import stamp

    e = edge.Edge()

    e.makes_dependency()
    e.has_data()
    e.full_of_data()
    e.datum_count()

    d = datum.complete()
    s = stamp.new_stamp(1)

    ed = edge.EdgeDatum(d, s)

    e.push_datum(ed)
    e.get_datum()

    e.push_datum(ed)
    e.peek_datum()
    e.pop_datum()

    modules.load_known_modules()

    p = process_factory.create_process("orphan", "process_name")

    e.set_upstream_process(p)
    e.set_downstream_process(p)

    e.mark_downstream_as_complete()
    e.is_downstream_complete()

    e.config_dependency
    e.config_capacity


def test_datum_api_calls():
    from kwiver.sprokit.pipeline import datum
    from kwiver.sprokit.pipeline import edge
    from kwiver.sprokit.pipeline import stamp

    d = datum.complete()
    s = stamp.new_stamp(1)

    ed = edge.EdgeDatum(d, s)

    ed.datum
    ed.datum = d
    ed.stamp
    ed.stamp = s


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        test_error("Expected two arguments")
        sys.exit(1)

    testname = sys.argv[1]

    run_test(testname, find_tests(locals()))
