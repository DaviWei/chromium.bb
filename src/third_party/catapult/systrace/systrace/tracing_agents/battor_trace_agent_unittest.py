#!/usr/bin/env python

# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from collections import namedtuple
import unittest
import logging

from systrace.tracing_agents import battor_trace_agent
from battor import battor_wrapper
from devil.android import battery_utils
from devil.utils import battor_device_mapping


mock_opts = namedtuple('mock_opts', ['target', 'device_serial_number',
                                     'hub_types', 'battor_path',
                                     'update_map', 'serial_map'])
OPTIONS = mock_opts('android', 'Phn2', ['plugable_7port'],
                    None, False, __file__)
CATEGORIES = None

def raise_error(*args, **kwargs):
  del args
  del kwargs
  raise RuntimeError('Should not call this function in the test')

battor_device_mapping.GenerateSerialMapFile = raise_error


def setup_battor_test(StartShell_error, StartTracing_error,
                      StopTracing_error, CollectTraceData_error):
  wrapper = MockBattorWrapper(StartShell_error, StartTracing_error,
                              StopTracing_error, CollectTraceData_error)
  def wrapper_maker(*args, **kwargs):
    del args
    del kwargs
    return wrapper
  battor_wrapper.BattorWrapper = wrapper_maker

class MockBattorWrapper(object):
  def __init__(self, StartShell_error=False, StartTracing_error=False,
               StopTracing_error=False, CollectTraceData_error=False):
    self._StartShell_error = StartShell_error
    self._StartTracing_error = StartTracing_error
    self._StopTracing_error = StopTracing_error
    self._CollectTraceData_error = CollectTraceData_error
    self._running = False
    self._tracing = False
    self._output = False

  def IsShellRunning(self):
    return self._running

  def StartShell(self):
    assert not self._running
    if self._StartShell_error:
      raise RuntimeError('Simulated error in StartShell')
    self._running = True

  def StartTracing(self):
    assert self._running
    assert not self._tracing
    if self._StartTracing_error:
      raise RuntimeError('Simulated error in StartTracing')
    self._tracing = True

  def StopTracing(self):
    assert self._running
    assert self._tracing
    if self._StopTracing_error:
      raise RuntimeError('Simulated error in StopTracing')
    self._running = False
    self._tracing = False
    self._output = True

  def CollectTraceData(self):
    assert self._output
    if self._CollectTraceData_error:
      raise RuntimeError('Simulated error in CollectTraceData')
    return 'traceout1\ntraceout2'

class MockBatteryUtils(object):
  def __init__(self, _):
    self._is_charging = True

  def GetCharging(self):
    return self._is_charging

  def SetCharging(self, value):
    self._is_charging = value

battery_utils.BatteryUtils = MockBatteryUtils

class BattorAgentTest(unittest.TestCase):

  def test_trace_double_start(self):
    setup_battor_test(StartShell_error=False, StartTracing_error=False,
                      StopTracing_error=False, CollectTraceData_error=False)
    agent = battor_trace_agent.BattorTraceAgent()
    agent.StartAgentTracing(OPTIONS, CATEGORIES)
    self.assertRaises(AssertionError,
                      lambda: agent.StartAgentTracing(OPTIONS, CATEGORIES))

  def test_trace_error_start_shell(self):
    setup_battor_test(StartShell_error=True, StartTracing_error=False,
                      StopTracing_error=False, CollectTraceData_error=False)
    agent = battor_trace_agent.BattorTraceAgent()
    self.assertRaises(RuntimeError,
                      lambda: agent.StartAgentTracing(OPTIONS, CATEGORIES))

  def test_trace_error_start_tracing(self):
    setup_battor_test(StartShell_error=False, StartTracing_error=True,
                      StopTracing_error=False, CollectTraceData_error=False)
    agent = battor_trace_agent.BattorTraceAgent()
    self.assertRaises(RuntimeError,
                      lambda: agent.StartAgentTracing(OPTIONS, CATEGORIES))

  def test_trace_error_stop_tracing(self):
    setup_battor_test(StartShell_error=False, StartTracing_error=False,
                      StopTracing_error=True, CollectTraceData_error=False)
    agent = battor_trace_agent.BattorTraceAgent()
    agent.StartAgentTracing(OPTIONS, CATEGORIES)
    self.assertRaises(RuntimeError, agent.StopAgentTracing)

  def test_trace_error_get_results(self):
    setup_battor_test(StartShell_error=False, StartTracing_error=False,
                      StopTracing_error=False, CollectTraceData_error=True)
    agent = battor_trace_agent.BattorTraceAgent()
    agent.StartAgentTracing(OPTIONS, CATEGORIES)
    agent.StopAgentTracing()
    self.assertRaises(RuntimeError, agent.GetResults)

  def test_trace_complete(self):
    setup_battor_test(StartShell_error=False, StartTracing_error=False,
                      StopTracing_error=False, CollectTraceData_error=False)
    agent = battor_trace_agent.BattorTraceAgent()
    agent.StartAgentTracing(OPTIONS, CATEGORIES)
    agent.StopAgentTracing()
    x = agent.GetResults()
    self.assertEqual(x.raw_data, 'traceout1\ntraceout2')

if __name__ == "__main__":
  logging.getLogger().setLevel(logging.DEBUG)
  unittest.main(verbosity=2)
