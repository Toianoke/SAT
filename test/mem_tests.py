#!/usr/bin/env python3
import sys as SYS
import subprocess as SP
import multiprocessing as MP
import multiprocessing.pool as MPP
import logging as LOG
import argparse as AP
import os as OS
import os.path as PATH
import glob as GLOB
import re as RE

from time import time

#+------------------------------------------------------------------------------+
#| Output Formatting / Logging                                                  |
#+------------------------------------------------------------------------------+

# optional color printing
if SYS.stdout.isatty():
  DO_COLOR = True
else:
  DO_COLOR = False

def color_text(color_code, text):
  if DO_COLOR:
    return "{}{}{}".format(color_code, text, "\x1b[0m")
  else:
    return text

def red(text):
  return color_text("\x1b[31m", text)

def green(text):
  return color_text("\x1b[32m", text)



#+------------------------------------------------------------------------------+
#| Read setting for run (options in source file)                                |
#+------------------------------------------------------------------------------+

def get_settings_from_source(source_file, settings):
  with open(source_file, 'r') as f:
    for line in f.readlines():
      get_line_settings(r"(SATISFIABLE)", line, "expected", settings)
      get_line_settings(r"(UNSATISFIABLE)", line, "expected", settings)
      get_line_settings(r"(ERROR)", line, "expected", settings)


def get_line_settings(search_string, line, category, settings):
  match = RE.search(search_string, line)
  if match:
    settings[category] = match.group(1).strip()

#+------------------------------------------------------------------------------+
#| Read setting for run (options for subdirectory)                              |
#+------------------------------------------------------------------------------+

def get_settings_from_directory(directory, base_directory):
  configs = list()
  # For all directories between the given one and the base read the config.yaml
  while directory != base_directory:
    yaml_file = PATH.join(directory, "config.yaml")
    if PATH.isfile(yaml_file):
      with open(yaml_file, 'r') as f:
        settings = yaml.safe_load(f)
      configs.append(settings)
    directory = PATH.split(directory)[0]

  # The list is now deepest nested settings to least nested, we want the deepest
  #     to have precedent
  current_settings = dict()
  while len(configs) > 0:
    overwriting_settings = configs.pop()
    merge_settings(current_settings, overwriting_settings)

  return current_settings


def merge_settings(base_settings, new_settings):
  for key in new_settings:
    base_settings[key] = new_settings[key]


#+------------------------------------------------------------------------------+
#| Runner for the tests and tally for test output                               |
#+------------------------------------------------------------------------------+

def run_test(source_file, base_directory):
  test_directory = PATH.split(source_file)[0]
  
  # get test settings
  settings = get_settings_from_directory(test_directory, base_directory)
  get_settings_from_source(source_file, settings)

  try:
    start = time()
    output = run_step(sat, [source_file])
    end = time()
  except SP.CalledProcessError as e:
    end = time()
    if e.returncode == -2: # control-c killed it
      return
    if e.returncode == 124: # timed out
      return tally_output(source_file, 'TIMED OUT', settings, end-start)
    if e.returncode == 134: # assertion failure
      return tally_output(source_file, 'ASSERTION FAILURE', settings, end-start)
    if e.returncode == 139: # segfault
      return tally_output(source_file, 'RUN FAILED SEGFAULT', settings, end-start)
    if e.returncode == 127: # no executable
      return tally_output(source_file, 'RUN FAILED EXECUTABLE NOT FOUND', settings, end-start)
    # unknown
    return tally_output(source_file, 'RUN FAILED: UNKNOWN ({})'.format(e.returncode), settings, end-start)

  # check output
  return tally_output(source_file, output, settings, end-start)


def run_step( cmd, args):
  command = cmd+" "+" ".join(args)
  with SP.Popen(command,
                stdout=SP.PIPE, stderr=SP.STDOUT, shell=True) as proc:
    output = proc.stdout.read().decode("utf-8")
    proc.wait()

    if proc.returncode != 0:
      raise SP.CalledProcessError(proc.returncode, command)
    return output


def tally_output(source_file, output, settings, time):
  tally = dict()
  tally["source"] = source_file
  tally["time"] = time
  tally['expected'] = settings["expected"]
  tally['actual'] = output

  return tally


def cnf_strip(text):
  if "ERROR" in text:
    return "ERROR"
  if "UNSATISFIABLE" in text:
    return "UNSATISFIABLE"
  if "SATISFIABLE" in text:
    return "SATISFIABLE"
  return text

passed = 0
failed = 0
def main_thread_sum_output(tally_dict):
  global passed, failed

  printout = ["-"*80,
              tally_dict["source"],
              "Time: {}".format(tally_dict['time'])]

  if "no leaks are possible" in tally_dict['actual']:
    pass_fail = True
    passed += 1
  else:
    pass_fail = False
    failed += 1

  printout.append("Expected: {}".format(tally_dict['expected']))
  printout.append("Actual: {}".format(tally_dict['actual']))

  printout += [" Status: {}".format(green("pass") if pass_fail else red("fail"))]
  printout += [" Completed: {} / {}".format(passed+failed, total_tests)]
  printout += ["", ""]

  if True: #pass_fail == False:
    LOG.info('\n'.join(printout))



def parse_command_args():

  parser = AP.ArgumentParser()

  parser.add_argument("-d", "--directory", action="store", dest="test_directory", type=str,
                      help="sets the test directory (current by default)")
  parser.add_argument("-p", "--processes",
                      help="number of processes to use",
                      type= int)
  parser.add_argument("--log", action="store", dest="log_level", default="DEBUG", type=str,
                      help="sets the logging level (DEBUG, INFO, WARNING)")
  parser.add_argument("--output-log", action="store", dest="log_path", type=str,
                      help="sets the output log path. (std out by default)")

  # actually parse
  args = parser.parse_args()

  log_format = ''
  log_level = LOG.DEBUG
  if args.log_level.upper() == "INFO":
    log_level = LOG.INFO
  elif args.log_level.upper() == "WARNING":
    log_level = LOG.WARNING

  if not args.processes:
    try:
      args.processes = MP.cpu_count()
    except:
      args.processes = 1
  # if the user supplied a log path, write the logs to that file.
  # otherwise, write the logs to std out.
  if args.log_path:
    LOG.basicConfig(filename=args.log_path, format=log_format, level=log_level)
  else:
    LOG.basicConfig(format=log_format, level=log_level)

  if args.test_directory:
    args.test_directory = PATH.abspath(args.test_directory)
  else:
    args.test_directory = PATH.split(PATH.abspath(__file__))[0]


  return {"processes" : args.processes,
          "test_directory" : args.test_directory}


sat = "valgrind --leak-check=full ../sat"
total_tests = 0
def main():
  global total_tests
  # parse command line args
  arg_dict = parse_command_args()
  this_path = arg_dict["test_directory"]

  # setup thread pool
  LOG.debug("Creating Pool with '{}' Workers".format(arg_dict["processes"]))
  pool = MPP.ThreadPool(processes=arg_dict["processes"])

  # run thread pool
  #  catch control-c
  start = time()
  try:
    LOG.info("Running tests")

    # start processing the tests.
    results = []
    tests = sorted(GLOB.glob(PATH.join(this_path,"**/*.cnf")) +
                       GLOB.glob(PATH.join(this_path,"*.cnf")))
    total_tests = len(tests)
    for test in tests:
      r = pool.apply_async(run_test,
                           args=(PATH.abspath(test), this_path),
                           callback=main_thread_sum_output)
      results.append(r)

    # keep the main thread active while there are active workers
    for r in results:
      r.wait()

  except KeyboardInterrupt:
    print("Testing exited by control-c")
    print("Quitting now")
    pool.terminate()
  finally:
    end = time()
    pool.close()
    pool.join()
    LOG.info("".format(passed))
    LOG.info("Tests passed: {}".format(passed))
    LOG.info("Tests failed: {}".format(failed))
    LOG.info("Total tests completed: {} / {}".format(failed+passed, total_tests))
    LOG.info("Total time: {}".format(end-start))


if __name__ == "__main__":
  main()

