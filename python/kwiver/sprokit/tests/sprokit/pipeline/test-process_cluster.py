#!/usr/bin/env python
# ckwg +28
# Copyright 2012-2020 by Kitware, Inc.
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
        import kwiver.vital.config
        import kwiver.sprokit.pipeline.process
        import kwiver.sprokit.pipeline.process_cluster
    except:
        test_error("Failed to import the process_cluster module")


def test_api_calls():
    from kwiver.vital.config import config
    from kwiver.sprokit.pipeline import process
    from kwiver.sprokit.pipeline import process_cluster

    process_cluster.PythonProcessCluster.property_no_threads
    process_cluster.PythonProcessCluster.property_no_reentrancy
    process_cluster.PythonProcessCluster.property_unsync_input
    process_cluster.PythonProcessCluster.property_unsync_output
    process_cluster.PythonProcessCluster.type_any
    process_cluster.PythonProcessCluster.type_none
    process_cluster.PythonProcessCluster.type_data_dependent
    process_cluster.PythonProcessCluster.type_flow_dependent
    process_cluster.PythonProcessCluster.flag_output_const
    process_cluster.PythonProcessCluster.flag_output_shared
    process_cluster.PythonProcessCluster.flag_input_static
    process_cluster.PythonProcessCluster.flag_input_mutable
    process_cluster.PythonProcessCluster.flag_input_nodep
    process_cluster.PythonProcessCluster.flag_required

    class BaseProcess(process.PythonProcess):
        def __init__(self, conf):
            process.PythonProcess.__init__(self, conf)

    c = config.empty_config()

    p = BaseProcess(c)

    if process_cluster.cluster_from_process(p) is not None:
        test_error("A non-cluster process was detected as a cluster process")


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        test_error("Expected two arguments")
        sys.exit(1)

    testname = sys.argv[1]

    run_test(testname, find_tests(locals()))
